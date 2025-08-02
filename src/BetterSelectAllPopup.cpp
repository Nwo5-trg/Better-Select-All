#include "BetterSelectAllPopup.hpp"

using namespace geode::prelude;

BetterSelectAllPopup* BetterSelectAllPopup::create() { 
    auto ret = new BetterSelectAllPopup();
    if (ret->initAnchored(300.0f, 225.0f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool BetterSelectAllPopup::setup() {
    this->setTitle("Better Select All", "goldFont.fnt");

    m_editor = LevelEditorLayer::get();
    m_ui = m_editor->m_editorUI;

    auto buttonMenu = CCMenu::create();
    buttonMenu->setContentSize({150.0f, 150.0f});
    buttonMenu->setAnchorPoint({0.0f, 0.5f});
    buttonMenu->setPosition(m_size / 2);
    m_mainLayer->addChild(buttonMenu);

    CCPoint buttonPositions[] {
        {-50.0f, 50.0f}, {0.0f, 50.0f}, {50.0f, 50.0f},
        {-50.0f, 0.0f}, {0.0f, 0.0f}, {50.0f, 0.0f},
        {-50.0f, -50.0f}, {0.0f, -50.0f}, {50.0f, -50.0f},
    };

    const char* buttonSprites[] {
        "northwest.png"_spr, "north.png"_spr, "northeast.png"_spr,
        "west.png"_spr, "center.png"_spr, "east.png"_spr,
        "southwest.png"_spr, "south.png"_spr, "southeast.png"_spr
    };

    for (int i = 0; i < 9; i ++) {
        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(CCSprite::create(buttonSprites[i]), 100, true, 100, "GJ_button_04.png", 1),
            this, menu_selector(BetterSelectAllPopup::onSelectButton)
        );
        button->setPosition(buttonPositions[i]);
        button->setTag(i);
        buttonMenu->addChild(button);
    }

    auto modeToggler = createToggler(
        {m_size.width - 35.0f, 35.0f}, 
        Mod::get()->getSavedValue<bool>("select-object-mode"), 
        menu_selector(BetterSelectAllPopup::onModeToggle)
    );
    m_selectObjectMode = modeToggler->m_toggled;

    m_modeLabel = createLabel({m_size.width - 35.0f, 55.0f}, 0.4f, "");
    updateModeLabel();

    createToggler({35.0f, 35.0f}, false, menu_selector(BetterSelectAllPopup::onBypassAllToggle));

    createLabel({35.0f, 55.0f}, 0.4f, "Bypass\nAll");

    return true;
}

CCMenuItemToggler* BetterSelectAllPopup::createToggler(CCPoint pos, bool toggle, SEL_MenuHandler callback) {
    auto toggler = CCMenuItemToggler::create(
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), 
        this, callback
    );
    toggler->setPosition(pos);
    toggler->toggle(toggle);
    m_buttonMenu->addChild(toggler);
    return toggler;
} 

CCLabelBMFont* BetterSelectAllPopup::createLabel(CCPoint pos, float scale, const std::string& text) {
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setPosition(pos);
    label->setScale(scale);
    label->setAnchorPoint({0.5f, 0.0f});
    m_mainLayer->addChild(label);
    return label;
}

void BetterSelectAllPopup::onSelectButton(CCObject* sender) {
    int tag = sender->getTag();

    auto center = ccp(0.0f, 0.0f);
    if (m_selectObjectMode) {
        auto objs = m_ui->getSelectedObjects();
        if (objs->count() == 0 && tag != 4) {
            onClose(nullptr);
            return;
        }
        center = m_ui->getGroupCenter(objs, true);
    } else {
        auto size = CCDirector::get()->getWinSize();
        float zoom = m_editor->m_objectLayer->getScale();
        center = ( // im not even gonna pretend to know why i have to offset by 45 instead of 90 here
            (size / 2) - ccp(m_editor->m_objectLayer->getPositionX(), m_editor->m_objectLayer->getPositionY() - 45)
        ) / zoom;
    }

    m_selectFilter = GameManager::get()->getIntGameVariable("0005");
    getLockedLayers();

    CCPoint directions[] = {
        {-1.0f,  1.0f}, {0.0f,  1.0f}, {1.0f,  1.0f},
        {-1.0f,  0.0f}, {0.0f,  0.0f}, {1.0f,  0.0f},
        {-1.0f, -1.0f}, {0.0f, -1.0f}, {1.0f, -1.0f}
    };
    auto dir = directions[tag];

    auto objs = m_editor->m_objects;
    auto array = CCArray::create();

    for (auto obj : CCArrayExt<GameObject>(objs)) {
        if (tag != 4) {
            auto pos = obj->getPosition() - center;
            bool x = dir.x == 0 || pos.x * dir.x >= 0;
            bool y = dir.y == 0 || pos.y * dir.y >= 0;
            if (!x || !y) continue;
        }

        if (!m_bypassAll && !canSelect(obj)) continue;

        array->addObject(obj);
    }

    m_ui->selectObjects(array, false);
    m_ui->updateButtons();
    m_ui->updateObjectInfoLabel();

    onClose(nullptr);
}

bool BetterSelectAllPopup::canSelect(GameObject* obj) {
    if (obj->m_objectID == m_selectFilter) return false; 
    int layer1 = obj->m_editorLayer;
    int layer2 = obj->m_editorLayer2;
    if (m_lockedLayers[layer1] || m_lockedLayers[layer2]) return false;
    if (m_currentLayer != -1 && layer1 != m_currentLayer && layer2 != m_currentLayer) return false;
    return true;
}

void BetterSelectAllPopup::getLockedLayers() {
    const auto& layers = m_editor->m_lockedLayers;
    auto size = layers.size();
    for (int i = 0; i < size; i++) {
        m_lockedLayers.insert({i, layers[i]});
    }
}

void BetterSelectAllPopup::onModeToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    Mod::get()->setSavedValue<bool>("select-object-mode", !toggler->m_toggled);
    m_selectObjectMode = !toggler->m_toggled;
    updateModeLabel();
}

void BetterSelectAllPopup::onBypassAllToggle(CCObject* sender) {
    m_bypassAll = !m_bypassAll;
}

void BetterSelectAllPopup::updateModeLabel() {
    m_modeLabel->setString(m_selectObjectMode ? "Selected\nObject" : "Screen\nCenter");
}
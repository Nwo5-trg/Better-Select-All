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

    auto buttonMenu = CCMenu::create();
    buttonMenu->setContentSize({150.0f, 150.0f});
    buttonMenu->setAnchorPoint({0.0f, 0.5f});
    buttonMenu->setPosition(m_size / 2);
    m_mainLayer->addChild(buttonMenu);

    CCPoint buttonPositions[] {
        ccp(-50.0f, 50.0f), ccp(0.0f, 50.0f), ccp(50.0f, 50.0f),
        ccp(-50.0f, 0.0f), ccp(0.0f, 0.0f), ccp(50.0f, 0.0f),
        ccp(-50.0f, -50.0f), ccp(0.0f, -50.0f), ccp(50.0f, -50.0f),
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

    auto modeToggler = CCMenuItemToggler::create(
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), 
        this, menu_selector(BetterSelectAllPopup::onModeToggle)
    );
    modeToggler->setPosition(m_size.width - 35.0f, 35.0f);
    modeToggler->toggle(Mod::get()->getSavedValue<bool>("select-object-mode"));
    selectObjectMode = modeToggler->m_toggled;
    m_buttonMenu->addChild(modeToggler);

    modeLabel = CCLabelBMFont::create("", "bigFont.fnt");
    modeLabel->setPosition(m_size.width - 35.0f, 55.0f);
    modeLabel->setScale(0.4f);
    modeLabel->setAnchorPoint({0.5f, 0.0f});
    m_mainLayer->addChild(modeLabel);
    updateModeLabel();

    return true;
}

void BetterSelectAllPopup::onSelectButton(CCObject* sender) {
    auto* editor = LevelEditorLayer::get();
    
    int tag = sender->getTag();

    auto center = ccp(0.0f, 0.0f);
    if (selectObjectMode) {
        auto objs = editor->m_editorUI->getSelectedObjects();
        if (objs->count() == 0 && tag != 4) {
            onClose(nullptr);
            return;
        }
        center = editor->m_editorUI->getGroupCenter(objs, true);
    } else {
        auto size = CCDirector::get()->getWinSize();
        float zoom = editor->m_objectLayer->getScale();
        center = ( // im not even gonna pretend to know why i have to offset by 45 instead of 90 here
            (size / 2) - ccp(editor->m_objectLayer->getPositionX(), editor->m_objectLayer->getPositionY() - 45)
        ) / zoom;
    }

    auto objs = editor->m_objects;
    auto array = CCArray::create();
    
    // ik i could refactor this or optimize or wtv but i dont care enough
    switch (tag) {
        case 0: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() <= center.x && obj->getPositionY() >= center.y) array->addObject(obj);
            } break;
        }
        case 1: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionY() >= center.y) array->addObject(obj);
            } break;
        }
        case 2: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() >= center.x && obj->getPositionY() >= center.y) array->addObject(obj);
            } break;
        }
        case 3: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() <= center.x) array->addObject(obj);
            } break;
        }
        case 4: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                array->addObject(obj);
            } break;
        }
        case 5: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() >= center.x) array->addObject(obj);
            } break;
        }
        case 6: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() <= center.x && obj->getPositionY() <= center.y) array->addObject(obj);
            } break;
        }
        case 7: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionY() <= center.y) array->addObject(obj);
            } break;
        }
        case 8: {
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                if (obj->getPositionX() >= center.x && obj->getPositionY() <= center.y) array->addObject(obj);
            } break;
        }
    }

    editor->m_editorUI->selectObjects(array, true);
    editor->m_editorUI->updateButtons();
    editor->m_editorUI->updateObjectInfoLabel();

    onClose(nullptr);
}

void BetterSelectAllPopup::onModeToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    Mod::get()->setSavedValue<bool>("select-object-mode", !toggler->m_toggled);
    selectObjectMode = !toggler->m_toggled;
    updateModeLabel();
}

void BetterSelectAllPopup::updateModeLabel() {
    modeLabel->setString(selectObjectMode ? "Selected\nObject" : "Screen\nCenter");
}
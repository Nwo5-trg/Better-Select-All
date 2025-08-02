class BetterSelectAllPopup : public geode::Popup<> {
private:
    bool m_selectObjectMode;
    bool m_bypassAll = false;

    cocos2d::CCLabelBMFont* m_modeLabel;

    std::unordered_map<int, bool> m_lockedLayers;

    int m_selectFilter;
    int m_currentLayer;
    
    LevelEditorLayer* m_editor;
    EditorUI* m_ui;
    
    void onSelectButton(cocos2d::CCObject* sender);
    void onModeToggle(cocos2d::CCObject* sender);
    void onBypassAllToggle(cocos2d::CCObject* sender);
    void updateModeLabel();

    CCMenuItemToggler* createToggler(cocos2d::CCPoint pos, bool toggle, cocos2d::SEL_MenuHandler callback);
    cocos2d::CCLabelBMFont* createLabel(cocos2d::CCPoint pos, float scale, const std::string& text);

    bool canSelect(GameObject* obj);
    void getLockedLayers();

public:
    static BetterSelectAllPopup* create();
    bool setup() override;

};
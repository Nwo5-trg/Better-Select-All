class BetterSelectAllPopup : public geode::Popup<> {
private:
    bool selectObjectMode;
    cocos2d::CCLabelBMFont* modeLabel;
    
    void onSelectButton(cocos2d::CCObject* sender);
    void onModeToggle(cocos2d::CCObject* sender);
    void updateModeLabel();

public:
    static BetterSelectAllPopup* create();
    bool setup() override;

};
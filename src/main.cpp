#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include "BetterSelectAllPopup.hpp"

using namespace geode::prelude;

class $modify(EditorPauseLayer) {
    void onSelectAll(CCObject* sender) {
        BetterSelectAllPopup::create()->show();
    }
};
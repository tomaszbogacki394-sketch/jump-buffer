#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(BufMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto spr = CircleButtonSprite::createWithSprite(
            "logo.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium
        );
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(BufMenuLayer::onJumpBuffer)
        );
        btn->setID("jump-buffer-button"_spr);

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }

    void onJumpBuffer(CCObject*) {
        // opens this mod's settings: Enabled, Buffer window (ms), Practice only
        openSettingsPopup(Mod::get(), false);
    }
};

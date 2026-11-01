#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace {
    struct SideState {
        bool pending = false;     // a press is waiting for landing
        float age = 0.f;          // seconds since that press
        bool held = false;        // is the real button down
        bool releaseNext = false; // we injected a press for a released button
    };

    struct State {
        SideState side[2];
        bool injecting = false;
    } g;

    bool bufferActive() {
        auto mod = Mod::get();
        if (!mod->getSettingValue<bool>("enabled")) return false;
        auto pl = PlayLayer::get();
        if (!pl) return false;
        if (pl->m_levelSettings && pl->m_levelSettings->m_platformerMode) return false;
        if (mod->getSettingValue<bool>("practice-only") && !pl->m_isPracticeMode) return false;
        return true;
    }

    bool isCube(PlayerObject* p) {
        return !(p->m_isShip || p->m_isBird || p->m_isBall || p->m_isDart ||
                 p->m_isRobot || p->m_isSpider || p->m_isSwing);
    }
}

class $modify(BufLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        GJBaseGameLayer::handleButton(down, button, isPlayer1);

        if (g.injecting || button != 1 || !bufferActive()) return;

        auto& s = g.side[isPlayer1 ? 0 : 1];
        s.held = down;
        if (!down) return;

        auto* p = isPlayer1 ? m_player1 : m_player2;
        if (p && !p->m_isOnGround && isCube(p)) {
            s.pending = true;
            s.age = 0.f;
        } else {
            s.pending = false;
        }
    }
};

class $modify(BufPlayer, PlayerObject) {
    void update(float dt) {
        auto* gl = m_gameLayer;
        if (gl && !g.injecting && bufferActive()) {
            bool p1 = this == gl->m_player1;
            bool p2 = this == gl->m_player2;
            if (p1 || p2) {
                auto& s = g.side[p1 ? 0 : 1];

                // finish a buffered tap: release on the step after the jump
                if (s.releaseNext) {
                    s.releaseNext = false;
                    if (!s.held) {
                        g.injecting = true;
                        gl->handleButton(false, 1, p1);
                        g.injecting = false;
                    }
                }

                if (s.pending) {
                    s.age += dt;
                    float window = Mod::get()->getSettingValue<int64_t>("buffer-ms") / 1000.f;

                    if (!isCube(this) || s.age > window) {
                        s.pending = false;
                    } else if (m_isOnGround) {
                        s.pending = false;
                        // holding already re-jumps in the game; only inject for released presses
                        if (!s.held) {
                            g.injecting = true;
                            gl->handleButton(true, 1, p1);
                            g.injecting = false;
                            s.releaseNext = true;
                        }
                    }
                }
            }
        }
        PlayerObject::update(dt);
    }
};

class $modify(BufPlayLayer, PlayLayer) {
    void resetLevel() {
        g = State();
        PlayLayer::resetLevel();
    }
};

#include <imgui.h>
#include <imgui_internal.h>
#include <EspLines\Memory\Memory.hpp>
#include <EspLines\Math\Vector\Vector3.hpp>
#include <EspLines\Math\Vector\Vector2.hpp>
#include <EspLines\Aimbot\Aimbot.hpp> 
#include <src\Globals.hpp>
#include <EspLines\Offsets.hpp>
#include <EspLines\Math\WordToScreen.hpp>

std::atomic<bool> aimbotRunning{ false };
std::thread aimbotThread;
static bool lastKeyState = false;
static Player* lastTarget = nullptr;

Vector3 Aim::Aimbot::GetHitBoxPosition(const Player& entity) {
    using HitBox = Config::HitBox;

    switch (g_Globals.AimBot.HitBox) {
    case HitBox::Neck: return entity.Neck;
    case HitBox::Chest: return entity.Hip;
    case HitBox::Head: return entity.Head;
    default:            return entity.Head;
    }
}


Player* Aim::Aimbot::FindClosestEnemy() {
    float closestDistance = FLT_MAX;
    Player* closestEntity = nullptr;

    Vector2 screenCenter(g_Globals.EspConfig.Width / 2.0f, g_Globals.EspConfig.Height / 2.0f);

    for (auto& pair : g_Globals.EspConfig.Entities) {
        Player* entity = &pair.second;

        if (entity->IsDead || (g_Globals.AimBot.IgnoreKnocked && entity->IsKnocked)) continue;

        Vector3 targetHitBox = GetHitBoxPosition(*entity);
        ImVec2 hitBox2D = W2S::WorldToScreenImVec2(g_Globals.EspConfig.ViewMatrix, targetHitBox, g_Globals.EspConfig.Width, g_Globals.EspConfig.Height);
        if (hitBox2D.x < 1 || hitBox2D.y < 1) continue;

        float distance = Vector3::Distance(g_Globals.EspConfig.MainCamera, targetHitBox);
        if (distance > g_Globals.AimBot.DistanceAim) continue;

        float crosshairDist = std::sqrt(std::pow(hitBox2D.x - screenCenter.X, 2) + std::pow(hitBox2D.y - screenCenter.Y, 2));
        if (crosshairDist > g_Globals.AimBot.Fov || crosshairDist >= closestDistance) continue;

        closestDistance = crosshairDist;
        closestEntity = entity;
    }

    return closestEntity;
}

void Aim::Aimbot::LegitAimbot()
{
    if (!g_Globals.AimBot.Enabled) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        return;
    }

    bool keyHeld = (GetAsyncKeyState(g_Globals.AimBot.AimbotBind) & 0x8000) != 0;
    if (!keyHeld) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        return;
    }

    if (g_Globals.EspConfig.Width <= 0 || g_Globals.EspConfig.Height <= 0 || !g_Globals.EspConfig.Matrix) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        return;
    }

    Player* target = FindClosestEnemy();
    if (!target || target->Address == 0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(5));
        return;
    }

    uintptr_t headColliderAddr = target->Address + Offsets::ColliderHECFNHJKOMN;
    uintptr_t lockedAimAddr = target->Address + Offsets::ColliderINICDNFOFJB;

    uint32_t collider = 0;
    if (!Mem.ReadFast2<uint32_t>(headColliderAddr, &collider) || collider == 0)
        return;

    Mem.Write<uint32_t>(lockedAimAddr, 0UL);
    Mem.Write<uint32_t>(lockedAimAddr, collider);
    std::this_thread::sleep_for(std::chrono::milliseconds(g_Globals.AimBot.UpdateInterval));
    Mem.Write<uint32_t>(lockedAimAddr, 0UL);

    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}


void Aim::Aimbot::StartAimbot() {
    if (aimbotRunning) return;
    aimbotRunning = true;

    aimbotThread = std::thread([] {
        while (aimbotRunning) {
            Aim::Aimbot::LegitAimbot();
        }
        });
    aimbotThread.detach();
}

void Aim::Aimbot::StopAimbot() {
    aimbotRunning = false;
    if (aimbotThread.joinable()) {
        aimbotThread.join();
    }
}
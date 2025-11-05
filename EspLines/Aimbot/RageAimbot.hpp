#pragma once
#include <cstdint>
#include <EspLines/Math/Vector/Vector3.hpp>
#include <EspLines/Player.h>
#include <src/Globals.hpp>
#include <EspLines/Memory/Memory.hpp>

namespace Aim {
    class RageAimbot {
    public:
        static void Aimbot();
        static void StartAimbot();
        static void StopAimbot();
    private:
        static bool EntityData(uint32_t entity, Player& player, Vector3& mainPos);
        static Vector3 GetHitBoxPosition(const Player& entity);
    };
}
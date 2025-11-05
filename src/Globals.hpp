#pragma once
#include <Windows.h>
#include <EspLines/Player.h>
#include <EspLines/Math/Matrix4v4.hpp>
#include <unordered_map>
#include <mutex>

namespace Config {
    static float CornerHeight = 20.0f;

    enum class HitBox {
        Head,
        Neck,
        Chest
    };

    enum class Priority {
        High,
        Normal,
        Low
    };
}

class Globals {
public:
    struct AimBot {
        bool Enabled;
        bool AimLockActive = false;
        bool AimLock;
        bool AimLock2x;
        bool AimLock2xActive = false;
        bool FastFireActive = false;
        bool AimShoulder;
        bool NoRecoil;
        bool FastReload;
        bool FastFire;
        bool AimbotFFMAX;
        bool IgnoreKnocked;
        bool IgnoreBots;
        int DistanceAim = 50;
        int Fov = 50.0f;
        float Fillcolor[4] = { 0.f, 0.f, 0.f, 0.2f };
        int AimbotBind = VK_LBUTTON;
        bool MouseAim;
        int MouseAimBind = 0;
        int UpdateInterval = 100;
        int ShotDelay = 100;
        float Smoothing = 0.5f;
        Config::HitBox HitBox = Config::HitBox::Head;
    }
    AimBot;

    struct Silent {
        bool Enabled;
        bool IgnoreKnocked;
        bool IgnoreBots;
        int HeadRate = 0;
        int ChestRate = 2;
        float Fillcolor[4] = { 0.f, 0.f, 0.f, 0.2f };
        int SilentBind = 0;
        int MaxDistance = 50;
        int UpdateInterval = 10;
        int DistanceAim = 50;
        int Fov = 50.0f;
        Config::HitBox HitBox = Config::HitBox::Head;
    }
    Silent;

    struct Exploits {
        bool UpPlayer;
        float Delay = 1.0f;
        float Distance = 1.0f;
        int UpPlayerBind = 0;
        bool DownPlayer;
        bool TeleportCar;
        int DownPlayerBind = 0;
        bool TpWall;
        int TpWallBind = 0;
        bool TeleKill;
        bool GodMode;
        int TeleKillBind = 0;
        bool WallHack;
        bool HighJump;
        bool InfiniteAmmo;
        bool TpWay;
        bool Magnetic;
        int MagneticBind = 0;
        bool DamageBoost;
        bool FlyHack;
        int WallHackBind = 0;
        int UnderCamBind = 0;
        int TeleportCarBind = 0;
        bool DownLocalPlayer;
        bool Telepneu;
        int DownLocalPlayerBind = 0;

        bool InfiniteAmmoActive = false;
        bool HighJumpActive = false;
        bool FlyHackActive = false;
        bool EnemyPullEnabled = false;
        int EnemyPullBind = 0;
        int EnemyPullStrength = 1;
        int EnemyPullMaxDistance = 50;
        int EnemyPullTickMs = 6;
        float PullHeightOffset = -1.4f;
    }
    Exploits;

    struct Visuals
    {
        float x, y;

        float LeftKneeOffset = -0.25f;
        float RightKneeOffset = -0.35f;

        float TextSize = 12.0f;
        float LogoScale = 0.60f;
        float Thickness = 1.0f;
        int DistanceEsp = 60;
        bool Enable = true;
        bool Observe = false;
        bool Watermark = false;
        bool Enemy = false;
        bool Lines = false;
        int EspLines = 0;
        float LinesColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float KnockedColor[4] = { 0.184f, 0.424f, 0.6f, 1.0f }; // Azul 
        bool FilledBox = false;
        float Filledboxcolor[4] = { 0.f, 0.f, 0.f, 0.4f };

        bool Box = false;
        int players_box = 0;
        float BoxColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool ESPHealthTEXT;
        bool HealthBar = false;
        int players_healthbar = 0;
        float texthColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool ESPWeapon = false;
        bool ESPWeaponIcon = false;
        float ESPWeaponColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool Skeleton = false;
        float SkeletonColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool Alvo = false;
        float AlvoColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool Name = false;
        float NameColor[4] = { 1.f, 1.f, 1.f, 1.f };

        bool Distance = false;
        float DistColor[4] = { 0.7f, 0.7f, 0.7f, 1.f };

        bool ESPGranada = false;
        float ESPGranadaColor[4] = { 0.184f, 0.424f, 0.6f, 1.0f }; // Azul 

        bool RadarHack = false;

        float WatermarkColor[4] = { 0.184f, 0.424f, 0.6f, 1.0f }; // Azul 
        float EnemyColor[4] = { 0.184f, 0.424f, 0.6f, 1.0f }; // Azul 

        bool Debug = false;
        float HipHeightOffset = 0.7f;
        float HipWidthOffset = 0.33f;
        float HipWidthScale = 0.19f;
        float LeftHipHeightOffset = 0.82f;
        float RightHipHeightOffset = 0.98f;

        float LeftKneeHeightOffset = 0.5f;
        float RightKneeHeightOffset = 0.5f;
        float KneeWidthScale = 0.5f;
        float LeftKneeWidthOffset = 0.1f;
        float RightKneeWidthOffset = 0.1f;
        float LeftKneeFlexionOffset = 0.1f;
        float RightKneeFlexionOffset = 0.1f;
    } Visuals;

    struct Misc
    {
        bool ShowAimbotFov;
        float AimbotFovColor[4] = { 0.184f, 0.424f, 0.6f, 1.0f }; // Azul 
    } Misc;

    struct General
    {
        char Username[255] = "";
        char Password[255] = "";
        char Key[255] = "";
        char License[255] = "";
        bool ShutDown = false;
        bool Capture = false;
        Config::Priority Priority = Config::Priority::Normal;
        int Delay = 0;
        int MenuKey = VK_INSERT;
    } General;

    struct Esp {
        std::unordered_map<long, Player> Entities;
        std::mutex EntitiesMutex;
        Matrix4x4 ViewMatrix{};
        Vector3 MainCamera{};
        uint32_t LocalPlayer = 0;
        uint32_t previousCount = 0;
        bool Matrix = false;
        int Width = 0;
        int Height = 0;
    } EspConfig;

};

inline Globals g_Globals;

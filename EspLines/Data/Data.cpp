#include "Data.hpp"
#include <src/Globals.hpp>
#include <EspLines/Memory/Memory.hpp>
#include <EspLines/Offsets.hpp>
#include <EspLines/Math/TMatrix.hpp>
#include <map>
#include <EspLines/Math/Vector/Vector2.hpp>
#include <EspLines/Math/WordToScreen.hpp>
#include <EspLines/Math/AimB.hpp>
#include <EspLines\Aimbot\Aimbot.hpp>
#include <EspLines\Aimbot\RageAimbot.hpp>
#include <EspLines/Exploits/Exploit/UpPlayer.hpp>
#include <EspLines/Exploits/Exploit/DownPlayer.hpp>
#include <EspLines/Exploits/Others/TeleKill.hpp>
#include <EspLines/Exploits/Exploit/EnemyPull360.hpp>
#include <EspLines/Exploits/Exploit/Telepneu.hpp>
#include <EspLines/Exploits/Exploit/DownLocalPlayer.hpp>
#include <EspLines/Exploits/Exploit/GhostHack.hpp>
#include <Psapi.h>

namespace FWork {
    void Data::Work() {
        uint32_t playerAttr = 0;
        GameContext ctx;

        ctx.currentGame = GetCurrentGame();
        if (!ctx.currentGame) {
            if (!g_Globals.EspConfig.Entities.empty()) {
                g_Globals.EspConfig.Entities.clear();
            }
            Mem.Cache.clear();
            return;
        }

        ctx.currentMatch = GetCurrentMatch(ctx.currentGame);
        if (!ctx.currentMatch) {
            if (!g_Globals.EspConfig.Entities.empty()) {
                g_Globals.EspConfig.Entities.clear();
            }
            Mem.Cache.clear();
            return;
        }

        ctx.localPlayer = Mem.Read<uint32_t>(ctx.currentMatch + Offsets::LocalPlayer);
        if (!ctx.localPlayer || !SetupLocalPlayerAndCamera(ctx.currentMatch)) {
            if (!g_Globals.EspConfig.Entities.empty()) {
                g_Globals.EspConfig.Entities.clear();
            }
            Mem.Cache.clear();
            return;
        }
        ProcessEntities(ctx);

        //Aimbot Legit
        Aim::Aimbot::StartAimbot();

        //Aimbot Rage
        Aim::RageAimbot::Aimbot();

        //UpPlayer Function
        UpPlayer::UpPlayerr::UpPlayer();

        //DownPlayer Function
        DownPlayer::DownPlayerr::DownPlayer();

        //Magnetic Trigger Function
        EnemyPull::EnemyPull360::Start();

        //DownPlayer Function
        DownLocalPlayer::DownLocalPlayerr::DownLocal();

        //Tp To Me Function
        TeleKill::Start();

        //Teleport Way Function
        Telepneu::Telepneuu::TelepneuWork();

    }

    bool IsInsideFOV(const ImVec2& pos) {
        int centerX = g_Globals.EspConfig.Width / 2;
        int centerY = g_Globals.EspConfig.Height / 2;
        int radius = g_Globals.AimBot.Fov;

        int dx = (int)pos.x - centerX;
        int dy = (int)pos.y - centerY;

        return (dx * dx + dy * dy) <= (radius * radius);
    }

    uint32_t Data::GetCurrentGame() {
        if (Offsets::Il2Cpp == 0) return 0;

#ifdef FFMax
        // Implementa??o para quando FFMax est? definido
        uint32_t staticGameFacade = Mem.Read<uint32_t>(Mem.Read<uint32_t>(Offsets::Il2Cpp + Offsets::InitBase) + Offsets::StaticClass);
        if (!staticGameFacade) return 0;
        return Mem.Read<uint32_t>(staticGameFacade);
#else
        // Implementa??o padr?o quando FFMax n?o est? definido
        uint32_t baseGameFacade = Mem.Read<uint32_t>(Offsets::Il2Cpp + Offsets::InitBase);
        if (!baseGameFacade) return 0;

        uint32_t gameFacade = Mem.Read<uint32_t>(baseGameFacade);
        if (!gameFacade) return 0;

        uint32_t staticGameFacade = Mem.Read<uint32_t>(gameFacade + Offsets::StaticClass);
        if (!staticGameFacade) return 0;

        return Mem.Read<uint32_t>(staticGameFacade);
#endif
    }
    uint32_t Data::GetCurrentMatch(uint32_t currentGame) {
        if (!currentGame) return 0;

        uint32_t currentMatch = Mem.Read<uint32_t>(currentGame + Offsets::CurrentMatch);
        if (!currentMatch) return 0;

        uint32_t matchStatus = Mem.Read<uint32_t>(currentMatch + Offsets::MatchStatus);
        return (matchStatus == 1) ? currentMatch : 0;
    }

    bool Data::SetupLocalPlayerAndCamera(uint32_t currentMatch) {
        uint32_t localPlayer = Mem.Read<uint32_t>(currentMatch + Offsets::LocalPlayer);
        if (!localPlayer) return false;

        g_Globals.EspConfig.LocalPlayer = localPlayer;

        uint32_t mainTransform = Mem.Read<uint32_t>(localPlayer + Offsets::MainCameraTransform);
        if (!mainTransform) return false;

        Vector3 mainPos;
        TransformUtils::GetPosition(mainTransform, mainPos);
        g_Globals.EspConfig.MainCamera = mainPos;

        uint32_t followCamera = Mem.Read<uint32_t>(localPlayer + Offsets::FollowCamera);
        if (!followCamera) return false;

        uint32_t camera = Mem.Read<uint32_t>(followCamera + Offsets::Camera);
        if (!camera) return false;

        uint32_t cameraBase = Mem.Read<uint32_t>(camera + 0x8);
        if (!cameraBase) return false;

        Matrix4x4 viewMatrix = Mem.Read<Matrix4x4>(cameraBase + Offsets::ViewMatrix);
        g_Globals.EspConfig.ViewMatrix = viewMatrix;
        g_Globals.EspConfig.Matrix = true;
        
        // Set screen resolution for ESP
        g_Globals.EspConfig.Width = GetSystemMetrics(SM_CXSCREEN);
        g_Globals.EspConfig.Height = GetSystemMetrics(SM_CYSCREEN);

        uint32_t playerAttr = 0;
        if (Mem.ReadFast2<uint32_t>(localPlayer + Offsets::PlayerAttributes, &playerAttr) && playerAttr != 0) {
            if (g_Globals.Exploits.InfiniteAmmo) {
                Mem.Write<bool>(playerAttr + 0x88, true);
                g_Globals.Exploits.InfiniteAmmoActive = true;
            }
            else if (g_Globals.Exploits.InfiniteAmmoActive) {
                Mem.Write<bool>(playerAttr + 0x88, false);
                g_Globals.Exploits.InfiniteAmmoActive = false;
            }
        }

        // Aimlock Mira 2x
        uint32_t weapon = 0;
        if (Mem.ReadFast2<uint32_t>(localPlayer + Offsets::Weapon, &weapon) && weapon != 0)
        {
            uint32_t attachmentData = 0;
            if (Mem.ReadFast2<uint32_t>(weapon + 0x4FC, &attachmentData) && attachmentData != 0)
            {
                uint32_t sightParams = 0;
                if (Mem.ReadFast2<uint32_t>(attachmentData + 0x84, &sightParams) && sightParams != 0)
                {
                    const float lockedValue = 1.90277084e17f;
                    const float defaultValue = 1.0f;

                    if (g_Globals.AimBot.AimLock2x)
                    {
                        if (!g_Globals.AimBot.AimLock2xActive)
                        {
                            Mem.Write<float>(sightParams + 0x5C, lockedValue);
                            g_Globals.AimBot.AimLock2xActive = true;
                        }
                    }
                    else if (g_Globals.AimBot.AimLock2xActive)
                    {
                        Mem.Write<float>(sightParams + 0x5C, defaultValue);
                        g_Globals.AimBot.AimLock2xActive = false;
                    }
                }
            }
        }

        // Aimlock
        weapon = 0;
        if (Mem.ReadFast2<uint32_t>(localPlayer + 0x348, &weapon) && weapon != 0) {
            if (g_Globals.AimBot.AimLock) {
                Mem.Write<float>(weapon + 0x3F0, -1.0f);
                g_Globals.AimBot.AimLockActive = true;
            }
            else if (g_Globals.AimBot.AimLockActive) {
                Mem.Write<float>(weapon + 0x3F0, 1.0f);
                g_Globals.AimBot.AimLockActive = false;
            }
        }

        // FastReload
        if (g_Globals.AimBot.FastReload) {
            uint32_t reload = 0;
            if (Mem.ReadFast2<uint32_t>(localPlayer + Offsets::PlayerAttributes, &reload) && reload != 0) {
                Mem.Write<bool>(reload + Offsets::NoReload2, true);
            }
        }
        else {
            uint32_t reload = 0;
            if (Mem.ReadFast2<uint32_t>(localPlayer + Offsets::PlayerAttributes, &reload) && reload != 0) {
                Mem.Write<bool>(reload + Offsets::NoReload2, false);
            }
        }

        return true;
    }

    void Data::ProcessEntities(const GameContext& ctx) {
        uint32_t entityDictionary = Mem.Read<uint32_t>(ctx.currentGame + Offsets::DictionaryEntities);
        if (!entityDictionary) return;

        uint32_t entities = Mem.Read<uint32_t>(entityDictionary + Offsets::Il2CppDictionaryDataPtr);
        if (!entities) return;

        entities += 0x10;

        uint32_t entitiesCount = Mem.Read<uint32_t>(entityDictionary + Offsets::Il2CppDictionaryCount);
        if (entitiesCount != g_Globals.EspConfig.previousCount) {
            g_Globals.EspConfig.previousCount = entitiesCount;
        }

        if (entitiesCount < 1) return;

        Vector3 mainPos = g_Globals.EspConfig.MainCamera;

        bool isInObserveMode = false;
        uint32_t observedPlayer = 0;

        if (g_Globals.EspConfig.LocalPlayer) {
            uint32_t currentObserver = 0;
            if (Mem.ReadFast2<uint32_t>(g_Globals.EspConfig.LocalPlayer + Offsets::CurrentObserver, &currentObserver)) {
                isInObserveMode = (currentObserver != 0);
                if (isInObserveMode) {
                    Mem.ReadFast2<uint32_t>(currentObserver + Offsets::ObserverPlayer, &observedPlayer);
                }
            }
        }

        for (uint32_t i = 0; i < entitiesCount; ++i) {
            uint32_t entity = Mem.Read<uint32_t>(entities + i * 0x4);

            if (entity == 0 || entity == ctx.localPlayer) continue;

            try {
                Player& player = g_Globals.EspConfig.Entities[entity];

                auto ProcessEntity = [&]() -> bool {

                    uint32_t avatarManager = Mem.Read<uint32_t>(entity + Offsets::AvatarManager);
                    uint32_t avatar = avatarManager ? Mem.Read<uint32_t>(avatarManager + Offsets::Avatar) : 0;
                    uint32_t avatarData = avatar ? Mem.Read<uint32_t>(avatar + Offsets::Avatar_Data) : 0;

                    if (!avatarData) return false;

                    player.IsVisible = Mem.Read<bool>(avatar + Offsets::Avatar_IsVisible);
                    bool isTeam = Mem.Read<bool>(avatarData + Offsets::Avatar_Data_IsTeam);

                    player.IsTeam = isTeam ? Player::Bool3::True : Player::Bool3::False;
                    player.IsKnown = !isTeam;

                    if (!isInObserveMode) {
                        if (!player.IsVisible || player.IsTeam == Player::Bool3::True || !player.IsKnown) {
                            return false;
                        }
                    }

                    if (!player.IsVisible || player.IsTeam == Player::Bool3::True || !player.IsKnown) {
                        return false;
                    }

                    uint32_t shadowBase = Mem.Read<uint32_t>(entity + Offsets::ShadowState);
                    player.IsKnocked = shadowBase ? (Mem.Read<int>(shadowBase + Offsets::XPose) == 8) : false;

                    player.IsDead = Mem.Read<bool>(entity + Offsets::Player_IsDead);
                    player.IsBot = Mem.Read<bool>(entity + Offsets::IsClientBot);

                    uint32_t dataPool = Mem.Read<uint32_t>(entity + Offsets::Player_Data);
                    if (!dataPool) return false;
                    uint32_t poolObj = Mem.Read<uint32_t>(dataPool + 0x8);
                    if (!poolObj) return false;
                    uint32_t pool = Mem.Read<uint32_t>(poolObj + 0x10);
                    if (!pool) return false;
                    uint32_t weaponptr = Mem.Read<uint32_t>(poolObj + 0x20);
                    if (!weaponptr) return false;

                    player.Health = Mem.Read<short>(pool + Offsets::Vida);
                    player.WeaponID = Mem.Read<short>(weaponptr + Offsets::Vida);

                    static const std::pair<uint32_t, Vector3 Player::*> boneOffsets[] = {
                        {Offsets::Bones::Head, &Player::Head},
                        {Offsets::Bones::Neck, &Player::Neck},
                        {Offsets::Bones::LeftShoulder, &Player::LeftShoulder},
                        {Offsets::Bones::RightShoulder, &Player::RightShoulder},
                        {Offsets::Bones::LeftElbow, &Player::LeftElbow},
                        {Offsets::Bones::RightElbow, &Player::RightElbow},
                        {Offsets::Bones::LeftWrist, &Player::LeftWrist},
                        {Offsets::Bones::RightWrist, &Player::RightWrist},
                        {Offsets::Bones::Hip, &Player::Hip},
                        {Offsets::Bones::Root, &Player::Root},
                        {Offsets::Bones::LeftAnkle, &Player::LeftAnkle},
                        {Offsets::Bones::RightAnkle, &Player::RightAnkle}
                    };

                    for (const auto& [offset, memberPtr] : boneOffsets) {
                        if (uint32_t boneAddress = 0; Mem.Read(entity + offset, boneAddress)) {
                            TransformUtils::GetNodePosition(boneAddress, player.*memberPtr);
                        }
                    }

                    if (player.Head != Vector3::Zero()) {
                        player.Distance = Vector3::Distance(mainPos, player.Head);
                    }

                    if (uint32_t nameAddr = 0; Mem.Read(entity + Offsets::Player_Name, nameAddr) && nameAddr) {
                        player.Name = Mem.String(nameAddr + 0xC, 128);
                    }

                    player.Address = entity;

                    return true;
                    };

                if (!ProcessEntity()) {
                    g_Globals.EspConfig.Entities.erase(entity);
                    continue;
                }
            }
            catch (...) {
                g_Globals.EspConfig.Entities.erase(entity);
                continue;
            }
        }
    }
}


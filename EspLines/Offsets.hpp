//#define FFMax

class Offsets {
public:
#ifdef FFMax
    static inline uintptr_t Il2Cpp;

    static inline uintptr_t InitBase = 0xA9EA0F4;
    static inline uintptr_t StaticClass = 0x5C;

    static inline uintptr_t CurrentMatch = 0x50;              // m_Match
    static inline uintptr_t MatchStatus = 0x3C;               // m_State
    static inline uintptr_t LocalPlayer = 0x44;               // m_LocalPlayer
    static inline uintptr_t DictionaryEntities = 0x68;        // m_ReplicationEntitis
    static inline uintptr_t Player_IsDead = 0x4C;             // m_IsDead
    static inline uintptr_t Player_Name = 0x22C;              // m_NickName
    static inline uintptr_t Player_Data = 0x44;               // m_PRIDataPool
    static inline uintptr_t Player_ShadowBase = 0x131C;       // m_ShadowState
    static inline uintptr_t XPose = 0x78;                     // TargetPhysXPose

    static inline uintptr_t AvatarManager = 0x3F8;            // m_AvatarManager
    static inline uintptr_t Avatar = 0x94;                    // m_Avatar
    static inline uintptr_t Avatar_IsVisible = 0x7C;          // IsVisible
    static inline uintptr_t Avatar_Data = 0x10;               // umaData
    static inline uintptr_t Avatar_Data_IsTeam = 0x49;        // isTeammate

    static inline uintptr_t FollowCamera = 0x38C;             // m_FollowCamera
    static inline uintptr_t Camera = 0x14;                    // m_TargetCamera
    static inline uintptr_t MainCameraTransform = 0x1B0;
    static inline uintptr_t ViewMatrix = 0xBC;
    static inline uintptr_t IsClientBot = 0x234;

    static inline uintptr_t ColliderHECFNHJKOMN = 0x3E0;
    static inline uintptr_t ColliderINICDNFOFJB = 0x50;
    static inline uintptr_t AimRotation;
    static inline uintptr_t Vida = 0xC;

    class Bones {
    public:
        static inline uintptr_t Head = 0x394;                 // HeadNode
        static inline uintptr_t Neck = 0x39C;
        static inline uintptr_t LeftShoulder = 0x3C8;
        static inline uintptr_t RightShoulder = 0x3CC;
        static inline uintptr_t LeftElbow = 0x3DC;
        static inline uintptr_t RightElbow = 0x3D8;
        static inline uintptr_t LeftWrist = 0x3D4;
        static inline uintptr_t RightWrist = 0x3D0;
        static inline uintptr_t Hip = 0x3A4;
        static inline uintptr_t RightAnkle = 0x3B4;
        static inline uintptr_t LeftAnkle = 0x3B0;
        static inline uintptr_t Root = 0x3A8;                 // m_RootNode
    };
#else
    static inline uintptr_t Il2Cpp = 0x0;
    static inline uintptr_t InitBase = 0x983FF10;
    static inline uintptr_t StaticClass = 0x5C;

    static inline uintptr_t Il2CppDictionaryDataPtr = 0x14;
    static inline uintptr_t Il2CppDictionaryCount = 0x18;
    static inline uintptr_t CurrentMatch = 0x50;
    static inline uintptr_t MatchStatus = 0x3C;
    static inline uintptr_t LocalPlayer = 0x44;
    static inline uintptr_t DictionaryEntities = 0x68;

    static inline uintptr_t Player_IsDead = 0x4C;
    static inline uintptr_t Player_Name = 0x24C;
    static inline uintptr_t Player_Data = 0x44;

    static inline uintptr_t ShadowState = 0x149C;
    static inline uintptr_t XPose = 0x78;

    static inline uintptr_t AvatarManager = 0x420;
    static inline uintptr_t Avatar = 0x94;
    static inline uintptr_t Avatar_IsVisible = 0x7C;
    static inline uintptr_t Avatar_Data = 0x10;
    static inline uintptr_t Avatar_Data_IsTeam = 0x51;

    static inline uintptr_t FollowCamera = 0x3B0;
    static inline uintptr_t Camera = 0x14;
    static inline uintptr_t AimRotation = 0x368;
    static inline uintptr_t AimRotationCheck = 0x378;
    static inline uintptr_t MainCameraTransform = 0x1BC;

    static inline uintptr_t Weapon = 0x35C;
    static inline uintptr_t WeaponData = 0x44;
    static inline uintptr_t WeaponRecoil = 0xC;
    static inline uintptr_t isfiring = 0x48C;

    static inline uintptr_t ViewMatrix = 0xBC;

    static inline uintptr_t IsClientBot = 0x254;

    static inline uintptr_t HeadCollider = 0x404;
    static inline uintptr_t LevelUp = 0x14A8;
    static inline uintptr_t PlayerAttributes = 0x41C;
    static inline uintptr_t No_Reload = 0x89;

    // Offsets adicionais
    static inline uintptr_t ColliderHECFNHJKOMN = 0x3E0;
    static inline uintptr_t ColliderINICDNFOFJB = 0x50;
    static inline uintptr_t GameTimer = 0x0;
    static inline uintptr_t RightcameraOffset = 0x0;
    static inline uintptr_t FOVcameraoffset = 0x0;
    static inline uintptr_t GameVariables = 0x0;
    static inline uintptr_t NoReload2 = 0x89;
    static inline uintptr_t CurrentObserver = 0x3B4;
    static inline uintptr_t ObserverPlayer = 0x3B8;
    static inline uintptr_t Player_ShadowBase = 0x149C;
    static inline uintptr_t Vida = 0xC;
    static inline uintptr_t tiro = 0x0;
    static inline uintptr_t GhostMode = 0x78;
    static inline uintptr_t telepneu = 0x78;
    static inline uintptr_t WeaponOnHand = 0x35C;

    class Bones {
    public:
        static inline uintptr_t Head = 0x3B8;
        static inline uintptr_t Neck = 0x3C0;
        static inline uintptr_t Hip = 0x3BC;
        static inline uintptr_t RightShoulder = 0x3F0;
        static inline uintptr_t LeftShoulder = 0x3EC;
        static inline uintptr_t LeftHand = 0x3E4;
        static inline uintptr_t RightHand = 0x3B4;
        static inline uintptr_t LeftAnkle = 0x3D4;
        static inline uintptr_t RightAnkle = 0x3D8;
        static inline uintptr_t Hand = 0x3B4;
        static inline uintptr_t Root = 0x3CC;
        static inline uintptr_t LeftFoot = 0x3DC;
        static inline uintptr_t RightFoot = 0x3E0;
        static inline uintptr_t RootBone = 0x3D0;
        static inline uintptr_t Groin = 0x3C8;
        static inline uintptr_t RightElbow = 0x3FC;
        static inline uintptr_t LeftElbow = 0x400;
        static inline uintptr_t RightWrist = 0x3F4;
        static inline uintptr_t LeftWrist = 0x3F8;
    };
#endif
};
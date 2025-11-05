#include "ui.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <src/Overlay/Overlay.hpp>
#include <src/Globals.hpp>
#include <src/Fonts/FontInter.hpp>
#include <imgui_internal.h>
#include <src/Fonts/FontAwesome.hpp>
#include <src/Fonts/Fonts.hpp>
#include <thread>
#include <Auth/auth.hpp>
#define CURL_STATICLIB
#include "Auth/Curl/curl.h"
#include "src\Fonts\Fonts.hpp"
#include <FontAwesome6.hpp>
#include "Logo.hpp"
#include <lmcons.h>
#include <sstream>    // para std::stringstream
#include <iomanip>    // para std::put_time
#include <chrono>     // para std::chrono::system_clock
#include <ctime>      // para std::time_t e std::tm

static inline ImVec2 Size = ImVec2(640, 440);

int CurrentTab = 2;

char Username[255] = "";
char Password[255] = "";
char Key[255] = "";
char Gmail[255] = "";

ImFont* InterBlack = nullptr;
ImFont* InterBold = nullptr;
ImFont* InterBold12 = nullptr;
ImFont* InterExtraBold = nullptr;
ImFont* InterExtraLight = nullptr;
ImFont* InterLight = nullptr;
ImFont* InterMedium = nullptr;
ImFont* InterRegular = nullptr;
ImFont* InterSemiBold = nullptr;
ImFont* InterThin = nullptr;

ImFont* FontAwesomeRegular = nullptr;
ImFont* FontAwesomeSolid = nullptr;
ImFont* FontAwesomeSolid14 = nullptr;
ImFont* FontAwesomeBrands = nullptr;

//Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

HWND hwnd = nullptr;

ID3D11ShaderResourceView* Logo = nullptr;

std::string GetUsername()
{
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len))
        return std::string(username);
    return "Unknown";
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace FWork {
    void Interface::Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext) {
        hWindow = Window;
        IDevice = Device;
        g_pd3dDevice = Device;
        g_pd3dDeviceContext = DeviceContext;

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hWindow);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

        InterBlack = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterBlack_compressed_data, InterBlack_compressed_size, 14);
        InterBold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 16);
        InterBold12 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 12);
        InterExtraBold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterExtraBold_compressed_data, InterExtraBold_compressed_size, 14);
        InterExtraLight = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterExtraLight_compressed_data, InterExtraLight_compressed_size, 14);
        InterLight = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterLight_compressed_data, InterLight_compressed_size, 16);
        InterMedium = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterMedium_compressed_data, InterMedium_compressed_size, 16);
        InterRegular = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterRegular_compressed_data, InterRegular_compressed_size, 16);
        InterSemiBold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterSemiBold_compressed_data, InterSemiBold_compressed_size, 16);
        InterThin = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(InterThin_compressed_data, InterThin_compressed_size, 14);

        ImFontConfig FontAwesomeConfig;

        static const ImWchar FontAwesomeRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        static const ImWchar FontAwesomeRangesBrands[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };


        FontAwesomeRegular = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Regular_compressed_data, FontAwesome6Regular_compressed_size, 17.f, &FontAwesomeConfig, FontAwesomeRanges);
        FontAwesomeSolid = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Solid_compressed_data, FontAwesome6Solid_compressed_size, 17.f, &FontAwesomeConfig, FontAwesomeRanges);
        FontAwesomeSolid14 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Solid_compressed_data, FontAwesome6Solid_compressed_size, 14.f, &FontAwesomeConfig, FontAwesomeRanges);

        FontAwesomeBrands = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(FontAwesome6Brands_compressed_data, FontAwesome6Brands_compressed_size, 17.f, &FontAwesomeConfig, FontAwesomeRangesBrands);

        Fonts::Initialize(IDevice);

        D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, LogoBytes, sizeof(LogoBytes), NULL, NULL, &Logo, NULL);

        InitializeMenu();
    }

    void Interface::InitializeMenu() {
        bIsMenuOpen = true;
        SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
        SetForegroundWindow(hWindow);
        SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    }

    void Interface::UpdateStyle() {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGuiStyle* Style = &ImGui::GetStyle();
        Style->AntiAliasedLines = true;
        Style->AntiAliasedLinesUseTex = true;
        Style->AntiAliasedFill = true;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        Style->WindowRounding = 7;
        Style->WindowBorderSize = 1;
        Style->WindowPadding = ImVec2(0, 0);
        Style->WindowShadowSize = 0;
        Style->ScrollbarSize = 8;

        Style->Colors[ImGuiCol_Separator] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_SeparatorActive] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_SeparatorHovered] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_ResizeGripActive] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(0, 0, 0, 0);

        Style->Colors[ImGuiCol_WindowBg] = ImColor(12, 12, 12);
        Style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);
        Style->Colors[ImGuiCol_Border] = ImColor(23, 24, 25);
        Style->Colors[ImGuiCol_Text] = ImColor(1.f, 1.f, 1.f, 0.8f);
        Style->Colors[ImGuiCol_TextSelectedBg] = ImColor(47, 108, 153, 100);
    }

    void Interface::RenderGui()
    {
        if (!bIsMenuOpen) return;

        static float AnimaTab = 0.f;
        static int LastCurrentTab = 0;

        static float animTop = -300.f;
        static float animBottom = 300.f;
        static int lastForms = -1;
        static bool firstOpen = true;
        static bool spinnerMoving = false;
        static bool spinnerMovedToBottom = false;
        static float Anima = 0.f;


        {
            if (LastCurrentTab > CurrentTab)
            {
                AnimaTab = -460.f;
                LastCurrentTab = CurrentTab;
            }
            else if (LastCurrentTab < CurrentTab)
            {
                AnimaTab = 460.f;
                LastCurrentTab = CurrentTab;
            }
            AnimaTab = ImLerp(AnimaTab, 0.f, 0.1f);

            ImGui::SetNextWindowSize(ImVec2(640, 440));
            ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            {
                ImDrawList* DrawList = ImGui::GetWindowDrawList();
                ImVec2 Pos = ImGui::GetWindowPos();
                ImVec2 Size = ImGui::GetWindowSize();

                // Tab
                static float AnimaTab = 0.0f;
                static int LastCurrentTab = 0;

                if (LastCurrentTab != CurrentTab)
                {
                    AnimaTab = (LastCurrentTab > CurrentTab) ? -460.f : 460.f;
                    LastCurrentTab = CurrentTab;
                }

                AnimaTab = ImLerp(AnimaTab, 0.f, 6.f * ImGui::GetIO().DeltaTime);

                // SubTab
                static int LastCurrentSub = 0;
                static int CurrentSub = 0;
                static float Anima = 0.f;

                if (LastCurrentSub != CurrentSub)
                {
                    Anima = (LastCurrentSub > CurrentSub) ? -460.f : 460.f;
                    LastCurrentSub = CurrentSub;
                }

                Anima = ImLerp(Anima, 0.f, 6.f * ImGui::GetIO().DeltaTime);

                if (CurrentTab == 0) {

                    ImVec2 childSize = ImVec2(Size.x, Size.y);
                    ImVec2 childPos = ImVec2((Size.x - childSize.x) * 0.5f, (Size.y - childSize.y) * 0.5f + AnimaTab);
                    ImGui::SetCursorPos(childPos);
                    ImGui::BeginChild("LoginChild", childSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                    {
                        ImVec2 childInnerSize = ImGui::GetWindowSize();
                        float centerX = (childInnerSize.x - 285) * 0.5f;
                        float centerY = (childInnerSize.y - 180) * 0.5f + 55;
                        ImVec2 childPosScreen = ImGui::GetCursorScreenPos();
                        ImVec2 imageSize = ImVec2(100, 100);
                        ImVec2 imagePos = ImVec2(childPosScreen.x + centerX + (285 - imageSize.x) * 0.5f, childPosScreen.y + centerY - 115);
                        DrawList->AddImage(Logo, imagePos, imagePos + imageSize, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, (int)(ImGui::GetStyle().Alpha * 255)));

                        static bool isLoginInProgress = false;
                        static std::string loginErrorMessage = "";
                        static bool showErrorMessage = false;
                        static bool apiInitialized = false;

                        if (CurrentSub == 0) {

                            ImGui::SetCursorPos({ centerX + Anima, centerY });
                            ImGui::InputTextEx("##Username", "Username", g_Globals.General.Username, IM_ARRAYSIZE(g_Globals.General.Username), ImVec2(285, 35), ImGuiInputTextFlags_None);

                            ImGui::SetCursorPos({ centerX + Anima, centerY + 40 });
                            ImGui::InputTextEx("##Password", "Password", g_Globals.General.Password, IM_ARRAYSIZE(g_Globals.General.Password), ImVec2(285, 35), ImGuiInputTextFlags_None);

                            ImGui::SetCursorPos({ centerX + Anima, centerY + 80 });
                            if (ImGui::Button("Login", ImVec2(285, 40))) {
                                
                            }
                        }
                        else if (CurrentSub == 1) {

                        }
                    }
                    ImGui::EndChild();

                }
                else {

                    DrawList->AddRectFilled(Pos, Pos + ImVec2(Size.x / 3.5f, Size.y), ImColor(16, 16, 16, (int)(ImGui::GetStyle().Alpha * 255)), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersLeft);
                    DrawList->AddLine(Pos + ImVec2(Size.x / 3.5f, 0), Pos + ImVec2(Size.x / 3.5f, Size.y), ImGui::GetColorU32(ImGuiCol_Border));

                    DrawList->AddShadowCircle(Pos + ImVec2(30, Size.y - 30), 18, ImColor(116, 116, 116, (int)(ImGui::GetStyle().Alpha * 255)), 15, ImVec2(0, 0));
                    DrawList->AddCircle(Pos + ImVec2(30, Size.y - 30), 18, ImGui::GetColorU32(ImGuiCol_Border), 360, 2);

                    ImGui::PushFont(InterMedium);
                    std::string username = GetUsername();
                    ImVec2 text_size = ImGui::CalcTextSize(username.c_str());
                    DrawList->AddText(Pos + ImVec2(56, Size.y - 30 - text_size.y), ImColor(255, 255, 255, (int)(ImGui::GetStyle().Alpha * 255)), username.c_str());
                    ImGui::PopFont();

                    ImGui::PushFont(InterRegular);
                    std::string expiry_text = "Expiry: Lifetime";
                    DrawList->AddText(Pos + ImVec2(56, Size.y - 30), ImColor(112, 112, 121, (int)(ImGui::GetStyle().Alpha * 255)), expiry_text.c_str());
                    ImGui::PopFont();

                    float scale = g_Globals.Visuals.LogoScale;

                    ImVec2 logoSize(100.0f * scale, 100.0f * scale);
                    ImVec2 sidebarSize(Size.x / 3.5f, Size.y);

                    ImVec2 topLeft(Pos.x + (sidebarSize.x - logoSize.x) * 0.5f, Pos.y + (60.0f - logoSize.y) * 0.5f + 10.0f);

                    ImVec2 bottomRight = topLeft + logoSize;

                    DrawList->AddImage(Logo,topLeft, bottomRight,ImVec2(0, 0), ImVec2(1, 1),ImColor(255, 255, 255, (int)(ImGui::GetStyle().Alpha * 255)));

                    DrawList->AddLine(Pos + ImVec2(0, Size.y - 60), Pos + ImVec2(Size.x / 3.5f, Size.y - 60), ImGui::GetColorU32(ImGuiCol_Border));

                    ImGui::BeginChild("LeftChild", ImVec2(Size.x / 3.5f, Size.y));
                    {
                        ImGui::SetCursorPos(ImVec2(20, Size.y / 7.0f + 10));
                        ImGui::BeginGroup();
                        {
                            ImGui::PushFont(InterBold);
                            ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 0.2f * ImGui::GetStyle().Alpha), "Combat");
                            ImGui::PopFont();

                            if (ImGui::Tab("Aimbot", ICON_FA_COMPUTER_MOUSE, CurrentTab == 2))
                                CurrentTab = 2;

                            ImGui::PushFont(InterBold);
                            ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 0.2f * ImGui::GetStyle().Alpha), "Visuals");
                            ImGui::PopFont();

                            if (ImGui::Tab("World", ICON_FA_GLOBE, CurrentTab == 3))
                                CurrentTab = 3;

                            ImGui::PushFont(InterBold);
                            ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 0.2f * ImGui::GetStyle().Alpha), "Others");
                            ImGui::PopFont();

                            if (ImGui::Tab("Misc", ICON_FA_LAYER_GROUP, CurrentTab == 4))
                                CurrentTab = 4;

                            if (ImGui::Tab("Configs", ICON_FA_FILE, CurrentTab == 5))
                                CurrentTab = 5;
                        }
                        ImGui::EndGroup();
                    }
                    ImGui::EndChild();

                    static float AnimaTab = 0.f;
                    static int LastCurrentTab = 0;

                    if (LastCurrentTab > CurrentTab)
                    {
                        AnimaTab = -460.f;
                        LastCurrentTab = CurrentTab;
                    }
                    else if (LastCurrentTab < CurrentTab)
                    {
                        AnimaTab = 460.f;
                        LastCurrentTab = CurrentTab;
                    }

                    AnimaTab = ImLerp(AnimaTab, 0.f, 0.1f);

                    ImGui::SetCursorPos(ImVec2(Size.x / 3.5f, AnimaTab));
                    ImGui::BeginChild("MainChild", ImVec2(Size.x - (Size.x / 3.5f), Size.y), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                    {
                        if (CurrentTab == 2) // Aimbot
                        {
                            static int LastCurrentSub = 0;
                            static int CurrentSub = 0;
                            ImGui::TabHeader("LegitBotHeader", &CurrentSub, { "Combat", "Others" }, CurrentTab);

                            static float Anima = 0.f;

                            if (LastCurrentSub > CurrentSub) // Trocou
                            {
                                Anima = -460.f;
                                LastCurrentSub = CurrentSub;
                            }
                            else if (LastCurrentSub < CurrentSub)
                            {
                                Anima = 460.f;
                                LastCurrentSub = CurrentSub;
                            }

                            Anima = ImLerp(Anima, 0.f, 0.1f);

                            ImGui::SetCursorPos(ImVec2(Anima, 85));
                            ImGui::BeginChild("Aimbot");
                            {
                                if (CurrentSub == 0)
                                {
                                    ImGui::SetCursorPos(ImVec2(20, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("General", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                        {
                                            ImGui::Checkbox("Aimbot Legit", &g_Globals.AimBot.Enabled);
                                            ImGui::CheckboxRisk("Aimbot Rage", &g_Globals.AimBot.AimShoulder);
                                            ImGui::KeyBind("KeyBind", &g_Globals.AimBot.AimbotBind, 0);
                                            ImGui::Checkbox("Ignore Knocked", &g_Globals.AimBot.IgnoreKnocked);
                                            ImGui::Checkbox("Ignore Bots", &g_Globals.AimBot.IgnoreBots);
                                            ImGui::Checkbox("Enable Fov", &g_Globals.Misc.ShowAimbotFov);
                                        }
                                        ImGui::EndCustomChild();

                                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                        ImGui::BeginGroup();
                                        {
                                            ImGui::CustomChild("Settings", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                            {
                                                ImGui::SliderInt("Field of View", &g_Globals.AimBot.DistanceAim, 0, 360, "%d");
                                                ImGui::SliderInt("Max Distance", &g_Globals.AimBot.Fov, 0, 200, "%dm");
                                                ImGui::SliderInt("Chest Rate ( ms )", &g_Globals.AimBot.UpdateInterval, 0, 500, "%dms");
                                                ImGui::ColorEdit4("Fov Filled Color", g_Globals.AimBot.Fillcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Fov Color", g_Globals.Misc.AimbotFovColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            }
                                            ImGui::EndCustomChild();
                                        }
                                        ImGui::EndGroup();
                                    }
                                    ImGui::EndGroup();
                                }
                                else if (CurrentSub == 1)
                                {
                                    ImGui::SetCursorPos(ImVec2(20, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("General", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 130));
                                        {
                                            ImGui::CheckboxRisk("Enemy Pull", &g_Globals.Exploits.EnemyPullEnabled);
                                            ImGui::KeyBind("KeyBind", &g_Globals.Exploits.EnemyPullBind, 0);
                                            ImGui::Checkbox("Ignore Knocked", &g_Globals.Silent.IgnoreKnocked);
                                        }
                                        ImGui::EndCustomChild();
                                        ImGui::CustomChild("Settings", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330 - 142.5f));
                                        {
                                            static const char* hitbox_items[] = { "Head", "Neck", "Chest" };
                                            static int current_hitbox = static_cast<int>(g_Globals.Silent.HitBox);
                                            if (ImGui::Combo("Hitbox", &current_hitbox, hitbox_items, IM_ARRAYSIZE(hitbox_items))) {
                                                g_Globals.Silent.HitBox = static_cast<Config::HitBox>(current_hitbox);
                                            }
                                            ImGui::SliderFloat("Pull Strength", &g_Globals.Exploits.PullHeightOffset, -2.0f, 2.0f, "%.2f");
                                            ImGui::SliderInt("Enemy Pull ( ms )", &g_Globals.Exploits.EnemyPullTickMs, 0, 10, "%dms");
                                            ImGui::SliderInt("Max Distance", &g_Globals.Exploits.EnemyPullMaxDistance, 10, 360, "%dm");
                                        }
                                        ImGui::EndCustomChild();
                                    }
                                    ImGui::EndGroup();
                                    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("Others", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                        {
                                            ImGui::Checkbox("Aim Lock", &g_Globals.AimBot.AimLock);
                                            ImGui::Checkbox("Aim Sight 2X", &g_Globals.AimBot.AimLock2x);
                                            ImGui::CheckboxRisk("Remove Recoil", &g_Globals.AimBot.NoRecoil);
                                            ImGui::CheckboxRisk("Remove Reload", &g_Globals.AimBot.FastReload);
                                            ImGui::CheckboxRisk("Infinite Ammo", &g_Globals.Exploits.InfiniteAmmo);
                                        }
                                        ImGui::EndCustomChild();
                                    }
                                    ImGui::EndGroup();
                                }
                            }
                            ImGui::EndChild();
                        }
                        else if (CurrentTab == 3) // World
                        {
                            static int LastCurrentSub = 0;
                            static int CurrentSub = 0;
                            ImGui::TabHeader("WorldHeader", &CurrentSub, { "World", "Colors" }, CurrentTab);

                            static float Anima = 0.f;

                            if (LastCurrentSub > CurrentSub) // Trocou
                            {
                                Anima = -460.f;
                                LastCurrentSub = CurrentSub;
                            }
                            else if (LastCurrentSub < CurrentSub)
                            {
                                Anima = 460.f;
                                LastCurrentSub = CurrentSub;
                            }

                            Anima = ImLerp(Anima, 0.f, 0.1f);

                            ImGui::SetCursorPos(ImVec2(Anima, 85));
                            ImGui::BeginChild("World");
                            {
                                if (CurrentSub == 0)
                                {
                                    ImGui::SetCursorPos(ImVec2(20, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("General", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                        {
                                            ImGui::Checkbox("Enable Esp", &g_Globals.Visuals.Enable);
                                            ImGui::Checkbox("Enable Observe Esp", &g_Globals.Visuals.Observe);
                                            ImGui::Checkbox("Enable Watermark", &g_Globals.Visuals.Watermark);
                                            ImGui::Checkbox("Enable Enemies", &g_Globals.Visuals.Enemy);
                                            ImGui::Checkbox("Esp Weapons Text", &g_Globals.Visuals.ESPWeapon);
                                            ImGui::Checkbox("Esp Weapons Icon", &g_Globals.Visuals.ESPWeaponIcon);
                                            ImGui::Checkbox("Esp Lines", &g_Globals.Visuals.Lines);
                                            ImGui::Checkbox("Enable Health Bar", &g_Globals.Visuals.HealthBar);
                                            ImGui::Checkbox("Health Text", &g_Globals.Visuals.ESPHealthTEXT);
                                            ImGui::Checkbox("Enable Box", &g_Globals.Visuals.Box);
                                            ImGui::Checkbox("Enable FilledBox", &g_Globals.Visuals.FilledBox);
                                            ImGui::Checkbox("Enable Name", &g_Globals.Visuals.Name);
                                            ImGui::Checkbox("Enable Distance", &g_Globals.Visuals.Distance);
                                            ImGui::Checkbox("Enable Skeleton", &g_Globals.Visuals.Skeleton);
                                        }
                                        ImGui::EndCustomChild();

                                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                        ImGui::BeginGroup();
                                        {
                                            ImGui::CustomChild("Settings", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                            {
                                                ImGui::Combo("Lines Position", &g_Globals.Visuals.EspLines, "None\0Top\0Bottom\0");
                                                ImGui::Combo("Health Position", &g_Globals.Visuals.players_healthbar, "None\0Left\0Right\0Top\0Bottom");
                                                ImGui::Combo("Box Style", &g_Globals.Visuals.players_box, "None\0Full\0Cornered\0");
                                                ImGui::SliderFloat("Text Size", &g_Globals.Visuals.TextSize, 0.0f, 20.0f, "%.1f");
                                                ImGui::SliderFloat("Thickness", &g_Globals.Visuals.Thickness, 0.1f, 3.0f, "%.1f");
                                                ImGui::SliderInt("Render Distance", &g_Globals.Visuals.DistanceEsp, 0, 250, "%dms");

                                            }
                                            ImGui::EndCustomChild();
                                        }
                                        ImGui::EndGroup();
                                    }
                                    ImGui::EndGroup();
                                }
                                else if (CurrentSub == 1)
                                {
                                    ImGui::SetCursorPos(ImVec2(20, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("Primary", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                        {
                                            ImGui::ColorEdit4("Name Color", g_Globals.Visuals.NameColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            ImGui::ColorEdit4("Distance Color", g_Globals.Visuals.DistColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            ImGui::ColorEdit4("Skeleleton Color", g_Globals.Visuals.SkeletonColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            ImGui::ColorEdit4("Box Color", g_Globals.Visuals.BoxColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            ImGui::ColorEdit4("Lines Color", g_Globals.Visuals.LinesColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            ImGui::ColorEdit4("Health Color", g_Globals.Visuals.texthColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                        }
                                        ImGui::EndCustomChild();

                                        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                        ImGui::BeginGroup();
                                        {
                                            ImGui::CustomChild("Secondary", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                            {
                                                ImGui::ColorEdit4("Enemies Detected Color", g_Globals.Visuals.EnemyColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Watermark Color", g_Globals.Visuals.WatermarkColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Enemies Knocked Color", g_Globals.Visuals.WatermarkColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Weapons Text Color", g_Globals.Visuals.ESPWeaponColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Filled Box Color", g_Globals.Visuals.Filledboxcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                                ImGui::ColorEdit4("Target Color", g_Globals.Visuals.AlvoColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
                                            }
                                            ImGui::EndCustomChild();
                                        }
                                        ImGui::EndGroup();
                                    }
                                    ImGui::EndGroup();
                                }
                            }
                            ImGui::EndChild();
                        }
                        else if (CurrentTab == 4) // Misc
                        {
                            static int LastCurrentSub = 0;
                            static int CurrentSub = 0;

                            static float Anima = 0.f;

                            if (LastCurrentSub > CurrentSub) // Trocou
                            {
                                Anima = -460.f;
                                LastCurrentSub = CurrentSub;
                            }
                            else if (LastCurrentSub < CurrentSub)
                            {
                                Anima = 460.f;
                                LastCurrentSub = CurrentSub;
                            }

                            Anima = ImLerp(Anima, 0.f, 0.1f);

                            ImGui::SetCursorPos(ImVec2(20, 35));
                            ImGui::SearchBar("SearchBar", ImVec2(250, 35));
                            ImGui::SetCursorPos(ImVec2(Anima, 85));
                            ImGui::BeginChild("Misc");
                            {
                                ImGui::SetCursorPos(ImVec2(20, 0));
                                ImGui::BeginGroup();
                                {
                                    ImGui::CustomChild("Settings", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                    {
                                        ImGui::Checkbox("Stream Mode", &g_Globals.General.Capture);
                                        static const char* priority_items[] = { "High", "Normal", "Low" };
                                        static int current_priority = static_cast<int>(g_Globals.Silent.HitBox);
                                        if (ImGui::Combo("Process Priority", &current_priority, priority_items, IM_ARRAYSIZE(priority_items))) {
                                            g_Globals.General.Priority = static_cast<Config::Priority>(current_priority);
                                            HANDLE hProcess = GetCurrentProcess();
                                            DWORD priorityClass = NORMAL_PRIORITY_CLASS;
                                            switch (current_priority) {
                                            case 0: // High
                                                priorityClass = HIGH_PRIORITY_CLASS;
                                                break;
                                            case 1: // Normal
                                                priorityClass = NORMAL_PRIORITY_CLASS;
                                                break;
                                            case 2: // Low
                                                priorityClass = IDLE_PRIORITY_CLASS;
                                                break;
                                            }
                                            if (!SetPriorityClass(hProcess, priorityClass)) {
                                                std::cerr << "Erro ao mudar prioridade! " << GetLastError() << std::endl;
                                            }
                                        }
                                        ImGui::SliderInt("Delay", &g_Globals.General.Delay, 0, 100, "%dms");
                                        ImGui::SliderFloat("Logo Scale", &g_Globals.Visuals.LogoScale, 0.10f, 1.00f, "%.2f");
                                        ImGui::KeyBind("Keybind Menu", &g_Globals.General.MenuKey, 0);
                                    }
                                    ImGui::EndCustomChild();

                                    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("Bypass Screen", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                        {
                                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.184f, 0.424f, 0.6f, 1.0f));
                                            ImGui::Text("Learn Please:");
                                            ImGui::PopStyleColor();
                                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f));
                                            ImGui::Text("By clicking on the button you\nunload the panel and\nall the functions\nbut if you want\nyou can inject them again!");
                                            ImGui::PopStyleColor();
                                            if (ImGui::Button("Unload Menu"))
                                            {
                                                g_Globals.General.ShutDown = true;
                                            }
                                        }
                                        ImGui::EndCustomChild();
                                    }
                                    ImGui::EndGroup();
                                }
                                ImGui::EndGroup();
                            }
                        }
                        else if (CurrentTab == 5) // Configs
                        {
                            static int LastCurrentSub = 0;
                            static int CurrentSub = 0;

                            static float Anima = 0.f;

                            if (LastCurrentSub > CurrentSub) // Trocou
                            {
                                Anima = -460.f;
                                LastCurrentSub = CurrentSub;
                            }
                            else if (LastCurrentSub < CurrentSub)
                            {
                                Anima = 460.f;
                                LastCurrentSub = CurrentSub;
                            }

                            Anima = ImLerp(Anima, 0.f, 0.1f);

                            ImGui::SetCursorPos(ImVec2(20, 35));
                            ImGui::SearchBar("SearchBar", ImVec2(250, 35));
                            ImGui::SetCursorPos(ImVec2(Anima, 85));
                            ImGui::BeginChild("Configs");
                            {
                                ImGui::SetCursorPos(ImVec2(20, 0));
                                ImGui::BeginGroup();
                                {
                                    ImGui::CustomChild("Cloud Config", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 330));
                                    {
                                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f));

                                        const char* text = "In Maintenance...";
                                        ImVec2 text_size = ImGui::CalcTextSize(text);

                                        float child_width = ImGui::GetContentRegionAvail().x;
                                        float child_height = ImGui::GetContentRegionAvail().y;

                                        float text_x = (child_width - text_size.x) * 0.5f;
                                        float text_y = (child_height - text_size.y) * 0.5f;

                                        ImGui::SetCursorPos(ImVec2(text_x, text_y));

                                        ImGui::Text("%s", text);

                                        ImGui::PopStyleColor();
                                    }
                                    ImGui::EndCustomChild();

                                    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25 + 35, 0));
                                    ImGui::BeginGroup();
                                    {
                                        ImGui::CustomChild("User Info", ImVec2(ImGui::GetWindowSize().x / 2 - 25, 130));
                                        {
                                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.184f, 0.424f, 0.6f, 1.0f));
                                            ImGui::Text("User Information:");
                                            ImGui::PopStyleColor();
                                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f));
                                            std::string username = GetUsername();
                                            ImGui::Text("Username: %s", username.c_str());
                                            //std::string expiry_text = GetExpiryString();
                                            ImGui::Text("Expiry: Lifetime");
                                            ImGui::Text("Dev: nash (85hz)");
                                            ImGui::PopStyleColor();
                                        }
                                        ImGui::EndCustomChild();
                                    }
                                    ImGui::EndGroup();
                                }
                                ImGui::EndGroup();
                            }
                        }
                    }

                    ImGui::EndChild();
                    DrawList->AddRect(Pos, Pos + Size, ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().WindowRounding);
                }
                ImGui::End();
            }
        }
    }

    void Interface::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                ResizeWidht = (UINT)LOWORD(lParam);
                ResizeHeight = (UINT)HIWORD(lParam);
            }
            break;
        }

        if (bIsMenuOpen) {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        }
    }

    void Interface::HandleMenuKey()
    {
        static bool MenuKeyDown = false;
        if (GetAsyncKeyState(g_Globals.General.MenuKey) & 0x8000)
        {
            if (!MenuKeyDown)
            {
                MenuKeyDown = true;
                bIsMenuOpen = !bIsMenuOpen;

                if (bIsMenuOpen) {
                    SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
                    SetForegroundWindow(hWindow);
                }
                else {
                    SetWindowLong(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE);
                    SetForegroundWindow(hTargetWindow);
                }
                SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
            }
        }
        else {
            MenuKeyDown = false;
        }
    }

    void Interface::ShutDown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        Overlay::ShutDown();
    }
}
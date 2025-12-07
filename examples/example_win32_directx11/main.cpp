#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include <d3d11.h>
#include <tchar.h>

#include "background_pic.h"
#include "blur.hpp"

#include "segue_font.h"
#include "ico_font.h"

float color_edit4[4] = { 1.00f, 1.00f, 1.00f, 1.000f };

float accent_color[4] = { 0.745f, 0.151f, 0.151f, 1.000f };

static int select_count = 0;

bool active = false;

float size_child = 0;

bool menu = false;
ImFont* ico = nullptr;
ImFont* ico_combo = nullptr;
ImFont* ico_button = nullptr;
ImFont* ico_grande = nullptr;
ImFont* segu = nullptr;
ImFont* default_segu = nullptr;
ImFont* bold_segu = nullptr;

// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int tab_count = 0, tabs = 0;

int main(int, char**)
{

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 0, 0, 1920, 1060, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiContext& g = *GImGui;
    io.Fonts->AddFontFromMemoryTTF(&seguoe, sizeof seguoe, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());

    default_segu = io.Fonts->AddFontFromMemoryTTF(&seguoe, sizeof seguoe, 22, NULL, io.Fonts->GetGlyphRangesCyrillic());

    segu = io.Fonts->AddFontFromMemoryTTF(&seguoe, sizeof seguoe, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    bold_segu = io.Fonts->AddFontFromMemoryTTF(&bold_segue, sizeof bold_segue, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 24, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico_combo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 19, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico_button = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ico_grande = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        DWORD window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::SetNextWindowPos(ImVec2(200, 200));

        ImGui::NewFrame();
        {

            ImGui::SetNextWindowSize(ImVec2(905, 624));

            ImGui::Begin("Hola!", &menu, window_flags);
            {
                ImVec2 P1, P2;
                ImDrawList* pDrawList;
                const auto& p = ImGui::GetWindowPos();
                const auto& pWindowDrawList = ImGui::GetWindowDrawList();
                const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
                const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();

                DrawBackgroundBlur(pWindowDrawList, g_pd3dDevice);


                if (scene == nullptr) D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &background, sizeof(background), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &scene);

                ImGui::GetBackgroundDrawList()->AddImage(scene, ImVec2(0, 0), ImVec2(1920, 1080), ImVec2(0, 0), ImVec2(1, 1), ImColor(color_edit4[0], color_edit4[1], color_edit4[2], color_edit4[3]));

                pBackgroundDrawList->AddRectFilled(ImVec2(0.000f + p.x, 0.000f + p.y), ImVec2(905 + p.x, 624 + p.y), ImColor(9, 9, 9, 180), 10); // Background

                pWindowDrawList->AddRectFilled(ImVec2(189.000f + p.x, 75.000f + p.y), ImVec2(903 + p.x, 76 + p.y), ImColor(25, 25, 25, 180), 10); // bar line

                ImGui::SetCursorPos(ImVec2(800, 21));

                if (ImGui::OptButton("L", ImVec2(30, 30), false));

                ImGui::SameLine(840);

                if(ImGui::OptButton("B", ImVec2(30, 30), true));

                pWindowDrawList->AddRectFilled(ImVec2(0.000f + p.x, 0.000f + p.y), ImVec2(190 + p.x, 624 + p.y), ImGui::GetColorU32(ImGuiCol_ChildBg), 10, ImDrawFlags_RoundCornersLeft); // bar line

                    const int vtx_idx_1 = pWindowDrawList->VtxBuffer.Size;

                    pWindowDrawList->AddText(ico_grande, 40.f, ImVec2(20.000f + p.x, 20.000f + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.70f), "U");

                    pWindowDrawList->AddText(bold_segu, 40.f, ImVec2(70.000f + p.x, 15.000f + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.70f), "Evicted");

                    pWindowDrawList->AddRectFilled(ImVec2(70.000f + p.x, 51.000f + p.y), ImVec2(163 + p.x, 52 + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.70f), 10); // bar line

                    const int vtx_idx_2 = pWindowDrawList->VtxBuffer.Size;

                    ImGui::ShadeVertsLinearColorGradientKeepAlpha(pWindowDrawList, vtx_idx_1, vtx_idx_2, ImVec2(0 + p.x, 0 + p.y), ImVec2(200 + p.x, 20 + p.y), ImColor(0.25f, 0.25f, 0.25f, 0.50f), ImColor(0.60f, 0.60f, 0.60f, 1.00f));

                    ImGui::SetCursorPosY(80);

                    if (ImGui::TabButton("P", "LegitBot", ImVec2(190, 40)) && tab_count != 0) {
                        tab_count = 0;
                        active = true;
                    }
                    if (ImGui::TabButton("N", "RageBot", ImVec2(190, 40)) && tab_count != 1) {
                        tab_count = 1;
                        active = true;
                    }

                    if (ImGui::TabButton("Q", "AntiAim", ImVec2(190, 40)) && tab_count != 2) {
                        tab_count = 2;
                        active = true;
                    }

                    if (ImGui::TabButton("I", "Visuals", ImVec2(190, 40)) && tab_count != 3) {
                        tab_count = 3;
                        active = true;
                    }

                    if (ImGui::TabButton("O", "Misc", ImVec2(190, 40)) && tab_count != 4) {
                        tab_count = 4;
                        active = true;
                    }

                    if (ImGui::TabButton("R", "PlayerList", ImVec2(190, 40)) && tab_count != 5) {
                        tab_count = 5;
                        active = true;
                    }

                    if (ImGui::TabButton("T", "Skins", ImVec2(190, 40)) && tab_count != 6) {
                        tab_count = 6;
                        active = true;
                    }

                    if (ImGui::TabButton("J", "Lua", ImVec2(190, 40)) && tab_count != 7) {
                        tab_count = 7;
                        active = true;
                    }

                    if (ImGui::TabButton("S", "Config", ImVec2(190, 40)) && tab_count != 8) {
                        tab_count = 8;
                        active = true;
                    }

                if (active) {
                    if (size_child <= 10) size_child += 1 / ImGui::GetIO().Framerate * 60.f;
                    else { active = false; tabs = tab_count; };
                }
                else {
                    if (size_child >= 0) size_child -= 1 / ImGui::GetIO().Framerate * 60.f;
                }

                pWindowDrawList->AddCircleFilled(ImVec2(57.000f + p.x, 570.000f + p.y), 25.000f, ImColor(10, 9, 10, 255), 30);

                pWindowDrawList->AddCircle(ImVec2(57.000f + p.x, 570.000f + p.y), 27.000f, ImColor(20, 19, 20, 255), 30, 4.000f);

                pWindowDrawList->AddText(segu, 40.f, ImVec2(51.000f + p.x, 548.000f + p.y), ImColor(0.60f, 0.60f, 0.60f, 0.50f), "?");



                const int vtx_idx_3 = pWindowDrawList->VtxBuffer.Size;

                pWindowDrawList->AddText(segu, 22.f, ImVec2(97.000f + p.x, 547.000f + p.y), ImColor(0.40f, 0.40f, 0.40f, 0.50f), "Benjy\nLifetime");

                const int vtx_idx_4 = pWindowDrawList->VtxBuffer.Size;

                ImGui::ShadeVertsLinearColorGradientKeepAlpha(pWindowDrawList, vtx_idx_3, vtx_idx_4, ImVec2(97.000f + p.x, 547.000f + p.y), ImVec2(200.000f + p.x, 567.000f + p.y), ImColor(0.35f, 0.35f, 0.35f, 0.50f), ImColor(0.90f, 0.90f, 0.90f, 1.00f));



                ImGui::SetCursorPos(ImVec2(203, 88 - size_child));

                switch (tabs) {

                case 0:

                    ImGui::BeginChild("Editor", ImVec2(339, 253), true); {

                        static bool test_0 = false;
                        ImGui::Checkbox("Checkbox_0", &test_0);

                        static bool test_1 = false;
                        ImGui::Checkbox("Checkbox_1", &test_1);

                        static int slider_test_0 = 50;
                        ImGui::SliderInt("Slider Intager", &slider_test_0, 1, 400);

                        static float slider_test_1 = 0;
                        ImGui::SliderFloat("Slider Float", &slider_test_1, 0.00f, 5.00f, "%.2f");

                    }ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(555, 88 - size_child));

                    ImGui::BeginChild("Background", ImVec2(339, 210), true); {

                        ImGui::ColorEdit4("Background Color", (float*)&color_edit4, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);

                        ImGui::ColorEdit4("Widget Color", (float*)&accent_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);

                    }ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(203, 353 - size_child));

                    ImGui::BeginChild("Aimbot", ImVec2(339, 258), true); {

                        static int selectedItem = 0;
                        static const char* items[]{ "Always", "Toggle" };
                        ImGui::Combo("Aimbot Mode", &selectedItem, items, IM_ARRAYSIZE(items), 5);

                        static char buf[64] = { "" };
                        ImGui::InputText("InputText", buf, 64);

                    }ImGui::EndChild();


                    ImGui::SetCursorPos(ImVec2(555, 313 - size_child));

                    ImGui::BeginChild("Keybinds", ImVec2(339, 298), true); {

                        static int k, m = 1;
                        ImGui::Keybind("Aimbot Keybind", &k, &m);

                    }ImGui::EndChild();

                    break;

                case 8:

                    ImGui::BeginChild("Configuration", ImVec2(339, 253), true); {


                    }ImGui::EndChild();

                    break;

                }
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

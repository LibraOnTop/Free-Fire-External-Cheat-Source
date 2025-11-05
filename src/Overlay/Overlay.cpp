#include "Overlay.hpp"
#include <string>
#include <dwmapi.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <iostream>
#include <algorithm>
#include <src/Globals.hpp>

std::wstring RandomString(size_t Length) {
	auto Randchar = []() -> char {
		const char* Charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		const size_t MaxIndex = (sizeof(Charset) - 1);
		return Charset[rand() % MaxIndex];
		};

	std::wstring Str(Length, 0);
	std::generate_n(Str.begin(), Length, Randchar);
	return Str;
}

struct WndRECT : public RECT {
	int Width() { return right - left; }
	int Height() { return bottom - top; }
};

static inline std::function<void(HWND, UINT, WPARAM, LPARAM)> pWindowProc;

bool bSettuped = false;
bool bInitialized = false;
bool bDeviceInitialized = false;
bool bRenderTargetInitialized = false;

HWND hWindow = nullptr;
WNDCLASSEX WindowClass;
HWND hTargetWindow = nullptr;
WndRECT wTargetWindowRect;

ID3D11Device* ID3dDevice;
ID3D11DeviceContext* ID3dDeviceContext;
IDXGISwapChain* ID3dSwapChain;
ID3D11RenderTargetView* ID3dRenderTargetView;

void CreateDeviceD3D() {

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
	SwapChainDesc.BufferDesc.Width = 0;
	SwapChainDesc.BufferDesc.Height = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.OutputWindow = hWindow;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL FeatureLevel;
	const D3D_FEATURE_LEVEL FeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, FeatureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &ID3dSwapChain, &ID3dDevice, &FeatureLevel, &ID3dDeviceContext);
	if (FAILED(hr)) {
#ifdef _DEBUG
		std::cerr << "FWork::Window::InitializeDirectX11::D3D11CreateDeviceAndSwapChain Error: " << hr << std::endl;
#endif // _DEBUG
		return;
	}

	bDeviceInitialized = true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (pWindowProc)
		pWindowProc(hWnd, uMsg, wParam, lParam);

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace FWork {
	void Overlay::Setup(HWND TargetHWND) {
		if (!TargetHWND) {
#ifdef _DEBUG
			std::cerr << "FWork::Window::Setup - Invalid target window handle" << std::endl;
#endif // _DEBUG
			return;
		}

		hTargetWindow = TargetHWND;
		if (GetClientRect(hTargetWindow, &wTargetWindowRect)) {
			MapWindowPoints(hTargetWindow, nullptr, reinterpret_cast<LPPOINT>(&wTargetWindowRect), 2);
			bSettuped = true;
		}
		else {
#ifdef _DEBUG
			std::cerr << "FWork::Window::Setup - Failed to get target window rect" << std::endl;
#endif // _DEBUG
		}
	}

	void Overlay::Initialize() {
		if (!bSettuped) {
#ifdef _DEBUG
			std::cerr << "FWork::Window::Initialize - Overlay not setup" << std::endl;
#endif // _DEBUG
			return;
		}
#ifdef _DEBUG
		std::cout << "Attempting to unregister existing window class..." << std::endl;
#endif // _DEBUG
		WindowClass.lpszClassName = RandomString(10).c_str();

		if (WindowClass.lpszClassName != nullptr) {
			UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
#ifdef _DEBUG
			std::cout << "Successfully unregistered existing window class." << std::endl;
#endif // _DEBUG
		}

		WindowClass.cbSize = sizeof(WindowClass);
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		WindowClass.lpfnWndProc = WindowProc;
		WindowClass.cbClsExtra = 0;
		WindowClass.cbWndExtra = 0;
		WindowClass.hInstance = GetModuleHandle(NULL);
		WindowClass.hIcon = NULL;
		WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));
		WindowClass.lpszMenuName = NULL;
		WindowClass.hIconSm = NULL;

		if (!RegisterClassEx(&WindowClass)) {
#ifdef _DEBUG
			std::cerr << "FWork::Window::Initialize::RegisterClassEx Error: " << GetLastError() << std::endl;
#endif // _DEBUG
			return;
		}

		hWindow = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED,
			WindowClass.lpszClassName,
			WindowClass.lpszMenuName,
			WS_POPUP | WS_VISIBLE,
			wTargetWindowRect.left,
			wTargetWindowRect.top,
			wTargetWindowRect.Width(),
			wTargetWindowRect.Height(),
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			nullptr
		);

		if (!hWindow) {
#ifdef _DEBUG
			std::cerr << "FWork::Window::Initialize::CreateWindowEx Error: " << GetLastError() << std::endl;
#endif // _DEBUG
			return;
		}

		SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_ALPHA);

		MARGINS Margins = { wTargetWindowRect.left, wTargetWindowRect.top, wTargetWindowRect.Width(), wTargetWindowRect.Height() };
		DwmExtendFrameIntoClientArea(hWindow, &Margins);

		ShowWindow(hWindow, SW_SHOW);
		UpdateWindow(hWindow);

		bInitialized = true;

		dxInitialize();
	}

	void Overlay::ShutDown() {
		dxShutDown();

		if (hWindow) {
			DestroyWindow(hWindow);
			hWindow = nullptr;
		}

		if (WindowClass.lpszClassName) {
			UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
			WindowClass.lpszClassName = nullptr;
		}

		bInitialized = false;
		bSettuped = false;
	}

	void Overlay::UpdateWindowPos() {
		WndRECT TargetWindowRect;
		GetClientRect(hTargetWindow, &TargetWindowRect);
		MapWindowPoints(hTargetWindow, nullptr, reinterpret_cast<LPPOINT>(&TargetWindowRect), 2);
		MoveWindow(hWindow, TargetWindowRect.left, TargetWindowRect.top, TargetWindowRect.Width(), TargetWindowRect.Height(), false);

		g_Globals.EspConfig.Width = TargetWindowRect.Width();
		g_Globals.EspConfig.Height = TargetWindowRect.Height();
	}

	void Overlay::SetupWindowProcHook(std::function<void(HWND, UINT, WPARAM, LPARAM)> Funtion) {
		pWindowProc = Funtion;
	}

	void Overlay::dxInitialize() {
		CreateDeviceD3D();
		if (bDeviceInitialized) {
			dxCreateRenderTarget();
		}
	}

	void Overlay::dxRefresh() {
		ID3dDeviceContext->OMSetRenderTargets(1, &ID3dRenderTargetView, nullptr);
		static float TransparentColor[4] = { 0, 0, 0, 0 };
		ID3dDeviceContext->ClearRenderTargetView(ID3dRenderTargetView, TransparentColor);
	}

	void Overlay::dxShutDown() {
		dxCleanupRenderTarget();
		dxCleanupDeviceD3D();
	}

	void Overlay::dxCreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		ID3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		ID3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &ID3dRenderTargetView);
		pBackBuffer->Release();

		bRenderTargetInitialized = true;
	}

	void Overlay::dxCleanupRenderTarget()
	{
		if (ID3dRenderTargetView) {
			ID3dRenderTargetView->Release();
			ID3dRenderTargetView = nullptr;
		}
		bRenderTargetInitialized = false;
	}

	void Overlay::dxCleanupDeviceD3D()
	{
		if (ID3dSwapChain) {
			ID3dSwapChain->Release();
			ID3dSwapChain = nullptr;
		}

		if (ID3dDeviceContext) {
			ID3dDeviceContext->Release();
			ID3dDeviceContext = nullptr;
		}

		if (ID3dDevice) {
			ID3dDevice->Release();
			ID3dDevice = nullptr;
		}

		bDeviceInitialized = false;
	}

	bool Overlay::IsSettuped() { return bSettuped; }
	bool Overlay::IsInitialized() { return bInitialized; }
	HWND Overlay::GetOverlayWindow() { return hWindow; }
	HWND Overlay::GetTargetWindow() { return hTargetWindow; }

	ID3D11Device* Overlay::dxGetDevice() { return ID3dDevice; }
	ID3D11DeviceContext* Overlay::dxGetDeviceContext() { return ID3dDeviceContext; }
	IDXGISwapChain* Overlay::dxGetSwapChain() { return ID3dSwapChain; }
	ID3D11RenderTargetView* Overlay::dxGetRenderTarget() { return ID3dRenderTargetView; }
}
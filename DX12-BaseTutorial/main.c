#define INITGUID
#include <Windows.h>
#include <d3d12.h>
#define RETURN_IF_ZERO(v) if (v == 0) return __LINE__
BOOL gQuit = FALSE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DESTROY)
	{
		gQuit = TRUE;
	}
	if (message == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		gQuit = TRUE;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	{
		WNDCLASS wndClass;
		wndClass.style = 0;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = NULL;
		wndClass.hCursor = NULL;
		wndClass.hbrBackground = NULL;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"DX12BaseTutorialClass";
		ATOM atom = RegisterClass(&wndClass);
		RETURN_IF_ZERO(atom);
	}

	HWND hWnd = CreateWindow(L"DX12BaseTutorialClass", L"DX12", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		100, 0, 300, 200, NULL, NULL, hInstance, NULL);

	ID3D12Debug* debug = NULL;
	D3D12GetDebugInterface(&IID_ID3D12Debug, &debug);
	RETURN_IF_ZERO(debug);
	debug->lpVtbl->EnableDebugLayer(debug);

	ID3D12Device* device = NULL;
	D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, &device);
	RETURN_IF_ZERO(device);

	ID3D12CommandAllocator* commandAllocator = NULL;
	ID3D12GraphicsCommandList* commandList = NULL;
	ID3D12CommandQueue* commandQueue = NULL;

	device->lpVtbl->CreateCommandAllocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		&IID_ID3D12CommandAllocator, &commandAllocator);
	device->lpVtbl->CreateCommandList;
	device->lpVtbl->CreateCommandQueue;

	while (gQuit == FALSE)
	{
		MSG msg;
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
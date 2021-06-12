#define INITGUID
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#define RETURN_IF_ZERO(v) if (v == 0) return __LINE__
#define COM_RELEASE(v) v->lpVtbl->Release(v)
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

	// 윈도우 생성
	HWND hWnd = CreateWindow(L"DX12BaseTutorialClass", L"DX12", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		100, 0, 300, 200, NULL, NULL, hInstance, NULL);

	// 디버그 레이어 생성
	ID3D12Debug* debug = NULL;
	D3D12GetDebugInterface(&IID_ID3D12Debug, &debug);
	RETURN_IF_ZERO(debug);
	debug->lpVtbl->EnableDebugLayer(debug);

	// 디바이스 생성
	ID3D12Device* device = NULL;
	D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, &device);
	RETURN_IF_ZERO(device);

	// 커맨드 3형제 생성
	ID3D12CommandAllocator* commandAllocator = NULL;
	ID3D12GraphicsCommandList* commandList = NULL;
	ID3D12CommandQueue* commandQueue = NULL;

	device->lpVtbl->CreateCommandAllocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		&IID_ID3D12CommandAllocator, &commandAllocator);
	RETURN_IF_ZERO(commandAllocator);

	device->lpVtbl->CreateCommandList(device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		commandAllocator, NULL, &IID_ID3D12GraphicsCommandList, &commandList);
	RETURN_IF_ZERO(commandList);

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Priority = 0;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.NodeMask = 0;

	device->lpVtbl->CreateCommandQueue(device, &commandQueueDesc, &IID_ID3D12CommandQueue, &commandQueue);
	RETURN_IF_ZERO(commandQueue);

	// 팩토리 생성
	IDXGIFactory2* dxgiFactory = NULL;
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory2, &dxgiFactory);

	// 프로그램 루프
	while (gQuit == FALSE)
	{
		MSG msg;
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 메모리 해제
	COM_RELEASE(dxgiFactory);
	COM_RELEASE(commandQueue);
	COM_RELEASE(commandList);
	COM_RELEASE(commandAllocator);
	COM_RELEASE(device);
	COM_RELEASE(debug);

	return 0;
}
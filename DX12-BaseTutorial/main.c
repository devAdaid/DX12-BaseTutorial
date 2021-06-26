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
	commandList->lpVtbl->Close(commandList);

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

	// 스왑 체인 생성
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = 300;
	swapChainDesc.BufferDesc.Height = 200;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;

	IDXGISwapChain* swapChain = NULL;
	dxgiFactory->lpVtbl->CreateSwapChain(dxgiFactory, (IUnknown*)commandQueue, &swapChainDesc, &swapChain);

	// 서술자 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapDesc.NumDescriptors = 2;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descHeapDesc.NodeMask = 0;

	ID3D12DescriptorHeap* descHeap = NULL;
	device->lpVtbl->CreateDescriptorHeap(device, &descHeapDesc, &IID_ID3D12DescriptorHeap, &descHeap);
	RETURN_IF_ZERO(descHeap);


	UINT rtvHeapSize = device->lpVtbl->GetDescriptorHandleIncrementSize(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	ID3D12Resource* bufferList[2] = { 0 };
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		// CPU 서술자 핸들 가져옴: 버그 있어서 올바른 시그니쳐로 함수 형변환 필요
		((void(__stdcall *)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			descHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(descHeap, &rtvHandle);

		// 스왑 버퍼 생성
		for (int i = 0; i < 2; i++)
		{
			swapChain->lpVtbl->GetBuffer(swapChain, i, &IID_ID3D12Resource, &bufferList[i]);
			RETURN_IF_ZERO(bufferList[i]);

			// 이 상황에서는 Desc 값이 이미 있어서 NULL로 해줘도 된다
			device->lpVtbl->CreateRenderTargetView(device, bufferList[i], NULL, rtvHandle);
			rtvHandle.ptr += rtvHeapSize;
		}
	}

	// 펜스 생성
	UINT64 fenceValue = 0;
	ID3D12Fence* fence = NULL;
	device->lpVtbl->CreateFence(device, fenceValue, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &fence);
	RETURN_IF_ZERO(fence);

	// 백버퍼 인덱스
	UINT backBufferIndex = 0;

	// 프로그램 루프
	while (gQuit == FALSE)
	{
		MSG msg;
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// 초기화
		commandAllocator->lpVtbl->Reset(commandAllocator);
		commandList->lpVtbl->Reset(commandList, commandAllocator, NULL);

		// 커맨드 삽입
		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = bufferList[backBufferIndex];
			barrier.Transition.Subresource = 0;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			commandList->lpVtbl->ResourceBarrier(commandList, 1, &barrier); 
		}

		{
			D3D12_RECT scissorRect;
			scissorRect.left = 0;
			scissorRect.top = 0;
			scissorRect.right = 300;
			scissorRect.bottom = 200;
			commandList->lpVtbl->RSSetScissorRects(commandList, 1, &scissorRect);

			D3D12_VIEWPORT viewport;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = 300;
			viewport.Height = 200;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;
			commandList->lpVtbl->RSSetViewports(commandList, 1, &viewport);

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
			((void(__stdcall *)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
				descHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(descHeap, &rtvHandle);
			rtvHandle.ptr += backBufferIndex * rtvHeapSize;

			float clearColor[] = { 1,0,0,1 };
			D3D12_RECT clearRect;
			clearRect.left = 0;
			clearRect.top = 0;
			clearRect.right = 300;
			clearRect.bottom = 200;
			commandList->lpVtbl->ClearRenderTargetView(commandList, rtvHandle, clearColor, 1, &clearRect);
		}

		{
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = bufferList[backBufferIndex];
			barrier.Transition.Subresource = 0;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			commandList->lpVtbl->ResourceBarrier(commandList, 1, &barrier);
		}
		// 커맨드 삽입 끝
		
		// 커맨드 실행
		commandList->lpVtbl->Close(commandList);
		ID3D12CommandList* commandListList[] = { (ID3D12CommandList*)commandList };
		commandQueue->lpVtbl->ExecuteCommandLists(commandQueue, 1, commandListList);

		// 프론트 버퍼 표시
		swapChain->lpVtbl->Present(swapChain, 0, 0);
		backBufferIndex = 1 - backBufferIndex;

		// 펜스 값 증가 및 Signal
		// 비동기 안전하게 하기 위해서는 InterlockedIncrement64 사용
		fenceValue += 1;
		commandQueue->lpVtbl->Signal(commandQueue, fence, fenceValue);

		// 기다려야 할 경우 이벤트가 발생할 때까지 대기
		if (fence->lpVtbl->GetCompletedValue(fence) < fenceValue)
		{
			// win32 Event
			HANDLE e = CreateEvent(NULL, FALSE, FALSE, NULL);
			fence->lpVtbl->SetEventOnCompletion(fence, fenceValue, e);
			WaitForSingleObject(e, INFINITE);
			CloseHandle(e);
		}
	}

	// 메모리 해제
	COM_RELEASE(fence);
	COM_RELEASE(bufferList[0]);
	COM_RELEASE(bufferList[1]);
	COM_RELEASE(descHeap);
	COM_RELEASE(swapChain);
	COM_RELEASE(dxgiFactory);
	COM_RELEASE(commandQueue);
	COM_RELEASE(commandList);
	COM_RELEASE(commandAllocator);
	COM_RELEASE(device);
	COM_RELEASE(debug);

	return 0;
}
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

typedef struct _VERTEX
{
	float x;
	float y;
	float z;
} VERTEX;

ID3D12Resource* CreateCommittedResource(ID3D12Device* device, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initState, UINT64 bufferSize)
{
	D3D12_HEAP_PROPERTIES heapProp;
	heapProp.Type = heapType;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC rescDesc;
	rescDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rescDesc.Alignment = 0;
	rescDesc.Width = bufferSize;
	rescDesc.Height = 1;
	rescDesc.DepthOrArraySize = 1;
	rescDesc.MipLevels = 1;
	rescDesc.Format = DXGI_FORMAT_UNKNOWN;
	rescDesc.SampleDesc.Count = 1;
	rescDesc.SampleDesc.Quality = 0;
	rescDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rescDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* bufferResource = NULL;
	device->lpVtbl->CreateCommittedResource(device, &heapProp, D3D12_HEAP_FLAG_NONE, &rescDesc, initState, NULL, &IID_ID3D12Resource, &bufferResource);

	return bufferResource;
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
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_SHIELD);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = NULL;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"DX12BaseTutorialClass";
		ATOM atom = RegisterClass(&wndClass);
		RETURN_IF_ZERO(atom);
	}

	// ????????? ??????
	HWND hWnd = CreateWindow(L"DX12BaseTutorialClass", L"DX12", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		100, 0, 300, 200, NULL, NULL, hInstance, NULL);

	// ????????? ????????? ??????
	ID3D12Debug* debug = NULL;
	D3D12GetDebugInterface(&IID_ID3D12Debug, &debug);
	RETURN_IF_ZERO(debug);
	debug->lpVtbl->EnableDebugLayer(debug);

	// ???????????? ??????
	ID3D12Device* device = NULL;
	D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, &device);
	RETURN_IF_ZERO(device);

	// ????????? 3?????? ??????
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

	// ????????? ??????
	IDXGIFactory2* dxgiFactory = NULL;
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory2, &dxgiFactory);

	// ?????? ?????? ??????
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

	// ????????? ??? ??????
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
		// CPU ????????? ?????? ?????????: ?????? ????????? ????????? ??????????????? ?????? ????????? ??????
		((void(__stdcall *)(ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE*))
			descHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart)(descHeap, &rtvHandle);

		// ?????? ?????? ??????
		for (int i = 0; i < 2; i++)
		{
			swapChain->lpVtbl->GetBuffer(swapChain, i, &IID_ID3D12Resource, &bufferList[i]);
			RETURN_IF_ZERO(bufferList[i]);

			// ??? ??????????????? Desc ?????? ?????? ????????? NULL??? ????????? ??????
			device->lpVtbl->CreateRenderTargetView(device, bufferList[i], NULL, rtvHandle);
			rtvHandle.ptr += rtvHeapSize;
		}
	}

	// ?????? ??????
	UINT64 fenceValue = 0;
	ID3D12Fence* fence = NULL;
	device->lpVtbl->CreateFence(device, fenceValue, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &fence);
	RETURN_IF_ZERO(fence);

	// ????????? ?????????
	UINT backBufferIndex = 0;

	// ????????? ????????? ??????
	VERTEX vertexList[] = {
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ 1, 0, 0 },
	};

	ID3D12Resource* uploadBuffer = CreateCommittedResource(device, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, sizeof(vertexList));
	RETURN_IF_ZERO(uploadBuffer);

	ID3D12Resource* defaultBuffer = CreateCommittedResource(device, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, sizeof(vertexList));
	RETURN_IF_ZERO(defaultBuffer);

	// ???????????? ??????
	while (gQuit == FALSE)
	{
		MSG msg;
		PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// ?????????
		commandAllocator->lpVtbl->Reset(commandAllocator);
		commandList->lpVtbl->Reset(commandList, commandAllocator, NULL);

		// ????????? ??????
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

			{
				float clearColor[] = { 1,0,0,1 };
				D3D12_RECT clearRect;
				clearRect.left = 0;
				clearRect.top = 0;
				clearRect.right = 300 / 2;
				clearRect.bottom = 200 / 2;
				commandList->lpVtbl->ClearRenderTargetView(commandList, rtvHandle, clearColor, 1, &clearRect);
			}
			{
				float clearColor[] = { 0,1,0,1 };
				D3D12_RECT clearRect;
				clearRect.left = 300 / 2;
				clearRect.top = 200 / 2;
				clearRect.right = 300;
				clearRect.bottom = 200;
				commandList->lpVtbl->ClearRenderTargetView(commandList, rtvHandle, clearColor, 1, &clearRect);
			}
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
		// ????????? ?????? ???
		
		// ????????? ??????
		commandList->lpVtbl->Close(commandList);
		ID3D12CommandList* commandListList[] = { (ID3D12CommandList*)commandList };
		commandQueue->lpVtbl->ExecuteCommandLists(commandQueue, 1, commandListList);

		// ????????? ?????? ??????
		swapChain->lpVtbl->Present(swapChain, 0, 0);
		backBufferIndex = 1 - backBufferIndex;

		// ?????? ??? ?????? ??? Signal
		// ????????? ???????????? ?????? ???????????? InterlockedIncrement64 ??????
		fenceValue += 1;
		commandQueue->lpVtbl->Signal(commandQueue, fence, fenceValue);

		// ???????????? ??? ?????? ???????????? ????????? ????????? ??????
		if (fence->lpVtbl->GetCompletedValue(fence) < fenceValue)
		{
			// win32 Event
			HANDLE e = CreateEvent(NULL, FALSE, FALSE, NULL);
			fence->lpVtbl->SetEventOnCompletion(fence, fenceValue, e);
			WaitForSingleObject(e, INFINITE);
			CloseHandle(e);
		}
	}

	// ????????? ??????
	COM_RELEASE(defaultBuffer);
	COM_RELEASE(uploadBuffer);
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
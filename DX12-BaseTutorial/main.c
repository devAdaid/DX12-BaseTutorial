#include <Windows.h>
#define RETURN_IF_ZERO(v) if (v == 0) return __LINE__

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
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
	CreateWindow(L"DX12BaseTutorialClass", L"DX12", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		100, 0, 300, 200, NULL, NULL, hInstance, NULL);
	return 0;
}
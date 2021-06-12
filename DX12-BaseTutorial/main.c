#include <Windows.h>

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	WNDCLASS wndClass;    
	wndClass.style = 0;
	wndClass.lpfnWndProc = NULL;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"DX12BaseTutorialClass";
	ATOM atom = RegisterClass(&wndClass);

	if (atom == 0)
	{
		return 1;
	}

	return 0;
}
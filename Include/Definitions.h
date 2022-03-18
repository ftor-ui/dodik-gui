//---------------//
//               //
//WinAPI declares//
//               //
//---------------//

TCHAR szWindowClass[] = _T("WindowClass");
TCHAR szTitle[] = _T("D0D1K");

long long int __stdcall WindowProc(HWND, UINT, WPARAM, LPARAM);
void InitClass(TCHAR *, HINSTANCE, WNDPROC);
HWND InitWindow(TCHAR *, TCHAR *, LONG, LONG, int, int, HINSTANCE, int);

//----------------//
//                //
//Widgets declares//
//                //
//----------------//

#define BUTTON_LAUNCH 1
#define BUTTON_BROWSE 2
#define BUTTON_HELP 3

HBITMAP hBackground = NULL;
HBITMAP hHelp = NULL;
HBITMAP hStart = NULL;
HBITMAP hStop = NULL;
HBITMAP hImage = NULL;
void InitWidgets(HWND);

HWND hwndHost = NULL;
HWND hwndPort = NULL;
HWND hwndMethod = NULL;
HWND hwndThreads = NULL;
HWND hwndTimeout = NULL;
HWND hwndProxy = NULL;
HWND hwndBrowse = NULL;
HWND hwndHelp = NULL;
HWND hwndHelpPrompt = NULL;
HWND hwndLaunch = NULL;
HWND hwndLaunchPrompt = NULL;

//----------------------//
//                      //
//Threads&Logic declares//
//                      //
//----------------------//

void *ThreadSend(void *);
size_t writefunction(char *, size_t, size_t, void *);
void ButtonLaunch(HWND);
void StartThreads();
void StopThreads();

struct data {
	TCHAR host[1024];
	TCHAR port[1024];
	TCHAR method[1024];
	FILE *proxy;
	int timeout;
	int wait;
	int status;
};
typedef struct data data;

data dataArgs = {0};
pthread_t *hThreads = NULL;
int iThreads = 1;
BOOLEAN bStarting = 0;

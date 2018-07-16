#include "windows.h"
#include "Gdiplus.h"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cmath>

using std::cout; using std::endl; using std::vector; using std::pair; using std::unordered_set; using std::abs;
using namespace Gdiplus;

const int BG_R = 0;
const int BG_G = 0;
const int BG_B = 0;

WCHAR const *fileName = L"ExcitedSteven.jpg";

static POINTS ptsBegin;

struct gameComp {
    int x = 0;
    int y = 0;
    Bitmap* bitmap;
    int width = 0;
    int height = 0; 
    bool initialized = false;
    unordered_set<int> pixelLoc;
};

struct pixel {
  union {
    struct {
      /* 'a' unused, used for 32-bit alignment,
       * could also be used to store pixel alpha
       */
      unsigned char b, g, r, a;
    };
    int val;
  };
  pixel() {
    val = 0;
  }
};

static gameComp BasicSteven;

// Window client size
const int WND_X_COORD = 0;
const int WND_Y_COORD = 0;
int WND_LENGTH = 500;
int WND_HEIGHT = 500;

const char *g_szClassName = "myWindowClass";
const char *windowTitle = "Click for a Steven!";

// Global Windows/Drawing variables
HBITMAP hbmp;
HANDLE hTickThread;
HWND hwnd;
HDC hdcMem;
// Pointer to pixels (will automatically have space allocated by CreateDIBSection
pixel *pixels;

void MakeSurface(HWND hwnd) {
  /* Use CreateDIBSection to make a HBITMAP which can be quickly
   * blitted to a surface while giving 100% fast access to pixels
   * before blit.
   */
  // Desired bitmap properties
  BITMAPINFO bmi;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
  bmi.bmiHeader.biWidth = WND_LENGTH;
  bmi.bmiHeader.biHeight =  -WND_HEIGHT; // Order pixels from top to bottom
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
  bmi.bmiColors[0].rgbBlue = 255;
  bmi.bmiColors[0].rgbGreen = 255;
  bmi.bmiColors[0].rgbRed = 255;
  bmi.bmiColors[0].rgbReserved = 0;
  HDC hdc = GetDC( hwnd );
  // Create DIB section to always give direct access to pixels
  hbmp = CreateDIBSection( hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0 );
  DeleteDC( hdc );
  // Create a new thread to use as a timer
  // hTickThread = CreateThread( NULL, NULL, &tickThreadProc, NULL, NULL, NULL );
}

void InitializeBasicSteven() {
    Image* newImage = Image::FromFile(fileName, true);
    BasicSteven.bitmap = Bitmap::FromFile(fileName, true);
    BasicSteven.width = newImage->GetWidth();
    BasicSteven.height = newImage->GetHeight();
    pixel *p;
    for(int i = 0; i < BasicSteven.width; ++i) {
        if(i >= WND_LENGTH) {
            break;
        }
        for(int j = 0; j < BasicSteven.height; ++j) {
            if(j >= WND_HEIGHT) {
                break;
            }
            Color c;
            BasicSteven.bitmap->GetPixel(i, j, &c);
            if((int) c.GetR() != 0 && (int) c.GetG() != 0 && (int) c.GetB() != 0) {
                BasicSteven.pixelLoc.insert(j * WND_LENGTH + i);
            }
        }
    }
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // HDC hdc;                       // handle to device context    
    RECT rcClient;                 // client area rectangle 
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 
    static HBITMAP bmpSource = NULL;
    static HDC hdcSource = NULL;

    HDC hdcDestination;

    switch(msg)
    {
        case WM_CREATE:
        {
            MakeSurface(hwnd);
            return 0;
        }
        case WM_LBUTTONDOWN:
        { 
            // Capture mouse input. 
            SetCapture(hwnd); 
 
            // Convert the cursor coordinates into a POINTS 
            // structure, which defines the beginning point of the 
            // line drawn during a WM_MOUSEMOVE message. 
 
            ptsBegin = MAKEPOINTS(lParam);

            ShowWindow( hwnd, SW_SHOW );
            // Retrieve the window's DC
            HDC hdc = GetDC( hwnd );
            // Create DC with shared pixels to variable 'pixels'
            hdcMem = CreateCompatibleDC( hdc );
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmp);
            // Milliseconds to wait each frame

            pixel *p;

            BasicSteven.x = ptsBegin.x;
            BasicSteven.y = ptsBegin.y;

            for(int i : BasicSteven.pixelLoc) {
                p = &pixels[i];
                p->r = BG_R;
                p->g = BG_G;
                p->b = BG_B;
            }
            BasicSteven.pixelLoc.clear();

            for(int i = 0; i < BasicSteven.width; ++i) {
                if(i >= WND_LENGTH) {
                    break;
                }
                for(int j = 0; j < BasicSteven.height; ++j) {
                    if(j >= WND_HEIGHT) {
                        break;
                    }
                    Color c;
                    BasicSteven.bitmap->GetPixel(i, j, &c);
                    p = &pixels[(j + BasicSteven.y) * WND_LENGTH + i + BasicSteven.x];
                    BasicSteven.pixelLoc.insert((j + BasicSteven.y) * WND_LENGTH + i + BasicSteven.x);
                    p->r = (int) c.GetR();
                    p->g = (int) c.GetG();
                    p->b = (int) c.GetB();
                }
            }

            // Draw pixels to window
            BitBlt( hdc, 0, 0, WND_LENGTH, WND_HEIGHT, hdcMem, 0, 0, SRCCOPY );
            SelectObject( hdcMem, hbmOld );
            DeleteDC( hdc );
            return 0; 
        }
        case WM_LBUTTONUP: 
            ReleaseCapture(); 
            return 0; 
            //Standard stuff
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
   
    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WND_LENGTH = GetSystemMetrics(SM_CXSCREEN);
    WND_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;    //CS_HREDRAW | CS_VREDRAW; //Originally 0; currently redraw if size changes
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0; //No extra class memory
    wc.cbWndExtra    = 0; //No extra window memory
    wc.hInstance     = hInstance; //handle to instance
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION); //Predefined icon
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); //Predefined arrow
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //Uh?
    wc.lpszMenuName  = NULL; //Name of menu resource
    wc.lpszClassName = g_szClassName; //Name of window class
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION); //Small class icon

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        WND_X_COORD, WND_Y_COORD, WND_LENGTH, WND_HEIGHT,
        NULL, NULL, hInstance, NULL);

    // cout << "LENGTH " << WND_LENGTH << "and HEIGHT " << WND_HEIGHT << endl;

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    cout << "Uh Huh" << endl;
    InitializeBasicSteven();
    cout << "Noope" << endl;

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    GdiplusShutdown(gdiplusToken);
    return Msg.wParam;
}

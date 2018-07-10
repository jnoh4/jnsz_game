#include "windows.h"
#include "gdiplus.h"
#include <iostream>

using std::cout;
using std::endl;

const int WND_X_COORD = 400;
const int WND_Y_COORD = 100;
const int WND_LENGTH = 500;
const int WND_HEIGHT = 500;

const char *g_szClassName = "myWindowClass";
const char *windowTitle = "Draw a line!";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;                       // handle to device context 
    RECT rcClient;                 // client area rectangle 
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 
    static POINTS ptsBegin;        // beginning point 
    static POINTS ptsEnd;          // new endpoint 
    static POINTS ptsPrevEnd;      // previous endpoint 
    static BOOL fPrevLine = FALSE; // previous line flag 
    switch(msg)
    {
       case WM_LBUTTONDOWN: 
 
            // Capture mouse input. 
 
            SetCapture(hwnd); 
 
            // Retrieve the screen coordinates of the client area, 
            // and convert them into client coordinates. 
 
            GetClientRect(hwnd, &rcClient); 
            ptClientUL.x = rcClient.left; 
            ptClientUL.y = rcClient.top;
 
            // Add one to the right and bottom sides, because the 
            // coordinates retrieved by GetClientRect do not 
            // include the far left and lowermost pixels. 
 
            ptClientLR.x = rcClient.right + 1; 
            ptClientLR.y = rcClient.bottom + 1; 
            ClientToScreen(hwnd, &ptClientUL); 
            ClientToScreen(hwnd, &ptClientLR); 
 
            // Copy the client coordinates of the client area 
            // to the rcClient structure. Confine the mouse cursor 
            // to the client area by passing the rcClient structure 
            // to the ClipCursor function. 
 
            SetRect(&rcClient, ptClientUL.x, ptClientUL.y, 
                ptClientLR.x, ptClientLR.y); 
            ClipCursor(&rcClient); 

            cout << ptClientUL.x << "and "<< ptClientUL.y << "and "<< endl;
 
            // Convert the cursor coordinates into a POINTS 
            // structure, which defines the beginning point of the 
            // line drawn during a WM_MOUSEMOVE message. 
 
            ptsBegin = MAKEPOINTS(lParam); 
            cout << ptsBegin.x << "and "<< ptsBegin.y << endl;
            return 0; 
 
        // case WM_MOUSEMOVE: 
 
        //     // When moving the mouse, the user must hold down 
        //     // the left mouse button to draw lines. 
 
        //     if (wParam & MK_LBUTTON) 
        //     { 
 
        //         // Retrieve a device context (DC) for the client area. 
 
        //         hdc = GetDC(hwnd);
        //         Gdiplus::Graphics gobj(hdc);
        //         Gdiplus::Pen wpen(Gdiplus::Color(255, 255, 255, 255));
        //         Gdiplus::Pen bpen(Gdiplus::Color(255, 0, 0, 0));
 
        //         // The following function ensures that pixels of 
        //         // the previously drawn line are set to white and 
        //         // those of the new line are set to black. 
 
        //         //////SetROP2(hdc, R2_NOTXORPEN);
 
        //         // If a line was drawn during an earlier WM_MOUSEMOVE 
        //         // message, draw over it. This erases the line by 
        //         // setting the color of its pixels to white. 
 
        //         if (fPrevLine) 
        //         { 
        //             gobj.DrawLine(&wpen, ptsBegin.x, ptsBegin.y, ptsPrevEnd.x, ptsPrevEnd.y);
        //             // MoveToEx(hdc, ptsBegin.x, ptsBegin.y, 
        //             //     (LPPOINT) NULL); 
        //             // LineTo(hdc, ptsPrevEnd.x, ptsPrevEnd.y); 
        //         } 
 
        //         // Convert the current cursor coordinates to a 
        //         // POINTS structure, and then draw a new line. 
 
        //         ptsEnd = MAKEPOINTS(lParam); 
        //         gobj.DrawLine(&bpen, ptsBegin.x, ptsBegin.y, ptsEnd.x, ptsEnd.y);
        //         // MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL); 
        //         // LineTo(hdc, ptsEnd.x, ptsEnd.y); 
 
        //         // Set the previous line flag, save the ending 
        //         // point of the new line, and then release the DC. 
 
        //         fPrevLine = TRUE; 
        //         ptsPrevEnd = ptsEnd; 
        //         ReleaseDC(hwnd, hdc); 
        //     } 
        //     break; 
 
        case WM_LBUTTONUP: 
 
            // The user has finished drawing the line. Reset the 
            // previous line flag, release the mouse cursor, and 
            // release the mouse capture. 
 
            fPrevLine = FALSE; 
            ClipCursor(NULL); 
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

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW; //Originally 0; currently redraw if size changes
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0; //No extra class memory
    wc.cbWndExtra    = 0; //No extra window memory
    wc.hInstance     = hInstance; //handle to instance
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION); //Predefined icon
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW); //Predefined arrow
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); //Uh?
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

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "conGraph.h"


/// Global Variables ///

static HINSTANCE gInstance;
controller       graphCon;
bool             drawGraph = true;  // Record if we need to redraw graph
bool             drawFocus = false;
UINT             controlMsgs[] = { IDE_MVALUE,IDC_OPERAND,IDE_BVALUE,IDS_BORDER,IDS_FUNCTION,IDS_YLABEL,IDS_XLABEL,
                                   IDB_DRAWGRAPH,IDS_BORDER2,IDS_XMIN,IDE_XMIN,IDS_RANGE,IDE_RANGE,IDS_PRECISION,
                                   IDE_PRECISION,IDS_FOCUSPNT };


/// Functions List ///

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE gInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = gInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(DKGRAY_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "Win32 Main Window";
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    
    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "Win32 Main Window",
        "Graph Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, gInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    
    return Msg.wParam;
    
}


LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // Post: Main Window Proceedure
    
    switch(msg)
    {
        case WM_CREATE:
        {      
               graphCon.createGUI( hwnd, gInstance, controlMsgs );
               graphCon.calculateGraphStats( hwnd );
        }
        break;
        case WM_SIZE:
        {
                // Reallign controls & graph when client window is resized
                
                RECT clientR;
                GetClientRect( hwnd, &clientR );
                
                // Check that client area is not less than minimum width & height
                if ( clientR.right >400 && clientR.bottom >400 ) 
                {
                   graphCon.resizeClientArea( hwnd, controlMsgs );
                   graphCon.calculateGraphStats( hwnd );
                   drawGraph = true;
                   InvalidateRect( hwnd, NULL, true );
                }
                else MessageBox( hwnd, "Graph cannot be drawn when window is less than 400/400 pixels",
                                 "Error", MB_OK|MB_ICONERROR );
        }
        break;
        case WM_MOUSEMOVE:
        {    
             /* Not implemented yet in Version 1
             if ( graphCon.collisionRect( LOWORD(lParam), HIWORD(lParam) ) == true ) 
             {
                 // if focusPnt has changed
                 if ( graphCon.findInstanceNearest(hwnd,IDS_FOCUSPNT,LOWORD(lParam),HIWORD(lParam)) == true ) 
                 {  
                    // find nearest function line pnt to mouse pos                                                      
                    drawFocus = true;
                    graphCon.eraseFocus(hwnd);
                 }
             } 
             */
        }     
        break;
        case WM_COMMAND:
        {
             switch( LOWORD(wParam) ) 
             {
                
                case IDB_DRAWGRAPH:
                {    
                     // Draw graph
                     
                     // if all input variables are valid
                     if ( graphCon.getValues( hwnd, controlMsgs ) == true ) 
                     {
                         graphCon.calculateGraphStats( hwnd );
                         drawGraph = true;
                         InvalidateRect( hwnd, NULL, true );                          
                     }
                     else MessageBox( hwnd, "One or more input fields are blank or invalid. \nValid input includes numerical data & '-' operand.",
                                      "Error", MB_OK|MB_ICONERROR );
                }     
                break;
                default:
                break;                    
             }

        }    
        break;
        case WM_PAINT:
        {
              HDC hdc;
              PAINTSTRUCT ps;
              
              hdc = BeginPaint( hwnd, &ps );
              
              if ( drawFocus ) 
              {
                  graphCon.drawFocus( hdc );
                  drawFocus = false;           
              }
              
              if ( drawGraph ) 
              {
                  graphCon.drawGraph( hwnd, hdc );
                  drawGraph = false;
              }
              
              EndPaint( hwnd, &ps );
        }
        break;
        case WM_CLOSE:
        {
            graphCon.garbageCollection(); // release dynamic data
            DestroyWindow( hwnd );
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
        break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



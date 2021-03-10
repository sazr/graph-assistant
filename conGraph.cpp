#include <windows.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include "conGraph.h"

using namespace std;


/// Graph Controller Class ///

controller :: controller()
{
     // Initialise class variables to their default values
   
     m             = 2;
     b             = -4;
     operand       = '+';
     
     xMin          = INT_MAX;
     yMin          = INT_MAX;
     xMax          = INT_MIN;
     yMax          = INT_MIN;
     
     xZero         = 20;
     yZero         = 450;
     minCellDim    = 10;
     cellWidth     = minCellDim;
     cellHeight    = minCellDim;
     xPrecision    = 1;
     yPrecision    = 1;
     // precision  = 1;
     xBegin        = -25;
     xEnd          = 5;
     range         = int(xEnd-xBegin);
     xCellNum      = int((xMax-xMin)/xPrecision);
     yCellNum      = int((yMax-yMin)/yPrecision);
     // xCellNum   = int((xMax-xMin)/precision);
     // yCellNum   = int((yMax-yMin)/precision);
     
     // Set default strokes
     xBorder       = CreatePen(PS_SOLID,1,RGB(255,0,0));
     yBorder       = CreatePen(PS_SOLID,1,RGB(255,0,0));
     functionLine  = CreatePen(PS_SOLID,2,RGB(0,220,40));
     intervalLine  = CreatePen(PS_SOLID,1,RGB(255,255,255));     
     graphRgn      = CreateRectRgn(0,0,1,1);  
     focusRgn      = CreateRectRgn(0,0,1,1);
     
}


void controller :: createGUI( HWND hwnd, HINSTANCE gInstance, UINT controlMsg[] )
{
     // Post: Create GUI to alter graph
     
     RECT   rRect     = {0};
     GetClientRect( hwnd, &rRect );
     HFONT  hfDefault = (HFONT) GetStockObject( DEFAULT_GUI_FONT );
     int    dataCB[4] = {43,45,42,47};
     char*  optCB[4]  = {" +"," -"," *"," /"}; 
     
     int GUIx        = 0;
     int GUIy        = rRect.bottom-100;
    
     HWND stBorder = CreateWindowEx(0,"Static","",WS_BORDER| WS_CHILD| WS_VISIBLE,//| SS_GRAYRECT,
                                    GUIx,GUIy,rRect.right,100,hwnd,(HMENU)controlMsg[3],gInstance,NULL);
     HWND stfunctionLabel = CreateWindowEx(0,"Static","",WS_BORDER| WS_CHILD| WS_VISIBLE,
                                          GUIx+10,GUIy+15,150,33,hwnd,(HMENU)controlMsg[4],gInstance,NULL);
     HWND styLabel = CreateWindowEx(0,"Static"," y = ",WS_CHILD| WS_VISIBLE,
                                          GUIx+13,GUIy+18,25,20,hwnd,(HMENU)controlMsg[5],gInstance,NULL);
     HWND edMValue = CreateWindowEx(0,"Edit","m",WS_BORDER| WS_CHILD| WS_VISIBLE| ES_NUMBER| ES_CENTER,
                                    GUIx+42,GUIy+15,20,22,hwnd,(HMENU)controlMsg[0],gInstance,NULL);
     HWND stxLabel = CreateWindowEx(0,"Static","x",WS_CHILD| WS_VISIBLE,
                                    GUIx+65,GUIy+18,10,20,hwnd,(HMENU)controlMsg[6],gInstance,NULL);
     HWND cbOperand = CreateWindowEx(0,"Combobox","",WS_BORDER| WS_CHILD| WS_VISIBLE| CBS_DROPDOWNLIST,
                                    GUIx+76,GUIy+15,40,100,hwnd,(HMENU)controlMsg[1],gInstance,NULL);
     HWND edBValue = CreateWindowEx(0,"Edit","b",WS_BORDER| WS_CHILD| WS_VISIBLE| ES_NUMBER| ES_CENTER,
                                    GUIx+116,GUIy+15,20,22,hwnd,(HMENU)controlMsg[2],gInstance,NULL);
     HWND btDrawGrph = CreateWindowEx(0,"Button","Draw Graph",WS_BORDER|WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
                                      rRect.right-87,rRect.bottom-40,80,33,hwnd,(HMENU)controlMsg[7],gInstance,NULL);
     HWND stLimitsLabel = CreateWindowEx(0,"Static","",WS_BORDER|WS_CHILD|WS_VISIBLE,GUIx+10,GUIy+50,150,44,
                                        hwnd,(HMENU)controlMsg[8],gInstance,NULL);
     HWND stXMin = CreateWindowEx(0,"Static","x min=",WS_CHILD|WS_VISIBLE,GUIx+13,GUIy+52,50,20,
                                  hwnd,(HMENU)controlMsg[9],gInstance,NULL);
     HWND edXMin = CreateWindowEx(0,"Edit","0",WS_BORDER|WS_CHILD|WS_VISIBLE|ES_RIGHT,GUIx+48,GUIy+50,30,18,
                                  hwnd,(HMENU)controlMsg[10],gInstance,NULL);
     HWND stRange = CreateWindowEx(0,"Static","Range=",WS_CHILD|WS_VISIBLE,GUIx+83,GUIy+52,50,20,
                                   hwnd,(HMENU)controlMsg[11],gInstance,NULL);
     HWND edRange = CreateWindowEx(0,"Edit","10",WS_BORDER|WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,GUIx+125,GUIy+50,30,18,
                                  hwnd,(HMENU)controlMsg[12],gInstance,NULL);
     HWND stPrec = CreateWindowEx(0,"Static","Precision=",WS_CHILD|WS_VISIBLE,GUIx+72,GUIy+72,50,17,
                                  hwnd,(HMENU)controlMsg[13],gInstance,NULL);
     HWND edPrec = CreateWindowEx(0,"Edit","1",WS_BORDER|WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,GUIx+125,GUIy+70,30,18,
                                  hwnd,(HMENU)controlMsg[14],gInstance,NULL);
     HWND stFocusPnt = CreateWindowEx(0,"Static","X = 0   Y = 0",WS_BORDER|WS_CHILD|WS_VISIBLE|SS_CENTER,
                                      rRect.right-100,GUIy+10,90,45,hwnd,(HMENU)controlMsg[15],gInstance,NULL);
    
     // Set control fonts
     for ( int i=0; i<15; i++ ) 
     {
         SendDlgItemMessage( hwnd, controlMsg[i], WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0) );
     }     
     
     // Add operand options to CB cbOperand
     for ( int i=0; i<4; i++ ) 
     {
         SendDlgItemMessage( hwnd, controlMsg[1], CB_ADDSTRING, 0, (LPARAM)optCB[i] );
     }
     
     /*
     SendDlgItemMessage(hwnd,controlMsg[1],CB_ADDSTRING,0,(LPARAM)" +");
     SendDlgItemMessage(hwnd,controlMsg[1],CB_ADDSTRING,0,(LPARAM)" -");
     SendDlgItemMessage(hwnd,controlMsg[1],CB_ADDSTRING,0,(LPARAM)" *");
     SendDlgItemMessage(hwnd,controlMsg[1],CB_ADDSTRING,0,(LPARAM)" /");
     */
     
     SendDlgItemMessage( hwnd, controlMsg[1], CB_SETCURSEL, 0, 0 ); // select 1st cell of LB
     
}


void controller :: resizeClientArea( HWND hwnd, UINT controlMsgs[] )
{
   // Post: Reallign controls when client area is resized
   
   RECT rRect = {0};
   GetClientRect( hwnd, &rRect );    
   int  GUIx  = 0;
   int  GUIy  = rRect.bottom-100;
   
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[3]),  HWND_TOPMOST, GUIx,            GUIy,             rRect.right, 100, SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[4]),  HWND_TOPMOST, GUIx+10,         GUIy+10,          150,         33,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[5]),  HWND_TOPMOST, GUIx+13,         GUIy+18,          25,          20,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[0]),  HWND_TOPMOST, GUIx+42,         GUIy+15,          20,          22,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[6]),  HWND_TOPMOST, GUIx+65,         GUIy+18,          10,          20,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[2]),  HWND_TOPMOST, GUIx+116,        GUIy+15,          20,          22,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[7]),  HWND_TOPMOST, rRect.right-87,  rRect.bottom-40,  80,          33,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[8]),  HWND_TOPMOST, GUIx+10,         GUIy+47,          150,         44,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[9]),  HWND_TOPMOST, GUIx+13,         GUIy+52,          50,          20,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[10]), HWND_TOPMOST, GUIx+48,         GUIy+50,          30,          18,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[11]), HWND_TOPMOST, GUIx+83,         GUIy+52,          50,          20,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[12]), HWND_TOPMOST, GUIx+125,        GUIy+50,          30,          18,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[13]), HWND_TOPMOST, GUIx+72,         GUIy+72,          50,          17,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[14]), HWND_TOPMOST, GUIx+125,        GUIy+70,          30,          18,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[15]), HWND_TOPMOST, rRect.right-100, GUIy+10,          90,          45,  SWP_NOZORDER);
   SetWindowPos( GetDlgItem(hwnd,controlMsgs[1]),  HWND_TOP,     GUIx+76,         GUIy+15,          40,          100, SWP_NOZORDER);
   
   // need to reset precision when window is resized
   //precision = 1;
   xPrecision = 1;
   yPrecision = 1;
   
}


float controller :: equation( float xVal )
{
   // Post: Calculate Y value, ie, Determine y = mx(?)b, according to the opperand type/value
   // Maybe rename function to findYValue();
   
   
   switch ( operand )
   {
       case '-':
       {
            return ((m*xVal)-b);
       }     
       break;
       case '*':
       {
            return ((m*xVal)*b);
       }     
       break;
       case '/':
       {
            return ((m*xVal)/b);
       }     
       break;
       // case '+'
       default:
       {
            return ((m*xVal)+b);  
       }
       break;
   } 
                   
}
      
      
void controller :: plotActualGraph()
{
     // Post: Calculate x,y data points for graph
     //       - xScaled = xZero + (x*cellWidth);
     //       - yScaled = yZero - (y*cellHeight);
     
     functPnt.clear();       // clear vectors 
     xMax = INT_MIN; 
     yMax = INT_MIN;
     xMin = 0; // INT_MAX; 
     yMin = 0; // INT_MAX;
    
     // Calculate x,y values & store min & max x,y values
     for ( float xVal=xBegin; xVal<=xEnd; xVal+=1 ) 
     { 
         
         POINT *p   = new POINT;
         float yVal = equation(xVal);  // determine y = mx + b
         p->x       = (LONG)xVal; 
         p->y       = (LONG)yVal;
         functPnt.push_back(p);
         
         // store min & max values
         if (xVal > xMax) { xMax = xVal; }
         if (yVal > yMax) { yMax = yVal; }
         if (xVal < xMin) { xMin = xVal; }
         if (yVal < yMin) { yMin = yVal; }
     
     }
     
     
     /*if (xMax < 0) {
        xMax = 0;
     }
     if (yMax < 0) {
        yMax = 0;
     }
     
     //range = (xMax-xMin); */
}


void controller :: plotScaledGraph()
{
     // Pre:  Vector variables x & y must have been defined & given values 
     //       & cell dimensions & min/max values calculated
     // Post: Create function line scaled to client area
     
     scaledPnt.clear();  // clear vectors
     
     for ( int i=0; i<functPnt.size(); i+=1 ) 
     { 
         
         // scale x,y values for optimal graphical display
         POINT *p    = new POINT;
         float xVal  = functPnt.at(i)->x;
         float yVal  = functPnt.at(i)->y;
         float xTemp = xZero + ((xVal-xMin)*cellWidth);
         float yTemp = yZero - ((yVal-yMin)*cellHeight);
         p->x        = (LONG)xTemp; p->y = (LONG)yTemp;
         scaledPnt.push_back(p);
    
    }
    
}


void controller :: debugGraph()
{
     // Post: Debugging function
     
     cout << "Actual x,y values: \n";
     
     for ( int i=0; i<scaledPnt.size(); i++ ) 
     {
         
         //if (i%10 !=0) { cout << endl; }
         
         cout << functPnt.at(i)->x << "," << functPnt.at(i)->y << " ";
         cout << scaledPnt.at(i)->x << "," << scaledPnt.at(i)->y << "\n";
     }
}


void controller :: calculateGraphStats( HWND hwnd )
{
    // Post: Using the user specified precision, xBegin & range we calculate the
    //       optimal cell height & width to fit the graph on screen. In some cases
    //       the range may be too large for the graph to fit on the screen. To solve 
    //       this, the precision will be overwritten & set to a more optimal number.
      
    RECT rRect = {0};
    GetClientRect( hwnd, &rRect );  // get the client area dimensions

    plotActualGraph();              // calculate/plot graph x,y values
    
    // find optimal cell dimensions
    int availableWidth  = (rRect.right-rRect.left)-60;
    int availableHeight = (rRect.bottom-rRect.top)-140;
    
    xCellNum   = int((range)/xPrecision);
    yCellNum   = int((range)/yPrecision);
    // xCellNum   = int((xMax-xMin)/xPrecision);
    // yCellNum   = int((yMax-yMin)/yPrecision);
    cellWidth  = int(availableWidth/xCellNum);    
    cellHeight = int(availableHeight/yCellNum);      
    
    
    /*// if range is too large for client width OR height
    while (cellWidth<=minCellDim || cellHeight<=minCellDim) {
        precision += 1;
        xCellNum   = int((xMax-xMin)/precision);
        yCellNum   = int((yMax-yMin)/precision);
        cellWidth  = int(availableWidth/xCellNum);    
        cellHeight = int(availableHeight/yCellNum);
    }*/
    
    
    // Use client dimensions to set optimal cell width & height
    // if range is too large for client width OR height
    while ( cellWidth <= minCellDim ) 
    {
        xPrecision += 1;
        xCellNum    = int((range)/xPrecision);
        // xCellNum = int((xMax-xMin)/xPrecision);
        cellWidth   = int(availableWidth/xCellNum);    
    }
    
    while ( cellHeight <= minCellDim ) 
    {
        yPrecision += 1;
        yCellNum    = int((range)/yPrecision);
        // yCellNum = int((yMax-yMin)/yPrecision);   
        cellHeight  = int(availableHeight/yCellNum);
    }
    
    setCentre( hwnd, rRect );  // Find if the graph contains x or y negative values / Plot point (0,0)
    plotScaledGraph();         // Plot/fit graph to client area
    
}


void controller :: setCentre( HWND hwnd, RECT hRect )
{
     // Post: Obtain/Calculate the best position for graph within client area 
     //       & set beginning points of x(xXAxis,yXAxis) & y(xYAxis,yYAxis) axis.
     
     // Default x,y axis position |__
     xZero  = (hRect.left)+20; 
     yZero  = (hRect.bottom)-120;
     xXAxis = xZero;
     yXAxis = yZero;
     xYAxis = xZero;
     yYAxis = yZero;
     
     // if any x or y values are negative then they require x,y borders to be
     // moved
     if ( isNegative( functPnt, 'x' ) ) 
     { 
         //xYAxis += int( abs(cellWidth*(xMin/precision)));
         xYAxis += int( abs(cellWidth*(xMin/xPrecision)) );
     }
     if ( isNegative( functPnt, 'y' ) ) 
     {
         //yXAxis -= int( abs(cellHeight*(yMin/precision)));
         yXAxis -= int( abs(cellHeight*(yMin/yPrecision)) );            
     }
     
     graphRgn = CreateRectRgn( xXAxis, yYAxis, int(xXAxis+(cellWidth*(xCellNum+0.1))), int(yYAxis-(cellHeight*(yCellNum+0.5))) ); 
     
}


void controller :: drawGraph( HWND hwnd, HDC hdc )
{
     // Pre:  Central client area point(xZero,yZero) must be defined otherwise error
     // Post: Draw graph using win32 GDI functions   
     
     int   graphSize = functPnt.size()-1;
     float xBeg,yBeg;
     float xEnd,yEnd;
     
     drawIntervals( hdc );
     drawBorders( hwnd,hdc );
     drawText( hdc );
     
     
     /*
     // Used to draw Non-linear functions/Graphs
     // Draws a line from point to point
     
     for (int i=0; i<graphSize; i++) 
     {
          // Get the lines start point(x,y values)
          xBeg = scaledPnt.at(i)->x;
          yBeg = scaledPnt.at(i)->y;
          // Get the lines end point(x,y values)
          xEnd = scaledPnt.at(i+1)->x;
          yEnd = scaledPnt.at(i+1)->y;
          // Draw line
          drawLine(hdc,(int)xBeg,(int)yBeg,(int)xEnd,(int)yEnd,functionLine);
     }
     */
     
     
     // Used to draw linear functions/Graphs
     // draw function line from start point to end of graphs range
     
     drawLine( hdc, int(scaledPnt.at(0)->x) ,int(scaledPnt.at(0)->y),
               int(xXAxis+(cellWidth*xCellNum)),
               int(yYAxis-(cellHeight*yCellNum)), functionLine);
               
}


void controller :: drawBorders( HWND hwnd, HDC hdc )
{
     // Post: draw x scale & y scale lines/borders 
     
     /*
     RECT rRect;
     GetClientRect(hwnd,&rRect);  // get the client area dimensions
     int availableWidth  = (rRect.right-rRect.left)-60;
     int availableHeight = (rRect.bottom-rRect.top)-140; 
     
     drawLine(hdc,xXAxis,yXAxis,xXAxis+availableWidth,yXAxis,xBorder);  // draw x axis
     drawLine(hdc,xYAxis,yYAxis,xXAxis,yXAxis-availableHeight,yBorder); // draw y axis 
     */

     drawLine( hdc, xXAxis, yXAxis, int(xXAxis+(cellWidth*(xCellNum+0.1))), yXAxis, xBorder);   // draw x axis
     drawLine( hdc, xYAxis, yYAxis, xYAxis, int(yYAxis-(cellHeight*(yCellNum+0.3))), yBorder);  // draw y axis
     
}


void controller :: drawText( HDC hdc )
{
     // Post: Draw intervals labels (1,2,3,5);
     
     char  num[33];
     int   xb        = xXAxis-3;
     int   yb        = yXAxis+5; 
     HFONT hfDefault = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
     
     SelectObject( hdc, hfDefault );  // select default font
     SetBkMode( hdc, TRANSPARENT ); 
     //SetTextColor(hdc,RGB(255,255,255));
     
     // draw x axis numbers
     for ( float i=xMin; i<=xMax; i+=xPrecision ) 
     {
         itoa( (int)i, num ,10 );
         TextOut( hdc, xb, yb, (LPCTSTR)num, 3 );
         xb += (int)cellWidth;
     }
     
     
     xb = xYAxis-17;
     yb = yYAxis-6;
     
     // draw y axis numbers
     for ( float i=yMin; i<=yMax; i+=yPrecision ) 
     {
         itoa( (int)i, num, 10 );
         TextOut( hdc, xb, yb, (LPCTSTR)num, 3 );
         yb -= (int)cellHeight;
     }
     
     DeleteObject( hfDefault );  // release dynamic data
}


void controller :: drawIntervals( HDC hdc )
{
   // Post: Draws a line/mark at each cell interval along x & y axis   
   
   int x,y;
   
   // draw x axis markers
   for ( int i=0, x=(int)xXAxis, y=(int)yXAxis; i<=xCellNum; i++, x+=(int)cellWidth ) 
   {
        drawLine( hdc, x, y, x, y+4, intervalLine );
   } 
   
   // draw y axis markers
   for ( int i=0, x=(int)xYAxis, y=(int)yYAxis; i<=yCellNum; i++, y-=(int)cellHeight ) 
   {
        drawLine( hdc, x, y, x-4, y, intervalLine );
   } 
   
}


void controller :: drawFocus( HDC hdc )
{
   // Post: - Determine & identify regions above & below function line
   //       - Draw lines from focusPnt to X Axis & focusPnt to the Y Axis
   
   HRGN  xLine, yLine;
   POINT xRgn[2];
   POINT yRgn[2];
   
   xRgn[0].x = scaledFocusPnt.x-1; xRgn[0].y = scaledFocusPnt.y+1;
   xRgn[1].x = scaledFocusPnt.x+1; xRgn[1].y = xYAxis;   
   yRgn[0].x = scaledFocusPnt.x-1; yRgn[0].y = scaledFocusPnt.y-1;
   yRgn[1].x = xYAxis; yRgn[1].y = scaledFocusPnt.y+1;
   
   xLine = CreateRectRgn(xRgn[0].x,xRgn[0].y,xRgn[1].x,xRgn[1].y);
   yLine = CreateRectRgn(yRgn[0].x,yRgn[0].y,yRgn[1].x,yRgn[1].y);
   
   CombineRgn(focusRgn,xLine,yLine,RGN_XOR);  
   DeleteObject(xLine);
   DeleteObject(yLine);
   
   // Option 1:
   HBRUSH hb = CreateSolidBrush(RGB(0,220,40));
   FillRgn(hdc,focusRgn,(HBRUSH)hb);
   // Option 2:
   //drawLine(hdc,xRgn[0].x,xRgn[0].y,xRgn[1].x,xRgn[1].y,xBorder);
   //drawLine(hdc,yRgn[0].x,yRgn[0].y,yRgn[1].x,yRgn[1].y,xBorder);
   
}


void controller :: eraseFocus( HWND hwnd )
{
   // Post: Erases focusPnt's lines by invalidating client area
   
   InvalidateRgn( hwnd, focusRgn, true );   
   //InvalidateRgn(hwnd,xLine,true);
   //InvalidateRgn(hwnd,yLine,true);
    
}


bool controller :: isNegative( vector<POINT*> v, char axis )
{
   // Post: returns true if vector contains negative values
   
   if ( axis == 'x' ) 
   {
      for ( int i=0; i<v.size(); i++ ) 
      {
          POINT *p = v.at(i);
          // if number is negative
          if ( p->x < 0 ) 
          {
             return true;
          }
      }
      return false;      // vector contains no positive values      
   }    
   else if (axis=='y') 
   {
      for (int i=0; i<v.size(); i++) 
      {
          POINT *p = v.at(i);
          if ( p->y < 0 ) 
          {
             return true;
          }
      }
      return false;
   } 
   else return true;  // axis value is invalid
   
}


void controller :: drawLine( HDC hdc, int xBeg, int yBeg, int xEnd, int yEnd, HPEN hPen )
{
     // Post: draw x,y Line & any other required lines
     
     SelectObject( hdc, hPen ); // need to add code to check if SelectObject fails
     
     MoveToEx( hdc, xBeg, yBeg, NULL );
     LineTo( hdc, xEnd, yEnd );
}


bool controller :: getValues( HWND hwnd, UINT controlMsg[] )
{
   // Post: Retrives values from GUI controls to calculate & draw graph

   UINT msgs[] = {controlMsg[0],controlMsg[2],controlMsg[10],controlMsg[12],controlMsg[14]};
   int tempVars[5];
   
   for ( int i=0; i<5; i++ ) 
   {
        BOOL success;
        tempVars[i] = GetDlgItemInt( hwnd, msgs[i], &success, true );
        // if control input is invalid
        if ( !success ) 
        {
            MessageBox( hwnd, "One or more input fields are blank or invalid. \nValid input includes numerical data & '-' operand.",
                        "Error", MB_OK|MB_ICONERROR);
            return false;
        }  
   }

   getOpperand( hwnd, controlMsg[1] );  // Get operand (+,-,* or /)
   
   // Check for invalid functions 
   // Gradient (m) cannot be <= 0
   if ( tempVars[0] <= 0 ) 
   {
       return false;  
   }
   // Constant (b) cannot be <= 0 when we have a geometric function
   if ( tempVars[1]<=0 && operand=='*'  ||  tempVars[1]<=0  &&  operand=='/' ) 
   {
       return false;  
   }
   
   
   // Range cannot be <= 0 
   if ( tempVars[3]<=0 ) 
   {
       tempVars[3] = 1;
   }
   // Precision cannot be <= 0 
   if ( tempVars[4]<=0 ) 
   {
       tempVars[4] = 1;
   }
   
   m          = tempVars[0];
   b          = tempVars[1];
   xBegin     = tempVars[2];
   range      = tempVars[3];
   //precision  = tempVars[4];
   xPrecision = tempVars[4];
   yPrecision = tempVars[4];
   xEnd       = (xBegin+range);
   return true;
   
}


void controller :: getOpperand( HWND hwnd, UINT msg )
{
   // Post: sets operand according to Combobox selection
   
   int cell = SendDlgItemMessage( hwnd, msg, CB_GETCURSEL, 0, 0 );
   
   switch( cell ) 
   {
     
     case 0:  operand = '+';  break;
     case 1:  operand = '-';  break;
     case 2:  operand = '*';  break;
     case 3:  operand = '/';  break;
     default: operand = '+';  break;           
   }
    
}


bool controller :: collisionRect( int mouse_x, int mouse_y )
{
   // Post: Returns true if mouse collides with Graph region(above or below the 
   //       function line) else false
   
   if ( PtInRegion( graphRgn, mouse_x, mouse_y ) != 0 ) 
   {
       return true;                                   
   }
   else return false;
   
}


bool controller :: findInstanceNearest( HWND hwnd, UINT msg, int mouse_x, int mouse_y )
{
   // Post: Finds the closest 'function line' point to mouse position &
   //       updates Focus Point(static control text) 
   
   int xDist = INT_MAX;
   int yDist = INT_MAX;
   POINT closestPnt;
   
   // find which axis is closest to mouse pos
   int xAxisDist = abs(yXAxis-mouse_y);
   int yAxisDist = abs(xYAxis-mouse_x);
   
   if (xAxisDist <= yAxisDist) { // if x axis is closest
       for (int i=0; i<scaledPnt.size(); i++) {
           POINT *tempP = scaledPnt.at(i);
           int xTemp = abs(mouse_x-(int)tempP->x);
           int yTemp = abs(mouse_y-(int)tempP->y);
           
           if (xTemp < xDist) {
               xDist = xTemp;
               closestPnt.x = (int)functPnt.at(i)->x;
               closestPnt.y = (int)functPnt.at(i)->y;
           }
       } 
   }
   else { // y axis is closest
       for (int i=0; i<scaledPnt.size(); i++) {
           POINT *tempP = scaledPnt.at(i);
           int xTemp = abs(mouse_x-(int)tempP->x);
           int yTemp = abs(mouse_y-(int)tempP->y);
           
           if (yTemp < yDist) {
               yDist = yTemp;
               closestPnt.x = (int)functPnt.at(i)->x;
               closestPnt.y = (int)functPnt.at(i)->y;
           }
       }  
   }
   
   // if closestPnt != focusPnt
   if ( (closestPnt.x != focusPnt.x)  &&  (closestPnt.y != focusPnt.y) ) 
   {
      focusPnt         = closestPnt;
      scaledFocusPnt.x = LONG(xZero + ((focusPnt.x-xMin)*cellWidth));
      scaledFocusPnt.y = LONG(yZero - ((focusPnt.y-yMin)*cellHeight));
      char buf[MAX_PATH];
      sprintf(buf,"(%i, %i)",focusPnt.x,focusPnt.y);
      SendDlgItemMessage(hwnd,msg,WM_SETTEXT,0,(LPARAM)buf);  // Display closest pnt coords in Static Control
      return true;
   }
   else return false; // no need to redraw focus because focus pnt hasn't changed
   
}


void controller :: garbageCollection()
{
   // Post: Delete all dynamically created variables/objects. Release data
   
   // delete GDI objects:
   DeleteObject(xBorder);
   DeleteObject(yBorder);
   DeleteObject(functionLine);
   DeleteObject(intervalLine);
   DeleteObject(graphRgn);
   DeleteObject(focusRgn);
   
   // delete dynamic objects:
   while (!functPnt.empty()) 
   {         
         delete functPnt.back();
         functPnt.pop_back();
   }
   
   while (!scaledPnt.empty()) 
   {
         delete scaledPnt.back();
         scaledPnt.pop_back();
   }      
    
}



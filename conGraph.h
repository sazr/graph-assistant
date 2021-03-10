#ifndef CONGRAPH_H
#define CONGRAPH_H

#include <windows.h>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;


/// Constants ///

#define IDE_MVALUE    1
#define IDC_OPERAND   2
#define IDE_BVALUE    3
#define IDS_BORDER    4
#define IDS_FUNCTION  5
#define IDS_YLABEL    6
#define IDS_XLABEL    7
#define IDB_DRAWGRAPH 8
#define IDS_BORDER2   9
#define IDS_XMIN      10
#define IDE_XMIN      11
#define IDS_RANGE     12
#define IDE_RANGE     13
#define IDS_PRECISION 14
#define IDE_PRECISION 15
#define IDS_FOCUSPNT  16



// In the extended version(Graph Assistant 2.0) the graphs' line will be made 
// up of a series of points stored as a linked list.
struct node 
{       
       int x;
       int y;
       node *next;
};



/// Graph Controller Class ///

class controller
{
   public:
      controller();
      
      void  createGUI( HWND hwnd, HINSTANCE gInstance, UINT controlMsg[] );
      void  resizeClientArea( HWND hwnd, UINT controlMsgs[] );
      float equation( float xVal );
      void  plotActualGraph(); 
      void  plotScaledGraph();
      void  debugGraph();
      void  calculateGraphStats( HWND hwnd );
      void  setCentre( HWND hwnd, RECT hRect );  // set centre of x,y border graph Better name would be SetAxisMetrics()
      void  drawBorders( HWND hwnd, HDC hdc );
      void  drawGraph( HWND hwnd, HDC hdc );
      void  drawText( HDC hdc );
      void  drawIntervals( HDC hdc );
      void  drawFocus( HDC hdc );
      void  eraseFocus( HWND hwnd );
      bool  isNegative( vector<POINT*> v, char axis );
      void  drawLine( HDC hdc, int xBeg, int yBeg, int xEnd, int yEnd, HPEN hPen );
      bool  getValues( HWND hwnd, UINT controlMsg[] );
      void  getOpperand( HWND hwnd, UINT msg );
      bool  collisionRect( int mouse_x, int mouse_y );
      bool  findInstanceNearest( HWND hwnd, UINT msg, int mouse_x, int mouse_y );
      void  garbageCollection();
      
   private:
      float xBegin, xEnd;                    // the start of x values
      float precision;                       // the precision of graph (do the points go up by 1; eg [1,5],[2,9],[3,15] )
      float xPrecision, yPrecision;          // the precision of graph (do the points go up by 1; eg [1,5],[2,9],[3,15] ) 
      float range;                           // xBegin+range = xEnd;
      int   xCellNum,yCellNum;               // x,y axis intervals spacing
      int   xXAxis, yXAxis;                  // the actual position of the start pnt of x axis
      int   xYAxis, yYAxis;                  // the actual position of the start pnt of y axis
      int   xZero,yZero;                     // the actual position of graphs 0,0 on client window
      float cellWidth,cellHeight;            // the actual width between x values on x graph line
      float minCellDim;                      // the minimum cell dimension(height/width) in pixels
      vector <node*> graphPoints;
      
      // Equation Variables: y = mx + b
      float           m;
      float           b;
      char            operand;               // +,-,*,/
      vector <POINT*> functPnt;              // actual x,y values
      vector <POINT*> scaledPnt;             // x,y values scaled for visual purposes
      float           xMax, yMax;            // maxium values of actual x & y vectors
      float           xMin, yMin;            // minimum values for x & y
      
      // Graph Visual Variables 
      POINT focusPnt;
      POINT scaledFocusPnt;
      HPEN  xBorder;                         // choose stroke of graph lines
      HPEN  yBorder;
      HPEN  functionLine;
      HPEN  intervalLine;
      HRGN  graphRgn;
      HRGN  focusRgn;
};

#endif 


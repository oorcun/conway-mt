/**
 * Written by Orçun Altınsoy on February 2012
 */

#include <windows.h>
#include<stdio.h>

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[ ] = "Conway";

MSG messages;            /* Here messages to the application are saved */

char cell[234][234];
int neighbour[234][234];
char cache[234 * 234];
int cacher[234 * 234];
int cachec[234 * 234];
int cachesize;
int generation;
bool started;
bool threadsafe;
bool terminatethread;
bool terminateexecuted;
int speed = 899;
HANDLE hcurrent;

void addneighbour (int x , int y)
{
     int dx , dy , ux , uy;
     if (x > 0) dx = x - 1; else dx = 233; 
     if (y > 0) dy = y - 1; else dy = 233;
     if (x < 233) ux = x + 1; else ux = 0; 
     if (y < 233) uy = y + 1; else uy = 0; 
     neighbour[dx][dy]++;
     neighbour[dx][y]++;
     neighbour[dx][uy]++;
     neighbour[x][dy]++;
     neighbour[x][uy]++;
     neighbour[ux][dy]++;
     neighbour[ux][y]++;
     neighbour[ux][uy]++;
}

void removeneighbour (int x , int y)
{
     int dx , dy , ux , uy;
     if (x > 0) dx = x - 1; else dx = 233; 
     if (y > 0) dy = y - 1; else dy = 233;
     if (x < 233) ux = x + 1; else ux = 0; 
     if (y < 233) uy = y + 1; else uy = 0; 
     neighbour[dx][dy]--;
     neighbour[dx][y]--;
     neighbour[dx][uy]--;
     neighbour[x][dy]--;
     neighbour[x][uy]--;
     neighbour[ux][dy]--;
     neighbour[ux][y]--;
     neighbour[ux][uy]--;
}

DWORD WINAPI start (void *hwnd)
{
     int r , c , i;
     terminateexecuted = false;
     for(;;)
     {
            threadsafe = terminatethread = false;
            cachesize = 0;
            for (r = 0 ; r < 234 ; r++) for (c = 0 ; c < 234 ; c++)
            {
                if (cell[r][c] == 1 && neighbour[r][c] < 2) { cell[r][c] = 0; cacher[cachesize] = r; cachec[cachesize] = c; cache[cachesize++] = -1; }
                else if (cell[r][c] == 1 && neighbour[r][c] > 3) { cell[r][c] = 0; cacher[cachesize] = r; cachec[cachesize] = c; cache[cachesize++] = -1; }
                else if (cell[r][c] == 0 && neighbour[r][c] == 3) { cell[r][c] = 1; cacher[cachesize] = r; cachec[cachesize] = c; cache[cachesize++] = 1; }
            }
            for (i = 0 ; i < cachesize ; i++)
            {
                if (cache[i] == 1) addneighbour (cacher[i] , cachec[i]);
                if (cache[i] == -1) removeneighbour (cacher[i] , cachec[i]);
            }
            InvalidateRgn ((HWND)hwnd , NULL , false);
            UpdateWindow ((HWND)hwnd);
            while (!threadsafe) ;
            generation++;
            Sleep (1000 - speed);
            if (terminateexecuted)
            {
                                  terminatethread = true;
                                  Sleep (INFINITE);  
            }
     }
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Conway",       /* Title Text */
           WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, /* default window */
           30,       /* Windows decides the position */
           30,       /* where the window ends up on the screen */
           902,                 /* The programs width */
           702,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);
    
    hcurrent = GetCurrentThread ();   
    
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int x , y , cx , cy;
    static HANDLE hthread;
    static DWORD threadID;
        
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            MessageBox (NULL , "Press left mouse button to place a cell\nPress right mouse button to kill a cell\nPress F1 to start/stop\nPress F2 to decrease speed\nPress F3 to increase speed\nPress F4 to new cycle" , "Information" , MB_OK | MB_ICONINFORMATION); 
            break; 
        
        case WM_LBUTTONDOWN:
        {
            x = LOWORD (lParam);
            y = HIWORD (lParam);
            cx = x / 3;
            cy = y / 3;
            if (cell[cx][cy] == 0)
            {
                             cell[cx][cy] = 1;
                             addneighbour (cx , cy);
                             HDC hdc = GetDC (hwnd);
                             SelectObject (hdc , GetStockObject (BLACK_PEN));
                             SelectObject (hdc , GetStockObject (BLACK_BRUSH));
                             Rectangle (hdc , 3 * cx , 3 * cy , 3 * cx + 3 , 3 * cy + 3);
                             ReleaseDC (hwnd , hdc); 
            }
            break; 
        }
        
        case WM_RBUTTONDOWN:
        {
            x = LOWORD (lParam);
            y = HIWORD (lParam);
            cx = x / 3;
            cy = y / 3;
            if (cell[cx][cy] == 1)
            {
                             cell[cx][cy] = 0;
                             removeneighbour (cx , cy);
                             HDC hdc = GetDC (hwnd);
                             SelectObject (hdc , GetStockObject (WHITE_PEN));
                             SelectObject (hdc , GetStockObject (WHITE_BRUSH));
                             Rectangle (hdc , 3 * cx , 3 * cy , 3 * cx + 3 , 3 * cy + 3);
                             ReleaseDC (hwnd , hdc);
            }
            break; 
        }
        
        case WM_MOUSEMOVE:
            if (wParam & MK_LBUTTON) SendMessage (hwnd , WM_LBUTTONDOWN , 0 , lParam);
            if (wParam & MK_RBUTTON) SendMessage (hwnd , WM_RBUTTONDOWN , 0 , lParam);
            break; 
        
        case WM_KEYDOWN: 
            if (wParam == VK_F1)
                if (!started)
                {
                             hthread = CreateThread (NULL , 0 , start , hwnd , 0 , &threadID);
                             started = true;
                }
                else
                {
                    terminateexecuted = true;
                    while (!threadsafe) PeekMessage (&messages , NULL , 0 , 0 , QS_PAINT << 16) ;
                    while (!terminatethread) ;
                    TerminateThread (hthread , 0);
                    started = false;
                }
            if (wParam == VK_F2) if (speed > 109) speed -= 10;
            if (wParam == VK_F3) if (speed < 999) speed += 10;
            if (wParam == VK_F4) if (!started)
            {
                       int r , c;
                       generation = 0;
                       HDC hdc = GetDC (hwnd);
                       SelectObject (hdc , GetStockObject (WHITE_PEN));
                       SelectObject (hdc , GetStockObject (WHITE_BRUSH));
                       for (r = 0 ; r < 234 ; r++) for (c = 0 ; c < 234 ; c++)
                       {
                           cell[r][c] = neighbour[r][c] = 0;
                           Rectangle (hdc , 3 * r , 3 * c , 3 * r + 3 , 3 * c + 3);
                       }
                       ReleaseDC (hwnd , hdc);
            }
            break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint (hwnd , &ps);
            SelectObject (hdc , GetStockObject (BLACK_PEN));
            Rectangle (hdc , 703 , 0 , 704 , 702);
            char s[32];
            sprintf (s , "Generation: %d" , generation);
            TextOut (hdc , 713 , 10 , s , strlen (s));
            sprintf (s , "Speed: %d" , speed);
            TextOut (hdc , 713 , 30 , s , strlen (s)); 
            int i;
            for (i = 0 ; i < cachesize ; i++)
            {
                if (cache[i] == 1) { SelectObject (hdc , GetStockObject (BLACK_PEN)); SelectObject (hdc , GetStockObject (BLACK_BRUSH)); }
                if (cache[i] == -1) { SelectObject (hdc , GetStockObject (WHITE_PEN)); SelectObject (hdc , GetStockObject (WHITE_BRUSH)); }
                Rectangle (hdc , 3 * cacher[i] , 3 * cachec[i] , 3 * cacher[i] + 3 , 3 * cachec[i] + 3);
            }
            EndPaint(hwnd , &ps);
            threadsafe = true;
            break;    
        }
        
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

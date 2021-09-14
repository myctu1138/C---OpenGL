#include "framework.h"
#include "WindowsProject7.h"
#include "resource.h"
#include <stdlib.h>
#include <stdio.h>
#include "commdlg.h"
#include <Math.h>
#include "gl/gl.h"
#include "gl/glu.h"
#include "glut.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
GLfloat ambient_light_pyramid[] = { 0.0, 1.0, 0.0, 0.2 };
GLfloat ambient_light_ball_circle[] = { 0.8, 0.8, 0.8, 0 };
GLfloat diffuse_light[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse_lightTri[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light_position[] = { 0.8, 0.0, 8.0,1.0 };
GLfloat emission[] = { 0.8, 0.8, 0.0, 1.0 };
GLfloat emission_none[] = { 0.0, 0.0, 0.0, 1.0 };

#define imageWidth  64
#define imageHeight 64

bool gdglu = false;

HBITMAP hbmHBITMAP1 = NULL;
LPBITMAPINFOHEADER m_lpBMIH1 = NULL;
BYTE* image1;

OPENFILENAME ofn;
char FileName[256];
char* imname;
char path[128];
wchar_t szFilter[] = _TEXT("All Files(*.*)\0*.*\0\0");

Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

long Datasize(LPBITMAPINFOHEADER m_lpBMIH)
{
	long sizeImage;
	int p, dump; p = 4; dump = 0;
	if (m_lpBMIH->biBitCount == 8) p = 1;
	if (m_lpBMIH->biBitCount == 24) p = 3;
	dump = (m_lpBMIH->biWidth * p) % 4;
	if (dump != 0) dump = 4 - dump;
	sizeImage = m_lpBMIH->biHeight * (m_lpBMIH->biWidth + dump);
	return sizeImage;
}

HBITMAP LoadFileImage(wchar_t* FileName);
void getDataHBITMAP(HBITMAP hbmHBITMAP);

BOOL DisplayBmpJPG(HWND hWnd, HBITMAP hbmHBITMAP);

GLubyte image[3 * imageWidth * imageHeight];

float r = 0;
float r1 = 360;
float w = -15;
float z = -15;
int i = 0;
float movingOne = -10;
HGLRC hRC = NULL;
HDC hdc;
HWND hWnd;
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void SetDCPixelFormat(HDC hDC){
	int nPixelFormat;
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,

		0, 0,
		0, 0, 0, 0, 0,
		16,
		0,
		0,
		0,
		0,
		0, 0, 0
	};
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void DefineView(int width, int height);
void RenderRecedingTriangle();
void RenderRotatingPyramid();
void RenderBallMoveThroughCircle();
void RenderBallMoveThroughFiveCircles();
void RenderSolarSystem();
void RenderMexicanCap();
void Lighting(void);
void RenderMexicanCapWithTexture();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDOWSPROJECT7, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT7));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT7));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_WINDOWSPROJECT7);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{

	case WM_CREATE:
		hdc = GetDC(hWnd);
			SetDCPixelFormat(hdc);
		hRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hRC);

		break;

		// Window is being destroyed, clean up

	case WM_DESTROY:
		hdc = GetDC(hWnd);
		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(hRC);

		if (m_lpBMIH1 != NULL) delete[] m_lpBMIH1;

		PostQuitMessage(0);
		break;
	case WM_SIZE:


	{ int height = HIWORD(lParam);
	int width = LOWORD(lParam);
	DefineView(width, height);
	}
	break;


	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_LOADANIMAGE:
		{
			wchar_t defex[] = __TEXT("*.*");
			*FileName = 0;
			RtlZeroMemory(&ofn, sizeof ofn);
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			ofn.hInstance = hInst;
			ofn.lpstrFile = (LPWSTR)FileName;
			ofn.nMaxFile = sizeof FileName;
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
			ofn.lpstrFilter = (LPWSTR)szFilter;
			ofn.lpstrDefExt = (LPWSTR)defex;
			ofn.lpstrCustomFilter = (LPWSTR)szFilter;
			ofn.lpstrInitialDir = (LPCWSTR)path;

			if (GetOpenFileName(&ofn) != 0)
			{
				size_t origsize = wcslen((LPWSTR)FileName) + 1;
				size_t convertedChars = 0;
				char strConcat[] = " (char *)";
				size_t strConcatsize = (strlen(strConcat) + 1) * 2;
				const size_t newsize = origsize * 2;
				imname = new char[newsize + strConcatsize];
				wcstombs_s(&convertedChars, imname, newsize, (LPWSTR)FileName, _TRUNCATE);

				GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
				hbmHBITMAP1 = LoadFileImage((LPWSTR)FileName);
				getDataHBITMAP(hbmHBITMAP1);
				Gdiplus::GdiplusShutdown(gdiplusToken);
				delete[] imname;
			}
		}
		break;
		
		case IDM_TEXTUREONBEZIER:
			gdglu = true;
			break;
			
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		//Всяка една от функциите извиква различен графичен рендер
		// 
		//RenderRecedingTriangle();
		//RenderRotatingPyramid();
		//RenderBallMoveThroughCircle();
		//RenderBallMoveThroughFiveCircles();
		RenderSolarSystem();
		//RenderMexicanCap();


		if (gdglu == false) {
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			DisplayBmpJPG(hWnd, hbmHBITMAP1);
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
		else
			RenderMexicanCapWithTexture();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


GLfloat diffuse_light2[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position2[] = { 7, 3, 0, 1.0 };
GLfloat light_direction2[] = { 0, 2, 0 };

GLfloat xRotated = 43, yRotated = 50, zRotated = 30;
GLdouble radius = 1;


void RenderRecedingTriangle() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_lightTri);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glEnable(GL_LIGHT1);

	r += 0.5; if (r >= 360.0) r = 0.0;
	gluLookAt(7, -1, 7, 0, 0, 0, 0.0, 1.0, 0.0);

	glRotatef(r, 1, 1, 0);

	glBegin(GL_TRIANGLES);

	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(-2.0, -2.0, 0.0);
	glVertex3f(2.0, -2.0, 0.0);
	glVertex3f(0.0, 2.0, 0.0);

	glEnd();

	glPopMatrix();
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);

}

void RenderRotatingPyramid()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 70.0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction2);
	glEnable(GL_LIGHT1);

	r += 0.5; if (r >= 360.0) r = 0.0;
	gluLookAt(7, 3, 0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glPushMatrix();

	glRotatef(r, 0, 1, 0);

	glBegin(GL_QUADS);
	glColor3f(0.1, 0.1, 0.1);
	glVertex3f(-2, 0, -2);
	glVertex3f(2, 0, -2);
	glVertex3f(2, 0, 2);
	glVertex3f(-2, 0, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(-2, 0, -2);
	glVertex3f(2, 0, -2);
	glVertex3f(0, 3, 0);

	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(2, 0, -2);
	glVertex3f(2, 0, 2);
	glVertex3f(0, 3, 0);

	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(2, 0, 2);
	glVertex3f(-2, 0, 2);
	glVertex3f(0, 3, 0);

	glColor3f(1, 1, 1);
	glVertex3f(-2, 0, 2);
	glVertex3f(-2, 0, -2);
	glVertex3f(0, 3, 0);
	glEnd();

	glPopMatrix();
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}



void RenderBallMoveThroughCircle()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 70.0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction2);
	glEnable(GL_LIGHT1);

	glColor3f(1, 0, 0);

	if ((i == 0)) gluLookAt(-10, 0, -8, 0, 0, 0, 0.0, 1.0, 0.0);
	else if ((i == 1)) gluLookAt(0, 0, -10, 0, 0, 0, 0.0, 1.0, 0.0);
	else if ((i == 2)) gluLookAt(0, 0, 10, 0, 0, 0, 0.0, 1.0, 0.0);
	else if ((i == 3)) gluLookAt(-1, 10, 0, 0, 0, 0, 0.0, 1.0, 0.0);
	else if ((i == 4)) gluLookAt(-2, -10, 0, 0, 0, 0, 0.0, 1.0, 0.0);

	w += 0.15;
	if (w >= 0 && w < 10) glColor3f(1, 0.6, 0);
	else if (w >= 10)
	{
		glColor3f(1, 0, 0);
		w = -15;
		i++;
		if (i == 5) i = 0;
	}

	glPushMatrix();
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, w);
	glScalef(1.0, 1.0, 1.0);

	glColor3f(0, 0.8, 0);
	glutSolidSphere(radius, 20, 20);
		glPopMatrix();

	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

void RenderBallMoveThroughFiveCircles()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 70.0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction2);
	glEnable(GL_LIGHT1);

	gluLookAt(-12, 1, -10, 0, 0, 0, 0.0, 1.0, 0.0);

	r += 0.8; if (r >= 360.0) r = 0.0;
	r1 -= 0.8; if (r <= 0.0) r1 = 360.0;
	glPushMatrix();

	glColor3f(1.0, 0.0, 0.0);
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 5.0);
	glRotatef(r, 0, 1, 0);

	glColor3f(0.0, 1.0, 0.0);
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 10.0);

	glColor3f(0.0, 0.0, 1.0);
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 15.0);
	glRotatef(r1, 0, 1, 0);

	glColor3f(1.0, 1.0, 0.0);
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 20.0);


	glColor3f(0.0, 1.0, 1.0);
	glutSolidTorus(0.3, 3, 15, 40);
	glPopMatrix();


	glPushMatrix();

	glColor3f(0.8, 1.2, 0.2);
	z += 0.15; if (z >= 35) z = -15;

	glTranslatef(0.0, 0.0, z);
	glScalef(1.0, 1.0, 1.0);


	glutSolidSphere(radius, 20, 20);
	glPopMatrix();

	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}


GLfloat diffuse_light3[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position3[] = { 7, 3, 0, 1.0 };
GLfloat light_direction3[] = { 0, 2, 0 };

float sun;
float planet1;
float planet2;
float planet3;
float moon;
float planet4;
float planet5;
float planet6;
float planet7;
float planet8;
float planet9;

void RenderSolarSystem()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 70.0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_light2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_direction2);
	glEnable(GL_LIGHT1);

	gluLookAt(0, 80, -80, 0, 0, 0, 0.0, 1.0, 0.0);


	planet1 += 1.48;
	planet2 += 1.32;
	planet3 += 1.12;
	moon += 1.7;
	planet4 += 1;
	planet5 += 0.8;
	planet6 += 0.6;
	planet7 += 0.4;
	planet8 += 0.2;
	planet9 += 0.13;

	glPushMatrix();

	glColor3f(1.0, 1.0, 0.0);
	glutSolidSphere(15, 30, 30);

	glPopMatrix();

	glPushMatrix();

	glRotatef(planet1, 0, 1, 0);
	glTranslatef(18.0, 0.0, 0.0);
	glColor3f(0.75, 0.75, 0.75);
	glutSolidSphere(0.5, 30, 30);


	glPopMatrix();

	glPushMatrix();
	glRotatef(planet2, 0, 1, 0);
	glTranslatef(20.1, 0.0, 0.0);
	glColor3f(1.0, 0.35, 0.0);
	glutSolidSphere(1.1, 30, 30);

	glPopMatrix();

	glPushMatrix();
	glRotatef(planet3, 0, 1, 0);
	glTranslatef(24.0, 0.0, 0.0);
	glColor3f(0.196078, 0.6, 0.8);
	glutSolidSphere(1.27, 30, 30);

	glRotatef(moon, 0, 1, 0);
	glTranslatef(1.7, 0.0, 0.0);
	glColor3f(0.6, 0.6, 0.6);
	glutSolidSphere(0.27, 30, 30);

	glPopMatrix();

	glPushMatrix();
	glRotatef(planet4, 0, 1, 0);
	glTranslatef(28.0, 0.0, 0.0);
	glColor3f(1.0, 0.5, 0.0);
	glutSolidSphere(0.67, 30, 30);

	glPopMatrix();


	glPushMatrix();

	glRotatef(planet5, 0, 1, 0);
	glTranslatef(35.0, 0.0, 0.0);
	glColor3f(0.5, 0.3, 0.0);
	glutSolidSphere(5, 30, 30);

	glPopMatrix();

	glPushMatrix();

	glRotatef(planet6, 0, 1, 0);
	glTranslatef(47.0, 0.0, 0.0);
	glColor3f(0.8, 0.7, 0.4);
	glutSolidSphere(4, 30, 30);


	glRotatef(xRotated, 0.3, 0.0, 0.0);
	glRotatef(yRotated, 0.0, 1.0, 0.0);
	glColor3f(0.8, 0.8, 0.8);
	glutSolidTorus(0.5, 5, 25, 40);

	glPopMatrix();

	glPushMatrix();

	glRotatef(planet7, 0, 1, 0);
	glTranslatef(57.0, 0.0, 0.0);
	glColor3f(0.137255, 0.419608, 0.556863);
	glutSolidSphere(2, 30, 30);

	glPopMatrix();

	glPushMatrix();

	glRotatef(planet8, 0, 1, 0);
	glTranslatef(64.0, 0.0, 0.0);
	glColor3f(0.137255, 0.556863, 0.419608);
	glutSolidSphere(1.8, 30, 30);

	glPopMatrix();

	glPushMatrix();

	glRotatef(planet9, 0, 1, 0);
	glTranslatef(68.0, 0.0, 0.0);
	glColor3f(0.6, 0.6, 0.6);
	glutSolidSphere(0.2, 30, 30);

	glPopMatrix();

	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

int a = 1;
bool showPoints = false;
void RenderMexicanCap()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 120.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { 5.0, 10.0, -2.0, 1.0 };
	GLfloat position1[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, position1);
	glMaterialf(GL_FRONT_AND_BACK, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt(50.0, 8.0, -6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	GLfloat ctrlpoints1[9][9][3] = {
	{ { -3.5, -2.5, 1 }, { -3, -3, 1 }, { -2, -4, 1 }, { -1, -4, -1 }, { 0, -4.5, -1 }, { 1, -4, -1 }, { 2, -4, 1 }, { 3, -3, 1 }, { 3.5, -2.5, -1 } },
	{ { -3, -3, 1 }, { -3, -3, -1 }, { -2, -3, -1 }, { -1, -3, -3 }, { 0, -3, -3 }, { 1, -3, -3 }, { 2, -3, -1 }, { 3, -3, -1 }, { 3, -3, 1 } },
	{ { -4, -2, 1 }, { -3, -2, -1 }, { -2, -2, -3 }, { -1, -2, -3 }, { 0, -2, -1 }, { 1, -2, -3 }, { 2, -2, -3 }, { 3, -2, -1 }, { 4, -2, -1 } },
	{ { -4, -1, -1 }, { -3, -1, -3 }, { -2, -1, -3 }, { -1, -1, 6 }, { 0, -1, 10 }, { 1, -1, 6 }, { 2, -1, -3 }, { 3, -1, -3 }, { 4, -1, -1 } },
	{ { -4.5, 0, -1 }, { -3, 0, -3 }, { -2, 0, -3 }, { -1, 0, 10 }, { 0, 0, 13 }, { 1, 0, 10 }, { 2, 0, -3 }, { 3, 0, -3 }, { 4.5, 0, -1 } },
	{ { -4, 1, -1 }, { -3, 1, -3 }, { -2, 1, -3 }, { -1, 1, 6 }, { 0, 1, 10 }, { 1, 1, 6 }, { 2, 1, -3 }, { 3, 1, -3 }, { 4, 1, -1 } },
	{ { -4, 2, 1 }, { -3, 2, -1 }, { -2, 2, -3 }, { -1, 2, -3 }, { 0, 2, -1 }, { 1, 2, -3 }, { 2, 2, -3 }, { 3, 2, -1 }, { 4, 2, 1 } },
	{ { -3, 3, 1 }, { -3, 3, -1 }, { -2, 3, -1 }, { -1, 3, -3 }, { 0, 3, -3 }, { 1, 3, -3 }, { 2, 3, -1 }, { 3, 3, -1 }, { 3, 3, 1 } },
	{ { -3.5, 2.5, 1 }, { -3, 3, 1 }, { -2, 4, 1 }, { -1, 4, -1 }, { 0, 4.5, -1 }, { 1, 4, -1 }, { 2, 4, 1 }, { 3, 3, 1 }, { 3.5, 2.5, 1 } }
	};
	glShadeModel(GL_FLAT);
	GLfloat knots[13] = { 0.0, 0.0, 0.0, 0.0, 0.2, 0.35, 0.62, 0.76, 0.87, 1.0, 1.0, 1.0, 1.0 };
	GLfloat edgePt[13][2] = {
		{ 0.2, 0.2 }, { 0.3, 0.12 }, { 0.6, 0.1 }, { 0.82, 0.17 }, { 0.90, 0.3 }, { 0.9, 0.56 }, { 0.83, 0.83 }, { 0.65, 0.9 }, { 0.3, 0.9 }, { 0.17, 0.82 }, { 0.10, 0.6 }, { 0.12, 0.28 }, { 0.2, 0.2 }
	};
	GLfloat curveKnots[17] = { 0.0, 0.0, 0.0, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.0, 1.0, 1.0 };
	GLfloat knotsx[12] = { 0.0, 0.0, 0.0, 0.0, 0.24, 0.58, 0.76, 0.87, 1.0, 1.0, 1.0, 1.0 };
	GLfloat knotsy[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	GLfloat edgePt1[12][2] = {
		{ 0.15, 0.19 }, { 0.5, 0.0 }, { 0.8, 0.0 }, { 0.82, 0.17 }, { 1.0, 0.5 }, { 1.0, 0.6 }, { 0.81, 0.81 }, { 0.8, 1.0 }, { 0.5, 1.0 }
	};

	r += 0.1; if (r > 360) r = 0.0;
	GLUnurbsObj* theNurb;
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 55.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	glRotatef(r, 1.0, 1.0, 1.0);
	glScalef(2.2, 2.2, 1);

	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 13, knots, 13, knots, 3, 9 * 3, &ctrlpoints1[0][0][0], 4, 4, GL_MAP2_VERTEX_3);
	gluEndSurface(theNurb);
	showPoints = true;
	if (showPoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 9; i++) {

			for (int j = 0; j < 9; j++) {
				glVertex3d(ctrlpoints1[i][j][0], ctrlpoints1[i][j][1], ctrlpoints1[i][j][2]);
			}

		}
		glEnd();
	}

	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}


GLfloat texpts[2][2][2] = { { { 0.0, 0.0 }, { 0.0, 1.0 } },
{ { 1.0, 0.0 }, { 1.0, 1.0 } } };


void RenderMexicanCapWithTexture()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 120.0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	GLfloat position[] = { 5.0, 10.0, -2.0, 1.0 };
	GLfloat position1[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, position1);
	glMaterialf(GL_FRONT_AND_BACK, GL_DIFFUSE, 30.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	gluLookAt(50.0, 8.0, -6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	GLfloat ctrlpoints1[9][9][3] =
	{ { { -3.5, -2.5, 1 }, { -3, -3, 1 }, { -2, -4, 1 }, { -1, -4, -1 }, { 0, -4.5, -1 }, { 1, -4, -1 }, { 2, -4, 1 }, { 3, -3, 1 }, { 3.5, -2.5, -1 } },
	{ { -3, -3, 1 }, { -3, -3, -1 }, { -2, -3, -1 }, { -1, -3, -3 }, { 0, -3, -3 }, { 1, -3, -3 }, { 2, -3, -1 }, { 3, -3, -1 }, { 3, -3, 1 } },
	{ { -4, -2, 1 }, { -3, -2, -1 }, { -2, -2, -3 }, { -1, -2, -3 }, { 0, -2, -1 }, { 1, -2, -3 }, { 2, -2, -3 }, { 3, -2, -1 }, { 4, -2, -1 } },
	{ { -4, -1, -1 }, { -3, -1, -3 }, { -2, -1, -3 }, { -1, -1, 6 }, { 0, -1, 10 }, { 1, -1, 6 }, { 2, -1, -3 }, { 3, -1, -3 }, { 4, -1, -1 } },
	{ { -4.5, 0, -1 }, { -3, 0, -3 }, { -2, 0, -3 }, { -1, 0, 10 }, { 0, 0, 13 }, { 1, 0, 10 }, { 2, 0, -3 }, { 3, 0, -3 }, { 4.5, 0, -1 } },
	{ { -4, 1, -1 }, { -3, 1, -3 }, { -2, 1, -3 }, { -1, 1, 6 }, { 0, 1, 10 }, { 1, 1, 6 }, { 2, 1, -3 }, { 3, 1, -3 }, { 4, 1, -1 } },
	{ { -4, 2, 1 }, { -3, 2, -1 }, { -2, 2, -3 }, { -1, 2, -3 }, { 0, 2, -1 }, { 1, 2, -3 }, { 2, 2, -3 }, { 3, 2, -1 }, { 4, 2, 1 } },
	{ { -3, 3, 1 }, { -3, 3, -1 }, { -2, 3, -1 }, { -1, 3, -3 }, { 0, 3, -3 }, { 1, 3, -3 }, { 2, 3, -1 }, { 3, 3, -1 }, { 3, 3, 1 } },
	{ { -3.5, 2.5, 1 }, { -3, 3, 1 }, { -2, 4, 1 }, { -1, 4, -1 }, { 0, 4.5, -1 }, { 1, 4, -1 }, { 2, 4, 1 }, { 3, 3, 1 }, { 3.5, 2.5, 1 } }
	};
	glShadeModel(GL_FLAT);
	GLfloat knots[13] = { 0.0, 0.0, 0.0, 0.0, 0.2, 0.35, 0.62, 0.76, 0.87, 1.0, 1.0, 1.0, 1.0 };
	GLfloat edgePt[13][2] = {
		{ 0.2, 0.2 }, { 0.3, 0.12 }, { 0.6, 0.1 }, { 0.82, 0.17 }, { 0.90, 0.3 }, { 0.9, 0.56 }, { 0.83, 0.83 }, { 0.65, 0.9 }, { 0.3, 0.9 }, { 0.17, 0.82 }, { 0.10, 0.6 }, { 0.12, 0.28 }, { 0.2, 0.2 }
	};
	GLfloat curveKnots[17] = { 0.0, 0.0, 0.0, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.0, 1.0, 1.0 };
	GLfloat knotsx[12] = { 0.0, 0.0, 0.0, 0.0, 0.24, 0.58, 0.76, 0.87, 1.0, 1.0, 1.0, 1.0 };
	GLfloat knotsy[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
	GLfloat edgePt1[12][2] = {
		{ 0.15, 0.19 }, { 0.5, 0.0 }, { 0.8, 0.0 }, { 0.82, 0.17 }, { 1.0, 0.5 }, { 1.0, 0.6 }, { 0.81, 0.81 }, { 0.8, 1.0 }, { 0.5, 1.0 }
	};

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints1[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int imageWidth1 = m_lpBMIH1->biWidth;
	int imageHeight1 = m_lpBMIH1->biHeight;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth1, imageHeight1, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);


	r += 0.25; if (r > 360) r = 0.0;
	GLUnurbsObj* theNurb;
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 55.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	glRotatef(r, 0.0, 0.4, 1.0);
	glScalef(2.2, 2.2, 1);

	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 13, knots, 13, knots, 3, 9 * 3, &ctrlpoints1[0][0][0], 4, 4, GL_MAP2_VERTEX_3);
	gluEndSurface(theNurb);
	showPoints = true;
	if (showPoints) {
		glPointSize(3.0);
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 9; i++) {

			for (int j = 0; j < 9; j++) {
				glVertex3d(ctrlpoints1[i][j][0], ctrlpoints1[i][j][1], ctrlpoints1[i][j][2]);
			}

		}
		glEnd();
	}

	glPopMatrix();
	gluDeleteNurbsRenderer(theNurb);
	SwapBuffers(hdc);
	ReleaseDC(hWnd, hdc);
}

void Lighting(void) {

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light_ball_circle);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_light_ball_circle);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_light);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void DefineView(int width, int height)
{
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Lighting();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}



BOOL DisplayBmpJPG(HWND hWnd, HBITMAP hbmHBITMAP)
{

	Gdiplus::Graphics g(::GetDC(hWnd));

	RECT rc;
	::GetClientRect(hWnd, &rc);
	
	Gdiplus::Bitmap* pb = Gdiplus::Bitmap::FromHBITMAP(hbmHBITMAP, NULL);
	g.DrawImage(pb, float(rc.left), float(rc.top), float(rc.right), float(rc.bottom));

	delete pb;
	return true;
}

void getDataHBITMAP(HBITMAP hbmHBITMAP)
{
	LPBYTE m_lpImage;
	DIBSECTION ds;

	::GetObject(hbmHBITMAP, sizeof(ds), &ds);
	if (m_lpBMIH1 != NULL) delete[] m_lpBMIH1;
	m_lpBMIH1 = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	memcpy(m_lpBMIH1, &ds.dsBmih, sizeof(BITMAPINFOHEADER));
	if (m_lpBMIH1->biSizeImage == 0)
		m_lpBMIH1->biSizeImage = Datasize(m_lpBMIH1);

	m_lpImage = (LPBYTE)ds.dsBm.bmBits;
	image1 = m_lpImage;
}

HBITMAP LoadFileImage(wchar_t* FileName)
{
	Gdiplus::Bitmap* pbmp = Gdiplus::Bitmap::FromFile(FileName);
	HBITMAP JPGImage;
	if (pbmp == NULL) JPGImage = NULL;
	else
		if (Gdiplus::Status::Ok != pbmp->GetHBITMAP(Gdiplus::Color::Black, &JPGImage))
			JPGImage = NULL;

	delete pbmp;
	return JPGImage;
}
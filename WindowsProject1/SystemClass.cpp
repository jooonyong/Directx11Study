#include "stdafx.h"
#include "InputClass.h"
#include "GraphicsClass.h"
//#include "SoundClass.h"
//#include "FpsClass.h"
//#include "CpuClass.h"
#include "Timer.h"
#include "PositionClass.h"
#include "SystemClass.h"

SystemClass::SystemClass()
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	//윈도우 생성 초기화
	InitializeWindows(screenWidth, screenHeight);

	//m_Input 객체 생성. 이클래스는 추후 사용자의 키보드 입력에 사용
	m_Input = new InputClass{};
	if (!m_Input)
		return false;

	//m_Input 객체 초기화
	if (!m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	m_Graphics = new GraphicsClass{};
	if (!m_Graphics)
		return false;

	if (!m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		return false;
	}

	/*m_Fps = new FpsClass{};
	if (!m_Fps)
	{
		return false;
	}
	m_Fps->Initialize();

	m_Cpu = new CpuClass{};
	if (!m_Cpu)
	{
		return false;
	}
	m_Cpu->Initialize();*/

	m_Timer = new TimerClass{};
	if (!m_Timer)
	{
		return false;
	}

	if (!m_Timer->Initialize())
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object", L"Error", MB_OK);
		return false;
	}

	m_Position = new PositionClass{};
	if (!m_Position)
	{
		return false;
	}

	return true;
}

void SystemClass::ShutDown()
{
	if (m_Position)
	{
		delete m_Position;
		m_Position = nullptr;
	}
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}
	/*if (m_Cpu)
	{
		m_Cpu->ShutDown();
		delete m_Cpu;
		m_Cpu = nullptr;
	}
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = nullptr;
	}*/

	//m_Graphics 객체 반환
	if (m_Graphics)
	{
		m_Graphics->ShutDown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}
	if (m_Input)
	{
		m_Input->ShutDown();
		delete m_Input;
		m_Input = nullptr;
	}

	ShutDownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}
		if (m_Input->IsEscapePressed() == true)
		{
			break;
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool SystemClass::Frame()
{
	m_Timer->Frame();

	if (!m_Input->Frame())
		return false;
	
	/*m_Position->SetFrameTime(m_Timer->GetTime());

	bool keyDown = m_Input->IsLeftArrowPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightArrowPressed();
	m_Position->Turnright(keyDown);

	float rotationY = 0.0f;
	m_Position->GetRotation(rotationY);*/

	if (!m_Graphics->Frame())
	{
		return false;
	}

	//return m_Graphics->Render();
}
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//외부 포인터를 이 객체로 지정
	ApplicationHandle = this;

	//이프로그램의 인스턴스
	m_hinstance = GetModuleHandle(NULL);
	//프로그램 이름 지정
	m_applicationName = L"Dx11";

	//window class 설정
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//window class 등록
	RegisterClassEx(&wc);

	//모니 화면 해상도
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		//모니터 화면 해상도를 데스크톱 해상도로 지정하고 색상을 32bit로 지정
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//풀스크린으로 디스플레이 설정 변경
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		//윈도우 모드 경우 800*600 으로 지정
		screenWidth = 800;
		screenHeight = 600;

		//윈도우 창을 가로, 세로 정가운데 오게 함
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//윈도우 생성하고 핸들을 구함
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//윈도우를 화면에 표시하고 포커스 지정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutDownWindows()
{
	//풀 스크린 모드였으면 디스플레이 설정 초기화
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	//윈도우 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;
	
	//프로그램 인스턴스 제거
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//외부 포인터 차조 초기화
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}

}
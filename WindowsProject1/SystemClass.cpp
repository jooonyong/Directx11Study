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

	//������ ���� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	//m_Input ��ü ����. ��Ŭ������ ���� ������� Ű���� �Է¿� ���
	m_Input = new InputClass{};
	if (!m_Input)
		return false;

	//m_Input ��ü �ʱ�ȭ
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

	//m_Graphics ��ü ��ȯ
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
	//�ܺ� �����͸� �� ��ü�� ����
	ApplicationHandle = this;

	//�����α׷��� �ν��Ͻ�
	m_hinstance = GetModuleHandle(NULL);
	//���α׷� �̸� ����
	m_applicationName = L"Dx11";

	//window class ����
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

	//window class ���
	RegisterClassEx(&wc);

	//��� ȭ�� �ػ�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		//����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� ����
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ��ũ������ ���÷��� ���� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		//������ ��� ��� 800*600 ���� ����
		screenWidth = 800;
		screenHeight = 600;

		//������ â�� ����, ���� ����� ���� ��
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//������ �����ϰ� �ڵ��� ����
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ�� ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutDownWindows()
{
	//Ǯ ��ũ�� ��忴���� ���÷��� ���� �ʱ�ȭ
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	//������ ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;
	
	//���α׷� �ν��Ͻ� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//�ܺ� ������ ���� �ʱ�ȭ
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
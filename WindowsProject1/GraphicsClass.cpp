#include "stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "RenderTextureClass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "RefractionShaderClass.h"
#include "WaterShaderClass.h"
#include "FadeShaderClass.h"
#include "GlassShaderClass.h"
#include "FireShaderClass.h"
#include "DepthShaderClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//direct3d °´Ã¼ »ı¼º
	m_Direct3D = new D3DClass{};
	if (!m_Direct3D)
		return false;

	//direct3d °´Ã¼ ÃÊ±âÈ­
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd,
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not Initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	//Ä«¸Ş¶ó °´Ã¼ »ı¼º
	m_Camera = new CameraClass{};
	if (!m_Camera)
	{
		return false;
	}

	m_Model = new ModelClass{};
	if (!m_Model)
	{
		return false;
	}
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), "./floor.txt", L"fire01.dds", L"noise01.dds", L"./alpha01.dds"))
	{
		MessageBox(hwnd, L"Could not Initialize model object", L"Error", MB_OK);
		return false;
	}

	m_DepthShader = new DepthShaderClass{};
	if (!m_DepthShader)
	{
		return false;
	}
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not Initialize shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::ShutDown()
{
	if (m_DepthShader)
	{
		m_DepthShader->ShutDown();
		delete m_DepthShader;
		m_DepthShader = nullptr;
	}
	if (m_Model)
	{
		m_Model->ShutDown();
		delete m_Model;
		m_Model = nullptr;
	}
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	//direct3d °´Ã¼ ¹İÈ¯
	if (m_Direct3D)
	{
		m_Direct3D->ShutDown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	/*m_Camera->SetPosition( 0.0f, 0.0f, -5.0f);*/

	return Render();
}


bool GraphicsClass::Render()
{
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	if (!m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	m_Direct3D->EndScene();
	
	return true;
}

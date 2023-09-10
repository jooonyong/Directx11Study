#include "stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	//���� ����ȭ ���� ����
	m_vsync_enabled = vsync;

	//directx �׷��� �������̽� ���丮 ����
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)&factory)))
	{
		return false;
	}

	//���丮 ��ü�� ����Ͽ� ù���� �׷��� ī�� �������̽� ����� ����
	IDXGIAdapter* adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
	{
		return false;
	}

	//��¿� ���� ù��° ����� ����
	IDXGIOutput* adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
	{
		return false;
	}

	//��¿� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� ���� ������
	unsigned int numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, NULL)))
	{
		return false;
	}

	//������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ ����
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//���÷��� ��忡 ���� ����Ʈ ä��
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList)))
	{
		return false;
	}

	//��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã��
	//������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� �� ����
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//����ī�� ����ü
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
	{
		return false;
	}

	//����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ ����
	m_videoCardMemmory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	
	//����ī�� �̸� ����
	size_t stringLength = 0;
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0)
	{
		return false;
	}
	
	//���÷��� ��� ����Ʈ ����
	delete[] displayModeList;
	displayModeList = 0;

	//��� ����� ����
	adapterOutput->Release();
	adapterOutput = nullptr;

	//����� ����
	adapter->Release();
	adapter = nullptr;
	
	//���丮 ��ü ����
	factory->Release();
	factory = nullptr;

	//����ü�� ����ü �ʱ�ȭ
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//����� 1���� ���
	swapChainDesc.BufferCount = 1;

	//����� �ʺ� ���� ����
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//32��Ʈ �����̽� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//������� ���ΰ�ħ ���� ����
	if (m_vsync_enabled)//���� ����ȭ ����
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//������� ���뵵 ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�������� ���� ������ �ڵ� ����
	swapChainDesc.OutputWindow = hwnd;

	//��Ƽ���ø� ��
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//â��� Ǯ��ũ�� ��� ����
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	//��ĵ ���� ���� �� ũ�⸦ �������� �������� ����
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//��µ� ���� ����۸� ��쵵�� ����
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�߰� �ɼ� �÷��� ������� ����
	swapChainDesc.Flags = 0;

	//��ó ���� directx11�� ����
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//����ü��, direct3d ��ġ �� ���ؽ�Ʈ�� ����
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext)))
	{
		return false;
	}

	//����� ������ ����
	ID3D11Texture2D* backBufferPtr = nullptr;
	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
	{
		return false;
	}

	//����� �����ͷ� ���� Ÿ�� �� ����
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
	{
		return false;
	}

	//����� ������ ����
	backBufferPtr->Release();
	backBufferPtr = 0;

	//���� ���� ����ü �ʱ�ȭ
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//���̹��� ����ü�� ���� ���� �ؽ��� ����
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
	{
		return false;
	}

	//���ٽ� ���� ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//�ȼ� ������ ���ٽ� ����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�ȼ� �޸��� ���ٽ� ����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//���� ���ٽ� ���� ����
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
	{
		return false;
	}
	//���� ���ٽ� ���� ����
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//���� ���ٽ� �� ����ü �ʱ�ȭ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//���� ���ٽ� �� ����ü ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//���� ���ٽ� �� ����
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc,
		&m_depthStencilView)))
	{
		return false;
	}

	//������ ��� ��� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//�׷����� ������� ����� ������ ������ ����ü ����
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//�����Ͷ����� ���� ����
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
	{
		return false;
	}

	//�����Ͷ����� ���� ����
	m_deviceContext->RSSetState(m_rasterState);

	//�������� ���� ����Ʈ ����
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//����Ʈ ����
	m_deviceContext->RSSetViewports(1, &viewport);

	//���� ��� ����
	float fieldOfView = XM_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	//3d �������� ���� ��������� ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	//worldmatrix�� ������ķ� �ʱ�ȭ
	m_worldMatrix = XMMatrixIdentity();
	//2d �������� ���� ���� ������� ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	
	//2d �������� ���� z���۸� ���� �ι�° ���� ���ٽ� ���� ����.
	//������ �������� DepthEnable�� false�� �����ϸ� �ٸ� ��� �Ű������� �ٸ� ���� ���ٽ� ���¿� ����
	
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))
	{
		return false;
	}

	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	if (FAILED(m_device->CreateBlendState(&blendStateDesc, &m_alphaEnableBlendingState)))
	{
		return false;
	}

	blendStateDesc.RenderTarget[0].BlendEnable = FALSE;

	if (FAILED(m_device->CreateBlendState(&blendStateDesc, &m_alphaDisableBlendingState)))
	{
		return false;
	}
	return true;
}

void D3DClass::ShutDown()
{
	//���� �� ������ ���� �������� ������ ���� ü���� �����Ҷ� ���ܰ� �߻���
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}
	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = nullptr;
	}
	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = nullptr;
	}
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	//���۸� ���� �� ����
	float color[4] = { red, green, blue, alpha };
	//����۸� ����
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	//���� ���� ����
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	//�������� �Ϸ�Ǿ����Ƿ� ȭ�鿡 ����� ǥ��
	if (m_vsync_enabled)
	{
		//ȭ�� ���� ��ħ ���� ����
		m_swapChain->Present(1, 0);
	}
	else {
		//������ ������ ���
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemmory;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffAlphBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	return m_depthStencilView;
}

void D3DClass::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

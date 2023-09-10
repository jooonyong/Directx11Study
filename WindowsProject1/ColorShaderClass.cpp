#include "stdafx.h"
#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{

}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//���ؽ� ���̴�, �ȼ� ���̴� �ʱ�ȭ
	return InitializeShader(device, hwnd, L"./VertexShader.hlsl", L"./PixelShader.hlsl");
}

void ColorShaderClass::ShutDown()
{
	//���ؽ� �����̴� �ȼ� ���̴� ��ü ����
	ShutDownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	//�������� ����� �Ű����� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}
	//������ ���۸� ���̴��� ������
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	ID3D10Blob* errorMessage = nullptr;

	//���ؽ� ���̴� ������
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFileName, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		else 
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//�ȼ� ���̴� ������
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFileName, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &pixelShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//���۷κ��� vertexshader ����
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		NULL, &m_vertexShader)))
	{
		return false;
	}
	//���ۿ��� pixelshader ����
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		NULL, &m_pixelShader)))
	{
		return false;
	}

	//���ؽ� �Է� ���̾ƿ� ����ü ����
	//���̴��� VertexType ������ ��ġ�ؾ���.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//���̾ƿ� ��� ���� ������
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ� ����
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	//������� �ʴ� vertex ���� �ȼ� ���̴� ���� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffefType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutDownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));
	
	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader.", shaderFileName, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	MatrixBuffefType* dataPtr = (MatrixBuffefType*)mappedResource.pData;

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	unsigned bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//���ؽ� �Է� ���̾ƿ� ����
	deviceContext->IASetInputLayout(m_layout);

	//�ﰢ���� �׸� ���ؽ� ���̴��� �ȼ� ���̴� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ﰢ�� �׸�
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

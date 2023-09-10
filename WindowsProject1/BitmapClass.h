#pragma once

class BitmapClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass& other);
	~BitmapClass();

	bool Initialize(ID3D11Device* device, int, int, int, int);
	void ShutDown();
	bool Render(ID3D11DeviceContext* deviceContext, int, int);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutDownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int, int);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	//bool LoadTexture(ID3D11Device* device, WCHAR*);
	//void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_bitmapWidth = 0;
	int m_bitmapHeight = 0;
	int m_previousPosX = 0;
	int m_previousPosY = 0;
};
#include "stdafx.h"
#include "TextureClass.h"

TextureClass::TextureClass()
{
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	// 텍스처를 파일로부터 읽어온다
	if (FAILED(CreateDDSTextureFromFile(device, filename, nullptr, &m_texture)))
	{
		return false;
	}

	return true;
}

void TextureClass::ShutDown()
{
	//텍스처 뷰 리소스를 해제한다.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}
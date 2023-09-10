#include "stdafx.h"
#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1)
{
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
		return false;
	
	return true;
}

void TextureArrayClass::ShutDown()
{
	if (m_texture[0])
	{
		m_texture[0]->Release();
		m_texture[0] = nullptr;
	}
	if (m_texture[1])
	{
		m_texture[1]->Release();
		m_texture[1] = nullptr;
	}
	if (m_texture[2])
	{
		m_texture[2]->Release();
		m_texture[2] = nullptr;
	}
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_texture;
}

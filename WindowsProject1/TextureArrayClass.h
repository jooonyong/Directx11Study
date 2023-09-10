#pragma once

class TextureArrayClass
{
public:
	TextureArrayClass() = default;
	TextureArrayClass(const TextureArrayClass& other);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void ShutDown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* m_texture[3] = { nullptr, nullptr, nullptr };
};
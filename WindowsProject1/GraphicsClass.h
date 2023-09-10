#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;

class D3DClass;
class CameraClass;
class ModelClass;
class DepthShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& other);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void ShutDown();
	bool Frame();
	//bool Render();

private:
	/*bool RenderToTexture(float);
	bool RenderFadingScene();
	bool RenderNormalScene(float);*/
	//bool RenderToTexture(float);
	bool Render();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	DepthShaderClass* m_DepthShader = nullptr;
};
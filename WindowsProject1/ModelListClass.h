#pragma once

class ModelListClass
{
private:
	struct ModelInfoType
	{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelListClass() = default;
	ModelListClass(const ModelListClass& other);
	~ModelListClass();

	bool Initialize(int);
	void ShutDown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMFLOAT4&);

private:
	int m_modelCount = 0;
	ModelInfoType* m_ModelInfoList = nullptr;
};
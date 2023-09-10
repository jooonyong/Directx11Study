#pragma once

#pragma comment(lib, "pdh.lib")
#include <Pdh.h>

class CpuClass
{
public:
	CpuClass() = default;
	CpuClass(const CpuClass& other);
	~CpuClass();

	void Initialize();
	void ShutDown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu = true;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime = 0;
	long m_cpuUsage = 0;
};
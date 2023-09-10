#pragma once

class PositionClass
{
public:
	PositionClass() = default;
	PositionClass(const PositionClass& other);
	~PositionClass();

	void SetFrameTime(float);
	void GetRotation(float&);

	void TurnLeft(bool);
	void Turnright(bool);

private:
	float m_frameTime = 0.0f;
	float m_rotationY = 0.0f;
	float m_leftTurnSpeed = 0.0f;
	float m_rightTurnSpeed = 0.0f;
};
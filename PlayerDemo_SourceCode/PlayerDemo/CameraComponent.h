#pragma once

#include <GameObject.h>

using namespace DirectX;

class CameraComponent : public GameComponent
{
private:
	XMFLOAT3	m_normal;
	XMFLOAT3	m_focus;
	XMFLOAT3	m_direction;

	float		m_near;
	float		m_far;
	float		m_fov;

	float		m_width;
	float		m_height;

	bool updateFlg;

	void initAction() override;		

public:
	bool frameAction() override;		�@
	void finishAction() override;		

	void changeCameraRatio(float width, float height);
	void changeCameraPosition(float x, float y, float z);
	void changeCameraRotation(float x, float y, float z);
	void changeCameraFocus(float x, float y, float z);
	void changeCameraDepth(float nearZ, float farZ);
	void changeCameraFOVRadian(float fovRad);

	XMFLOAT3 getCameraNormal()
	{
		return m_normal;
	}

	XMFLOAT3 getCameraDirection()
	{
		return m_direction;
	}

	XMFLOAT3 getCameraFocus()
	{
		return m_focus;
	}

	float getViewRatio()
	{
		return m_width / m_height;
	}

	float getSetWidth()
	{
		return m_width;
	}

	float getSetHeight()
	{
		return m_height;
	}

	float getCameraFOVRad()
	{
		return m_fov;
	}

	XMFLOAT2 getCameraRange()
	{
		return XMFLOAT2(m_near, m_far);
	}


};
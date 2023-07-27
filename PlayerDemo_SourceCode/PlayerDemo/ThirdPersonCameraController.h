#pragma once

#include <GameObject.h>
#include "KeyBindComponent.h"
#include "CameraComponent.h"

class ThirdPersonCameraController : public GameComponent
{
private:
	KeyBindComponent* m_keyBind;
	CameraComponent* m_camera;

	float			m_hDeg;
	float			m_vDeg;
	float			m_distance;

	void initAction() override;		

public:
	bool frameAction() override;		Å@
	void finishAction() override;		
};
#pragma once
#include <GameObject.h>
#include "CameraComponent.h"

class FlyingCameraController : public GameComponent
{
private:
	CameraComponent* m_camera;

	XMFLOAT3		m_targetPos;

public:
	void initAction() override;		
	bool frameAction() override;	
	void finishAction() override;	

};
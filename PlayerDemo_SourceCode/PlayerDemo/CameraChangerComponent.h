#pragma once
#include <vector>
#include "GameObject.h"
#include "KeyBindComponent.h"
#include "CameraComponent.h"

class CameraChangerComponent :
    public GameComponent
{
private:
	KeyBindComponent* m_keyBind;
	std::vector<GameComponent*> m_cameraComponents;

	int m_currentCamera = -1;

	void initAction() override;		

public:
	bool frameAction() override;		Å@
	void finishAction() override;		

	void SetCameraController(GameComponent* camCon);
	void ChangeCameraController(int index);
};


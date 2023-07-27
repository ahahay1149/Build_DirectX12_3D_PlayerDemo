#pragma once

#include <Windows.h>

class SceneController
{
protected:
	UINT m_scene = 0;

public:
	HRESULT virtual initSceneController() = 0;
	HRESULT virtual changeGameScene(UINT scene) = 0;
};
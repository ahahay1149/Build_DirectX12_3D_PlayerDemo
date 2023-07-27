#pragma once

#include <memory>
#include <GameObject.h>
#include <SceneController.h>

#include <unordered_map>

#include "TerrainComponent.h"

class GamePrograming2Scene : public SceneController
{
private:
	std::unique_ptr<GameObject> m_systemObject;
	std::unique_ptr<GameComponent> m_keyComponent;

	std::unordered_map<std::wstring, std::unique_ptr<GameComponent>> m_cameraComponents;

	std::vector<TerrainComponent*> m_terrains;

public:
	HRESULT initSceneController() override;
	HRESULT changeGameScene(UINT scene) override;

	GameComponent* getKeyComponent()
	{
		return m_keyComponent.get();
	}

	GameComponent* getCameraComponent(const std::wstring cameraName)
	{
		if (m_cameraComponents[cameraName] != nullptr)
		{
			return m_cameraComponents[cameraName].get();
		}

		return nullptr;
	}

	TerrainComponent* getTerrainComponent(int index)
	{
		if (m_terrains.size() <= index)
			return nullptr;

		return m_terrains[index];
	}

	void clearTerrains()
	{
		m_terrains.clear();
	}
};

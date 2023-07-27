#include <MyAccessHub.h>
#include "CameraComponent.h"
#include "FlyingCameraController.h"

void FlyingCameraController::initAction()
{
	auto components = getGameObject()->getComponents();

	m_camera = nullptr;
	for (auto comp : components)
	{
		m_camera = dynamic_cast<CameraComponent*>(comp);
		if (m_camera != nullptr)
		{
			break;
		}
	}

}

bool FlyingCameraController::frameAction()
{
	m_targetPos = m_camera->getCameraFocus();

	m_camera->changeCameraPosition(m_targetPos.x, m_targetPos.y + 3.0f, m_targetPos.z - 2.0f);
	m_camera->changeCameraFocus(m_targetPos.x, m_targetPos.y, m_targetPos.z);
	return true;
}

void FlyingCameraController::finishAction()
{
}

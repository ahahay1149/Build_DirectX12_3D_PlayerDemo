#include <MyAccessHub.h>
#include "CameraComponent.h"
#include "ThirdPersonCameraController.h"

#include "GamePrograming2Scene.h"

void ThirdPersonCameraController::initAction()
{
	GamePrograming2Scene* scene = dynamic_cast<GamePrograming2Scene*>(MyAccessHub::getMyGameEngine()->GetSceneController());
	m_keyBind = dynamic_cast<KeyBindComponent*>(scene->getKeyComponent());

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

	m_hDeg = 0.0f;
	m_vDeg = 0.0f;
	m_distance = 1.5f;
}

bool ThirdPersonCameraController::frameAction()
{
	int move_x, move_y;
	CharacterData* chData = getGameObject()->getCharacterData();
	float height = m_camera->getSetHeight();
	float width = m_camera->getSetWidth();

	if (m_keyBind->getCurrentInputType() == KeyBindComponent::INPUT_TYPES::KEYBOARD)
	{
		XMINT2 mousePos = MyAccessHub::getMyGameEngine()->GetInputManager()->getMousePosition();

		float half_h = height * 0.5f;	
		float quat_w = width * 0.25f;	

		move_x = mousePos.x - quat_w;	
		move_y = mousePos.y - half_h;	

		m_vDeg = move_y / half_h * 90.0f;	
		m_hDeg = move_x / quat_w * 90.0f;
	}
	else
	{
		float f_temp = 0.0f;

		move_x = m_keyBind->getCurrentAnalogValue(KeyBindComponent::ANALOG_IDS::CAMERA_H);
		move_y = m_keyBind->getCurrentAnalogValue(KeyBindComponent::ANALOG_IDS::CAMERA_V);

		f_temp = move_y / (height * 0.5f) * 90.0f;	
		m_vDeg += f_temp;
		m_vDeg = m_vDeg < -90.0f ? -90.0f : m_vDeg > 90.0f ? 89.9999f : m_vDeg;	

		f_temp = move_x / (width * 0.25f) * 90.0f;	
		m_hDeg += f_temp;
		m_hDeg = m_hDeg < -180.0f ? m_hDeg + 360.0f : m_hDeg > 180.0f ? m_hDeg - 360.0f : m_hDeg;
	}

	XMVECTOR pos = { 0.0f, 0.0f, m_distance, 0.0f };
	XMMATRIX mat = XMMatrixRotationX(XMConvertToRadians(m_vDeg)) * XMMatrixRotationY(XMConvertToRadians(m_hDeg));

	XMFLOAT3 focus = m_camera->getCameraFocus();
	mat = mat * XMMatrixTranslation(focus.x, focus.y, focus.z);	

	pos = XMVector3Transform(pos, mat);	

	m_camera->changeCameraPosition(pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2]);
	return true;
}

void ThirdPersonCameraController::finishAction()
{
}

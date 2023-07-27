#include <MyAccessHub.h>

#include "SkyDomeComponent.h"
#include "GamePrograming2Scene.h"

void SkyDomeComponent::initAction()
{
	CharacterData* chdata = getGameObject()->getCharacterData();
	chdata->setScale(10.0f, 10.0f, 10.0f);	

	chdata->SetGraphicsPipeLine(L"StaticFBX");		
}

bool SkyDomeComponent::frameAction()
{
	CharacterData* myData = getGameObject()->getCharacterData();

	if (centerCharacter != nullptr)
	{
		XMFLOAT3 charaPos = centerCharacter->getPosition();

		myData->setPosition(charaPos.x, charaPos.y, charaPos.z);
	}

	myData->GetPipeline()->AddRenerObject(getGameObject());
	return true;
}

void SkyDomeComponent::finishAction()
{
}

void SkyDomeComponent::setCenterCharacter(CharacterData* target)
{
	centerCharacter = target;
}

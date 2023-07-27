#include "GamePrograming2Scene.h"
#include "GamePrograming2Enum.h"
#include "MyAccessHub.h"
#include "KeyBindComponent.h"

#include "SpriteRenderPipeline.h"

#include "FBXCharacterData.h"		

#include "StandardFbxPipeline.h"	

#include "SkyDomeComponent.h"		
#include "TerrainComponent.h"		
#include "CameraComponent.h"		

#include "UnityChanPlayer.h"		

#include "CameraChangerComponent.h"			
#include "FlyingCameraController.h"			
#include "ThirdPersonCameraController.h"	

HRESULT GamePrograming2Scene::initSceneController()
{
	m_scene = static_cast<UINT>(GAME_SCENES::AWAKE);

	return changeGameScene(static_cast<UINT>(GAME_SCENES::INIT));
}

HRESULT GamePrograming2Scene::changeGameScene(UINT scene)
{
	HRESULT hr = S_OK;
	MyGameEngine* engine = MyAccessHub::getMyGameEngine();

	if (m_scene != scene)
	{
		switch (scene)
		{
		case static_cast<UINT>(GAME_SCENES::INIT):	
			{
				engine->GetTextureManager()->CreateTextureFromFile(engine->GetDirect3DDevice(), L"Sprite00", L"./Resources/textures/texture.png");
				engine->GetMeshManager()->createPresetMeshData();

				PipeLineManager* plMng = engine->GetPipelineManager();
				
				SpriteRenderPipeline* spritePL = new SpriteRenderPipeline();
				spritePL->SetSamplerMode(0);	
				spritePL->SetBlendMode(0);		
				plMng->AddPipeLineObject(L"Sprite", spritePL);

				spritePL = new SpriteRenderPipeline();
				spritePL->SetSamplerMode(0);	
				spritePL->SetBlendMode(1);		
				plMng->AddPipeLineObject(L"AlphaSprite", spritePL);

				StandardFbxPipeline* fbxPL = new StandardFbxPipeline();
				fbxPL->SetAnimationMode(false);		
				plMng->AddPipeLineObject(L"StaticFBX", fbxPL);	

				fbxPL = new StandardFbxPipeline();
				fbxPL->SetAnimationMode(true);		
				plMng->AddPipeLineObject(L"AnimationFBX", fbxPL);
				HitManager* hitMng = engine->GetHitManager();
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_PLAYER_ATTACK, (UINT)HIT_ORDER::HIT_ENEMY_SHIELD);	
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_PLAYER_ATTACK, (UINT)HIT_ORDER::HIT_ENEMY_BODY);	
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_ENEMY_ATTACK, (UINT)HIT_ORDER::HIT_PLAYER_SHIELD);	
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_ENEMY_ATTACK, (UINT)HIT_ORDER::HIT_PLAYER_BODY);	
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_PLAYER_BODY, (UINT)HIT_ORDER::HIT_ENEMY_BODY);		
				hitMng->setHitOrder((UINT)HIT_ORDER::HIT_PLAYER_BODY, (UINT)HIT_ORDER::HIT_ITEM);			


				SoundManager* soMng = engine->GetSoundManager();

				int soundId = 0;

				if (!soMng->loadSoundFile(L"./Resources/sounds/playerShot.wav", soundId))
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/playerOut.wav", soundId))
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/enemyHit.wav", soundId))
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/enemyOut.wav", soundId))
					return E_FAIL;
				if (!soMng->loadSoundFile(L"./Resources/sounds/Barrier.wav", soundId))
					return E_FAIL;

				m_systemObject = make_unique<GameObject>(nullptr);
				m_keyComponent = make_unique<KeyBindComponent>();
				m_systemObject->addComponent(m_keyComponent.get());
				engine->AddGameObject(m_systemObject.get());

				engine->UploadCreatedTextures();

				changeGameScene(static_cast<UINT>(GAME_SCENES::IN_GAME));
			}
			break;

		case static_cast<UINT>(GAME_SCENES::TITLE):
			break;

		case static_cast<UINT>(GAME_SCENES::IN_GAME):
			{
				PipeLineManager* plMng = engine->GetPipelineManager();

				FBXCharacterData* terrainFbx = new FBXCharacterData();
				if (FAILED(terrainFbx->LoadMainFBX(L"./Resources/fbx/TerrainSample.fbx", L"TerrainSample")))
					return E_FAIL;

				GameObject* terrainObj = new GameObject(terrainFbx);

				TerrainComponent* trCom = new TerrainComponent();
				terrainObj->addComponent(trCom);

				engine->AddGameObject(terrainObj);

				m_terrains.push_back(trCom);
				FBXCharacterData* skydomeFbx = new FBXCharacterData();
				if (FAILED(skydomeFbx->LoadMainFBX(L"./Resources/fbx/SkyDome001.fbx", L"SkyDome")))
					return E_FAIL;

				GameObject* skydomeObj = new GameObject(skydomeFbx);
				
				SkyDomeComponent* skCom = new SkyDomeComponent();
				skydomeObj->addComponent(skCom);

				engine->AddGameObject(skydomeObj);
				GameObject* unityChanObj;	
				FBXCharacterData* unityChanFbx = new FBXCharacterData();	
				UnityChanPlayer* unityChanPlayer = new UnityChanPlayer();	
				unityChanObj = new GameObject(unityChanFbx);	
				unityChanObj->addComponent(unityChanPlayer);	
				engine->AddGameObject(unityChanObj);			

				GameObject* cameraObj;
				cameraObj = new GameObject(new CharacterData());
				CameraComponent* camComp = new CameraComponent();
				cameraObj->addComponent(camComp);
				engine->AddGameObject(cameraObj);

				FlyingCameraController* flyCam = new FlyingCameraController();
				cameraObj->addComponent(flyCam);

				ThirdPersonCameraController* tpCam = new ThirdPersonCameraController();
				cameraObj->addComponent(tpCam);

				tpCam->setActive(false);	

				CameraChangerComponent* camChanger = new CameraChangerComponent();
				cameraObj->addComponent(camChanger);		

				camChanger->SetCameraController(flyCam);	
				camChanger->SetCameraController(tpCam);		

				camChanger->ChangeCameraController(0);		
				engine->SetCameraData(cameraObj->getCharacterData());

				camComp->changeCameraRatio(engine->GetWidth(), engine->GetHeight());
				camComp->changeCameraDepth(0.01f, 1000.0f);
				camComp->changeCameraFOVRadian(DirectX::XMConvertToRadians(45.0f));

				camComp->changeCameraPosition(1.5f, 1.2f, 0.0f);

				unityChanPlayer->SetCurrentCamera(camComp);

				engine->UploadCreatedTextures();
		}
			break;

		case static_cast<UINT>(GAME_SCENES::GAME_OVER):
			break;

		default:
			return E_FAIL;	
		}

		m_scene = scene;

		engine->WaitForGpu();	
	}

	return hr;
}

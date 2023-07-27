#include "UnityChanPlayer.h"
#include "FBXCharacterData.h"

#include "GamePrograming2Scene.h"   
#include "KeyBindComponent.h"       

#define MOVE_3D (0)                 

void UnityChanPlayer::initAction()
{
    FBXCharacterData* chdata = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
    chdata->SetGraphicsPipeLine(L"AnimationFBX");       

    chdata->LoadMainFBX(L"./Resources/fbx/unitychan.fbx", L"UnityChan");
    chdata->setPosition(0.0f, 0.0f, 0.0f);      
    chdata->setScale(0.01f, 0.01f, 0.01f);      

    XMFLOAT3 min = chdata->GetMainFbx()->GetFbxMin();       
    XMFLOAT3 max = chdata->GetMainFbx()->GetFbxMax();       

    float headY = (max.y - min.y) * 0.8f;                   
    m_unityChanHeadHeight = headY * chdata->getScale().y;   

    m_walkableHeight = (max.y - min.y) * 0.25f * chdata->getScale().y;

    chdata->LoadAnimationFBX(L"./Resources/fbx/UnityChanAnime/unitychan_WAIT00.fbx", L"WAIT00");
    chdata->LoadAnimationFBX(L"./Resources/fbx/UnityChanAnime/unitychan_WALK00_F.fbx", L"WALK_F");
    chdata->SetAnime(L"WAIT00");    

}

bool UnityChanPlayer::frameAction()
{
    FBXCharacterData* chData = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());

    GamePrograming2Scene* scene = static_cast<GamePrograming2Scene*>(MyAccessHub::getMyGameEngine()->GetSceneController());
    KeyBindComponent* keyBind = static_cast<KeyBindComponent*>(scene->getKeyComponent());
    XMFLOAT3 moveVect = {};     

    TerrainComponent* terCom = scene->getTerrainComponent(0);
    XMFLOAT3 hitPos = {};       
    XMFLOAT3 hitNormal = {};    
    XMFLOAT3 rayStart;          
    XMFLOAT3 rayEnd;            
    HitRayLine ray;             

    switch (keyBind->getCurrentInputType())
    {
        case KeyBindComponent::INPUT_TYPES::KEYBOARD:
        {
            if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS,
                KeyBindComponent::BUTTON_IDS::MOVE_LEFT))
            {
                moveVect.x = -1.0f; 
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS,
                KeyBindComponent::BUTTON_IDS::MOVE_RIGHT))
            {
                moveVect.x = 1.0f;  
            }

            if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS,
                KeyBindComponent::BUTTON_IDS::MOVE_BACK))
            {
                moveVect.z = -1.0f; 
            }
            else if (keyBind->getCurrentInputState(InputManager::BUTTON_STATE::BUTTON_PRESS,
                KeyBindComponent::BUTTON_IDS::MOVE_FORWARD))
            {
                moveVect.z = 1.0f;  
            }

            if (moveVect.x != 0.0f && moveVect.z != 0.0f)
            {
                moveVect.x /= 1.41421356f;
                moveVect.y /= 1.41421356f;
            }

            break;
        }
        case KeyBindComponent::INPUT_TYPES::XINPUT_0:
        {
            moveVect.x = keyBind->getAnalogValue
                (KeyBindComponent::INPUT_TYPES::XINPUT_0, KeyBindComponent::ANALOG_IDS::MOVE_H);
            moveVect.z = keyBind->getAnalogValue
                (KeyBindComponent::INPUT_TYPES::XINPUT_0, KeyBindComponent::ANALOG_IDS::MOVE_V);

            break;
        }
    }

    XMFLOAT3 camVect = m_currentCamera->getCameraDirection();
    XMVECTOR rotMove = {};  
    float xzRadian;         
    xzRadian = atan2f(camVect.x, camVect.z);        
    XMMATRIX qtXZ = XMMatrixRotationY(xzRadian);     

#if MOVE_3D
    float yzRadian;     
    XMVECTOR workVect = XMVector3Transform(XMLoadFloat3(&camVect),XMMatrixRotationY(-xzRadian));

    yzRadian = atan2f(-XMVectorGetY(workVect),XMVectorGetZ(workVect));
    XMMATRIX qtYZ = XMMatrixRotationX(yzRadian);

    rotMove = XMVector3Transform(XMLoadFloat3(&moveVect), qtYZ * qtXZ);

    chData->setRotation(XMConvertToDegrees(yzRadian), XMConvertToDegrees(xzRadian), 0.0f);

#else
    rotMove = XMVector3Transform(XMLoadFloat3(&moveVect), qtXZ);

    if (moveVect.x != 0.0f || moveVect.z != 0.0f)
    {
        float charRad = atan2f(XMVectorGetX(rotMove),XMVectorGetZ(rotMove));
        chData->setRotation(0.0f, XMConvertToDegrees(charRad), 0.0f);
        chData->SetAnime(L"WALK_F");    
    }
    else
    {
        chData->SetAnime(L"WAIT00");    
    }

#endif  

    moveVect.x = XMVectorGetX(rotMove); 
    moveVect.y = XMVectorGetY(rotMove);
    moveVect.z = XMVectorGetZ(rotMove);

    moveVect.x *= 0.05f;
    moveVect.y *= 0.05f;    
    moveVect.z *= 0.05f;

    XMFLOAT3 nowPos = chData->getPosition();    
    nowPos.x += moveVect.x;  
    nowPos.y += moveVect.y;
    nowPos.z += moveVect.z;

    bool canwalk = true;

    rayStart = chData->getPosition();
    rayStart.y += m_walkableHeight;
    rayEnd = nowPos;
    rayEnd.y += m_walkableHeight;

    ray.setLine(rayStart, rayEnd, 0.0f);

    if (terCom->RayCastHit(ray, hitPos, hitNormal))
    {
        nowPos.x = hitPos.x;
        nowPos.z = hitPos.z;
    }

    rayStart = nowPos;
    rayEnd = nowPos;
    rayStart.y += m_walkableHeight;
    rayEnd.y -= m_walkableHeight;

    ray.setLine(rayStart, rayEnd, 0.0f);
    if (terCom->RayCastHit(ray, hitPos, hitNormal))
    {
        nowPos.y = hitPos.y;
    }
    else
    {
        canwalk = false; 
    }
    chData->setPosition(nowPos.x, nowPos.y, nowPos.z);

    chData->UpdateAnimation();

    chData->GetPipeline()->AddRenerObject(getGameObject());

    XMFLOAT3 plPos = chData->getPosition(); 

    m_currentCamera->changeCameraFocus(plPos.x, plPos.y + m_unityChanHeadHeight, plPos.z);

    return true;
}

void UnityChanPlayer::finishAction()
{

}

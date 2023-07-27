#pragma once
#include <GameObject.h>
#include "CameraComponent.h"

class UnityChanPlayer : public GameComponent
{
private:
    float   m_unityChanHeadHeight;  
    float   m_walkableHeight;       

    CameraComponent* m_currentCamera;

public:
    virtual void initAction() override;
    virtual bool frameAction() override;
    virtual void finishAction() override;

    void SetCurrentCamera(CameraComponent* cam)
    {
        m_currentCamera = cam;
    }

};


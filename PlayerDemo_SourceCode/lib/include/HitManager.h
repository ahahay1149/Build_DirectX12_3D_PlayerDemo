#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <list>
#include "GameObject.h"

#include "HitShapes.h"

using namespace DirectX;
using namespace std;

class GameComponent;

class HitManager
{
private:
    class HitStructure	　
    {
    private:
		HitAreaBase* m_pHitArea;							
        GameComponent* m_pGameComponent;          
    public:
        HitStructure(GameComponent* cmp, HitAreaBase* hitArea)
        {
            m_pHitArea = hitArea;
            m_pGameComponent = cmp;
        }

        GameComponent* getGameComponent();			
		HitAreaBase* getHitArea();						
    };

	list<DirectX::XMUINT2> m_hitOrders;
	
    UINT m_numOfHitTypes = 0;
    vector<unique_ptr<list<HitStructure*>>> m_hitArray;

    void flushHitList(list<HitStructure*>* p_hitlist);  　

    XMFLOAT3    m_lastHitPos;

public:
	void initHitList(UINT hitTypes);
	void setHitOrder(UINT atk, UINT def);

    void refreshHitSystem();                            　
    void setHitArea(GameComponent * cmp, HitAreaBase* box);  

    void hitFrameAction();                              　

    bool isHit(HitAreaBase* atkHit, HitAreaBase* defHit);

    XMFLOAT3 getLastHitPoint()
    {
        return m_lastHitPos;
    }
};

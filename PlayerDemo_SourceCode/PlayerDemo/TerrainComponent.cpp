#include <MyAccessHub.h>
#include <HitManager.h>
#include "TerrainComponent.h"

#include "FBXCharacterData.h"

void TerrainComponent::initAction()
{
	FBXCharacterData* fbxChara = static_cast<FBXCharacterData*>(getGameObject()->getCharacterData());
	fbxChara->SetGraphicsPipeLine(L"StaticFBX");	

	m_bloadHit.setAABBMinMax(fbxChara->GetMainFbx()->GetFbxMin(), fbxChara->GetMainFbx()->GetFbxMax());

	MeshContainer* meshCon;		
	FbxVertex *vA, *vB, *vC;	 
	unique_ptr<TerrainPlate> pTerrainPlate;	

	int index = 0;		
	UINT vcount = 0;	

	while ((meshCon = fbxChara->GetMainFbx()->GetMeshContainer(index)) != nullptr)
	{
		vcount = meshCon->m_vertexCount;

		for (UINT i = 0; i < vcount; i += 3)
		{
			vA = &meshCon->m_vertexData[meshCon->m_indexData[i]];
			vB = &meshCon->m_vertexData[meshCon->m_indexData[i + 1]];
			vC = &meshCon->m_vertexData[meshCon->m_indexData[i + 2]];

			pTerrainPlate = make_unique<TerrainPlate>();
			pTerrainPlate.get()->setTerrainPlate(vA->position, vB->position, vC->position);

			m_terrainHit.push_back(move(pTerrainPlate));
		}

		index++;
	}
}

bool TerrainComponent::frameAction()
{
	getGameObject()->getCharacterData()->GetPipeline()->AddRenerObject(getGameObject());
	return true;
}

void TerrainComponent::finishAction()
{
}

void TerrainComponent::hitReaction(GameObject* obj, HitAreaBase* hit)
{
}

bool TerrainComponent::RayCastHit(const XMFLOAT3& rayStart, const XMFLOAT3& rayEnd, XMFLOAT3& hitPosition, XMFLOAT3& hitNormal)
{
	HitRayLine rayLine;
	rayLine.setLine(rayStart, rayEnd, 0);

	return RayCastHit(rayLine, hitPosition, hitNormal);
}

bool TerrainComponent::RayCastHit(HitRayLine& ray, XMFLOAT3& hitPosition, XMFLOAT3& hitNormal)
{
	HitManager* hitMng = MyAccessHub::getMyGameEngine()->GetHitManager();
	XMVECTOR nmlVect = ray.getNormal();
	XMFLOAT3 rayNormal = {XMVectorGetX(nmlVect), XMVectorGetY(nmlVect), XMVectorGetZ(nmlVect)};
	XMFLOAT3 tempHit = {};
	XMFLOAT3 tempNml = {};

	bool res = false;

	if (hitMng->isHit(&ray, &m_bloadHit))
	{
		for (int i = 0; i < m_terrainHit.size(); i++)
		{
			TerrainPlate* terHit = m_terrainHit[i].get();

			if (terHit->RayCastHit(ray, tempHit, tempNml))
			{
				if (!res)
				{
					res = true;
					hitPosition = tempHit;
					hitNormal = tempNml;
				}
				else
				{
					bool ow = false;

					if (rayNormal.x > 0.0f)
					{
						if (hitPosition.x > tempHit.x)
						{
							hitPosition.x = tempHit.x;
							ow = true;
						}
					}
					else
					{
						if (hitPosition.x < tempHit.x)
						{
							hitPosition.x = tempHit.x;
							ow = true;
						}
					}

					if (rayNormal.y > 0.0f)
					{
						if (hitPosition.y > tempHit.y)
						{
							hitPosition.y = tempHit.y;
							ow = true;
						}
					}
					else
					{
						if (hitPosition.y < tempHit.y)
						{
							hitPosition.y = tempHit.y;
							ow = true;
						}
					}

					if (rayNormal.z > 0.0f)
					{
						if (hitPosition.z > tempHit.z)
						{
							hitPosition.z = tempHit.z;
							ow = true;
						}
					}
					else
					{
						if (hitPosition.z < tempHit.z)
						{
							hitPosition.z = tempHit.z;
							ow = true;
						}
					}

					if (ow)
					{
						hitNormal.x = (hitNormal.x + tempNml.x) * 0.5f;
						hitNormal.y = (hitNormal.y + tempNml.y) * 0.5f;
						hitNormal.z = (hitNormal.z + tempNml.z) * 0.5f;
					}
				} 
			}	
		}	
	}	

	return res;

}	

void TerrainComponent::TerrainPlate::setTerrainPlate(const XMFLOAT3& pA, const XMFLOAT3& pB, const XMFLOAT3& pC)
{
	mainHit.setTriangle(pA, pB, pC);	

	XMFLOAT3 max = pA;	
	XMFLOAT3 min = pA;	

	for (int i = 0; i < 2; i++)
	{
		XMFLOAT3 target;
		switch (i)
		{
		case 0:
			target = pB;
			break;
		default:
			target = pC;
			break;
		}

		if (max.x < target.x)
				max.x = target.x;
		if (max.y < target.y)
				max.y = target.y;
		if (max.z < target.z)
				max.z = target.z;

		if (min.x > target.x)
				min.x = target.x;
		if (min.y > target.y)
				min.y = target.y;
		if (min.z > target.z)
				min.z = target.z;
	}

	subHit.setAABBMinMax(min, max);	

}

bool TerrainComponent::TerrainPlate::RayCastHit(HitRayLine& ray, XMFLOAT3& hitPosition, XMFLOAT3& hitNormal)
{
	HitManager* hitMng = MyAccessHub::getMyGameEngine()->GetHitManager();

	if (!hitMng->isHit(&ray, &subHit))
	{
		return false;	
	}

	if (hitMng->isHit(&ray, &mainHit))
	{
		XMFLOAT3 pos = hitMng->getLastHitPoint();
		hitPosition.x = pos.x;
		hitPosition.y = pos.y;
		hitPosition.z = pos.z;

		XMVECTOR nml = mainHit.getNormal();
		hitNormal.x = XMVectorGetX(nml);
		hitNormal.y = XMVectorGetY(nml);
		hitNormal.z = XMVectorGetZ(nml);

		return true;
	}

	return false;

}

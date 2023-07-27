#pragma once
#include <HitShapes.h>

#include "GameObject.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class TerrainComponent : public GameComponent
{
private:
	class TerrainPlate
	{
		private:
			HitAABB subHit;			
			HitTriangle mainHit;	
		public:
			void setTerrainPlate(const XMFLOAT3& pA, const XMFLOAT3& pB, const XMFLOAT3& pC);
			bool RayCastHit(HitRayLine& ray, XMFLOAT3& hitPosition, XMFLOAT3& hitNormal);
	};

	HitAABB m_bloadHit;	
	std::vector<unique_ptr<TerrainPlate>> m_terrainHit;	
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void hitReaction(GameObject* obj, HitAreaBase* hit) override;Å@

	bool RayCastHit(const XMFLOAT3& rayStart, const XMFLOAT3& rayEnd, 
					XMFLOAT3& hitPosition, XMFLOAT3& hitNormal);
	bool RayCastHit(HitRayLine& ray, XMFLOAT3& hitPosition, XMFLOAT3& hitNomal);
};
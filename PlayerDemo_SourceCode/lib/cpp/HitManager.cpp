#include <list>
#include <algorithm>		

#include "HitManager.h"

bool isHitVS(HitQuad* atk, HitQuad* def, XMFLOAT3& resHitPos);			  
bool isHitVS(HitQuad* atk, HitCircle* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitQuad* atk, HitTriangle* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitQuad* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck);			  
bool isHitVS(HitQuad* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck);			  
bool isHitVS(HitQuad* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitQuad* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitQuad* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);		  

bool isHitVS(HitCircle* atk, HitCircle* def, XMFLOAT3& resHitPos);		  
bool isHitVS(HitCircle* atk, HitTriangle* def, XMFLOAT3& resHitPos, bool revCheck);	  
bool isHitVS(HitCircle* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitCircle* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitCircle* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitCircle* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);	  
bool isHitVS(HitCircle* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);	  

bool isHitVS(HitTriangle* atk, HitTriangle* def, XMFLOAT3& resHitPos);	  
bool isHitVS(HitTriangle* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitTriangle* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitTriangle* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck);	  
bool isHitVS(HitTriangle* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);	  
bool isHitVS(HitTriangle* atk, HitRayLine* def, XMFLOAT3& resHitPos);	  	

bool isHitVS(HitAABB* atk, HitAABB* def, XMFLOAT3& resHitPos);			  	
bool isHitVS(HitAABB* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck);			  
bool isHitVS(HitAABB* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck);		  	
bool isHitVS(HitAABB* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitAABB* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);		  	

bool isHitVS(HitOBB* atk, HitOBB* def, XMFLOAT3& resHitPos);			  	
bool isHitVS(HitOBB* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitOBB* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);		  
bool isHitVS(HitOBB* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);		  	

bool isHitVS(HitSphere* atk, HitSphere* def, XMFLOAT3& resHitPos);		  		
bool isHitVS(HitSphere* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck);	  
bool isHitVS(HitSphere* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);	  	

bool isHitVS(HitPillar* atk, HitPillar* def, XMFLOAT3& resHitPos);	  	
bool isHitVS(HitPillar* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck);	  

bool isHitVS(HitRayLine* atk, HitRayLine* def, XMFLOAT3& resHitPos);	  	

bool getVectorT(const float max, const float min, const float nml, const float pos, float& sT, float& eT);

bool isHitVS(HitQuad* atk, HitQuad* def, XMFLOAT3& resHitPos)
{
	XMFLOAT4 myRect = atk->getHitRect();			
	XMFLOAT4 targetRect = def->getHitRect();		

	if (targetRect.x > myRect.z || targetRect.z < myRect.x)
	{
		return false;	
	}

	if (targetRect.y > myRect.w || targetRect.w < myRect.y)
	{
		return false;	
	}

	XMFLOAT2 atkCenter = { (myRect.z - myRect.x) * 0.5f, (myRect.w - myRect.y) * 0.5f };
	if (atkCenter.x < targetRect.x)
	{
		resHitPos.x = targetRect.x;
	}
	else if (atkCenter.x > targetRect.z)
	{
		resHitPos.x = targetRect.z;
	}
	else
	{
		resHitPos.x = atkCenter.x;	
	}

	if (atkCenter.y < targetRect.y)
	{
		resHitPos.y = targetRect.y;
	}
	else if (atkCenter.y > targetRect.w)
	{
		resHitPos.y = targetRect.w;
	}
	else
	{
		resHitPos.y = atkCenter.y;	
	}

	return true;		
}

bool isHitVS(HitQuad* atk, HitCircle* def, XMFLOAT3& resHitPos, bool revCheck)
{
	XMFLOAT4 atkRect = atk->getHitRect();
	XMFLOAT2 defCent = def->getCenterPosition();

	float r = def->getRadius();	

	if ((atkRect.w + r < defCent.y || atkRect.y - r > defCent.y) && (atkRect.z < defCent.x || atkRect.x > defCent.x))
	{
		if ((atkRect.z + r < defCent.x || atkRect.x - r > defCent.x) && (atkRect.w < defCent.y || atkRect.y > defCent.y))
		{
			float x_dist, y_dist;
			float dR = def->getDoubledRadius();
			float totalDist = 0.0f;

			for (int i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0:
					x_dist = atkRect.x - defCent.x;
					y_dist = atkRect.y - defCent.y;
					break;

				case 1:
					x_dist = atkRect.z - defCent.x;
					y_dist = atkRect.y - defCent.y;
					break;

				case 2:
					x_dist = atkRect.x - defCent.x;
					y_dist = atkRect.w - defCent.y;
					break;

				default:
					x_dist = atkRect.z - defCent.x;
					y_dist = atkRect.w - defCent.y;
					break;
				}

				totalDist = x_dist * x_dist + y_dist * y_dist;
				if (totalDist <= dR)
				{
					switch (i)
					{
					case 0:	
						resHitPos.x = atkRect.x;
						resHitPos.x = atkRect.y;
						break;

					case 1:	
						resHitPos.x = atkRect.z;
						resHitPos.y = atkRect.y;
						break;

					case 2:	
						resHitPos.x = atkRect.x;
						resHitPos.y = atkRect.w;
						break;

					default:
						resHitPos.x = atkRect.z;
						resHitPos.y = atkRect.w;
						break;
					}

					resHitPos.z = 0.0f;
					return true;
				}
			}

			return false;
		}

	}

	if (defCent.x < atkRect.x)
	{
		resHitPos.x = atkRect.x;
	}
	else if (defCent.x > atkRect.z)
	{
		resHitPos.x = atkRect.z;
	}
	else
	{
		resHitPos.x = defCent.x;
	}

	if (defCent.y < atkRect.y)
	{
		resHitPos.y = atkRect.y;
	}
	else if (defCent.y > atkRect.w)
	{
		resHitPos.y = atkRect.w;
	}
	else
	{
		resHitPos.y = defCent.y;
	}

	resHitPos.z = 0.0f;
	return true;
}

bool isHitVS(HitQuad* atk, HitTriangle* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitQuad* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitQuad* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitQuad* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitQuad* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitQuad* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitCircle* def, XMFLOAT3& resHitPos)
{
	XMFLOAT2 centerA = atk->getCenterPosition();
	XMFLOAT2 centerB = def->getCenterPosition();

	float xdist = centerB.x - centerA.x;	
	float ydist = centerB.y - centerA.y;	

	float length = (xdist * xdist + ydist * ydist);
	if ((atk->getDoubledRadius() + def->getDoubledRadius() + 2 * atk->getRadius() * def->getRadius()) < length)
	{
		return false;
	}

	length = sqrtf(length);
	XMFLOAT2 nmlVect = { xdist / length, ydist / length };

	if (length < atk->getRadius())
	{
		resHitPos.x = centerA.x + atk->getRadius() * nmlVect.x;
		resHitPos.y = centerA.y + atk->getRadius() * nmlVect.y;
	}
	else
	{
		resHitPos.x = centerB.x - def->getRadius() * nmlVect.x;
		resHitPos.y = centerB.y - def->getRadius() * nmlVect.y;
	}

	resHitPos.z = 0.0f;

	return true;				
}

bool isHitVS(HitCircle* atk, HitTriangle* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitCircle* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitTriangle* atk, HitTriangle* def, XMFLOAT3& resHitPos)
{
	return false;
}

bool isHitVS(HitTriangle* atk, HitAABB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitTriangle* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitTriangle* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitTriangle* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool getVectorT(const float max, const float min, const float nml, const float pos, float& sT, float& eT)
{
	if (nml < 0.0f)
	{
		sT = (max - pos) / nml;	
		eT = (min - pos) / nml;
	}
	else if (nml > 0.0f)
	{
		sT = (min - pos) / nml;	
		eT = (max - pos) / nml;
	}
	else
	{
		if (pos < min || pos > max)
		{
			return false;
		}
		else
		{
			sT = 0.0f;
			eT = FLT_MAX;
		}
	}

	if (sT < 0.0f) sT = 0.0f;

	if (eT < 0.0f) return false;

	return true;
}

#define TRIANGLE_CHECK_MODE (1)
#if TRIANGLE_CHECK_MODE == 0
bool isHitVS(HitTriangle* atk, HitRayLine* def, XMFLOAT3& resHitPos)
{
	XMFLOAT3 stPos = def->getStart();	
	XMVECTOR stVect = XMLoadFloat3(&stPos);

	XMVECTOR poNormal = atk->getNormal();

	XMVECTOR rayNormal = def->getNormal();

	float	rayLength = def->getLength();

	float dot = XMVector3Dot(rayNormal, poNormal).m128_f32[0];
	if (dot > -0.0001f && dot < 0.0001f)
	{
		return false;	
	}

	float rayDot = XMVector3Dot(stVect, poNormal).m128_f32[0];

	float crLen = -1.0f * rayDot / dot;

	if (crLen < 0.0f || crLen > rayLength)
	{
		return false;	  
	}

	XMFLOAT3 position = { stPos.x + XMVectorGetX(rayNormal) * crLen, stPos.y + XMVectorGetY(rayNormal) * crLen, stPos.z + XMVectorGetZ(rayNormal) * crLen };

	XMFLOAT3* vertex = atk->getVertex();	

	XMVECTOR vectAB = atk->getVectAB();		
	XMVECTOR vectBP = XMVectorSet(position.x - vertex[1].x, position.y - vertex[1].y, position.z - vertex[1].z, 0.0f);

	XMVECTOR vectBC = XMVectorSet(vertex[2].x - vertex[1].x, vertex[2].y - vertex[1].y, vertex[2].z - vertex[1].z, 0.0f);
	XMVECTOR vectCP = XMVectorSet(position.x - vertex[2].x, position.y - vertex[2].y, position.z - vertex[2].z, 0.0f);

	XMVECTOR vectCA = XMVectorSet(vertex[0].x - vertex[2].x, vertex[0].y - vertex[2].y, vertex[0].z - vertex[2].z, 0.0f);
	XMVECTOR vectAP = XMVectorSet(position.x - vertex[0].x, position.y - vertex[0].y, position.z - vertex[0].z, 0.0f);

	XMVECTOR crossABP = XMVector3Cross(vectAB, vectBP);
	XMVECTOR crossBCP = XMVector3Cross(vectBC, vectCP);
	XMVECTOR crossCAP = XMVector3Cross(vectCA, vectAP);

	if (XMVector3Dot(crossABP, crossBCP).m128_f32[0] <= 0.0f || XMVector3Dot(crossABP, crossCAP).m128_f32[0] <= 0.0f)
	{
		return false;	
	}

	resHitPos = position;

	return true;
}

#else
float det(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c)
{
	float res = a.x * b.y * c.z + a.z * b.x * c.y + a.y * b.z * c.x;
	res -= a.z * b.y * c.x + a.y * b.x * c.z + a.x * b.z * c.y;
	return res;
}

bool isHitVS(HitTriangle* atk, HitRayLine* def, XMFLOAT3& resHitPos)
{
	XMVECTOR vectAB = atk->getVectAB();
	XMVECTOR vectAC = atk->getVectAC();

	XMFLOAT3 rayStart = def->getStart();
	XMFLOAT3 rayEnd = def->getEnd();

	XMFLOAT3 flA = { XMVectorGetX(vectAB), XMVectorGetY(vectAB), XMVectorGetZ(vectAB) };
	XMFLOAT3 flB = { XMVectorGetX(vectAC), XMVectorGetY(vectAC), XMVectorGetZ(vectAC) };

	XMFLOAT3 flC = { rayStart.x - rayEnd.x, rayStart.y - rayEnd.y, rayStart.z - rayEnd.z };

	float detABC = det(flA, flB, flC);

	if (detABC < 0.0001f && detABC > -0.0001f)
	{
		return false;
	}

	detABC = 1.0f / detABC;	

	XMFLOAT3 vtxA = atk->getVertex()[0];
	XMFLOAT3 flD = { rayStart.x - vtxA.x, rayStart.y - vtxA.y, rayStart.z - vtxA.z };

	float u, v, t;
	u = det(flD, flB, flC) * detABC;	
	v = det(flA, flD, flC) * detABC;
	t = det(flA, flB, flD) * detABC;

	if (u > 1.0f || u < 0.0f) return false;
	if (v > 1.0f || v < 0.0f) return false;
	if (t > 1.0f || t < 0.0f) return false;

	if ((u + v) > 1.0f) return false;

	resHitPos.x = rayStart.x - flC.x * t;
	resHitPos.y = rayStart.y - flC.y * t;
	resHitPos.z = rayStart.z - flC.z * t;

	return true;
}
#endif

bool isHitVS(HitAABB* atk, HitAABB* def, XMFLOAT3& resHitPos)
{
	XMFLOAT3 atkMin, atkMax;
	XMFLOAT3 defMin, defMax;

	atkMin = atk->getMin();
	atkMax = atk->getMax();
	defMin = def->getMin();
	defMax = def->getMax();

	if (atkMax.x < defMin.x || atkMax.y < defMin.y || atkMax.z < defMin.z ||
		defMax.x < atkMin.x || defMax.y < atkMin.y || defMax.z < atkMin.z)
		return false;

	XMFLOAT3 atkCenter = atk->getCenter();
	if (atkCenter.x < defMin.x)
	{
		resHitPos.x = defMin.x;
	}
	else if (atkCenter.x > defMax.x)
	{
		resHitPos.x = defMax.x;
	}
	else
	{
		resHitPos.x = atkCenter.x;	
	}

	if (atkCenter.y < defMin.y)
	{
		resHitPos.y = defMin.y;
	}
	else if (atkCenter.y > defMax.y)
	{
		resHitPos.y = defMax.y;
	}
	else
	{
		resHitPos.y = atkCenter.y;	
	}

	if (atkCenter.z < defMin.z)
	{
		resHitPos.z = defMin.z;
	}
	else if (atkCenter.z > defMax.z)
	{
		resHitPos.z = defMax.z;
	}
	else
	{
		resHitPos.z = atkCenter.z;	
	}


	return true;
}

bool isHitVS(HitAABB* atk, HitOBB* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitAABB* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck)
{
	XMFLOAT3	atkMin = atk->getMin();
	XMFLOAT3	atkMax = atk->getMax();

	XMFLOAT3	center = def->getCenter();

	XMFLOAT3	vect = {};						

	float		dR = def->getDoubleRadius();	

	float		len = 0.0f;						
	float		dist = 0.0f;					

	if (center.x < atkMin.x)
	{
		dist = atkMin.x - center.x;
	}
	else if (center.x > atkMax.x)
	{
		dist = atkMax.x - center.x;
	}

	len += dist * dist;					
	vect.x = dist;
	dist = 0.0f;

	if (center.y < atkMin.y)
	{
		dist = atkMin.y - center.y;
	}
	else if (center.y > atkMax.y)
	{
		dist = atkMax.y - center.y;
	}

	len += dist * dist;					
	vect.y = dist;
	dist = 0.0f;

	if (center.z < atkMin.z)
	{
		dist = atkMin.z - center.z;
	}
	else if (center.z > atkMax.z)
	{
		dist = atkMax.z - center.z;
	}

	len += dist * dist;					
	vect.z = dist;

	if (len > dR) return false;

	resHitPos.x = center.x + vect.x;
	resHitPos.y = center.y + vect.y;
	resHitPos.z = center.z + vect.z;

	return true;
}

bool isHitVS(HitAABB* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitAABB* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	XMFLOAT3 min = atk->getMin();
	XMFLOAT3 max = atk->getMax();

	XMFLOAT3 startP = def->getStart();
	XMVECTOR lineNormal = def->getNormal();
	float length = def->getLength();

	float minEnd = FLT_MAX, maxStart = FLT_MIN;
	float sT, eT;

	for (int i = 0; i < 3; i++)
	{
		float max_v, min_v, nml_v, pos_v;

		switch (i)
		{
		case 0:
			max_v = max.x;
			min_v = min.x;
			nml_v = XMVectorGetX(lineNormal);
			pos_v = startP.x;
			break;

		case 1:
			max_v = max.y;
			min_v = min.y;
			nml_v = XMVectorGetY(lineNormal);
			pos_v = startP.y;
			break;

		default:
			max_v = max.z;
			min_v = min.z;
			nml_v = XMVectorGetZ(lineNormal);
			pos_v = startP.z;
			break;
		}

		if (!getVectorT(max_v, min_v, nml_v, pos_v, sT, eT)) return false;

		if (maxStart < sT) maxStart = sT;

		if (minEnd > eT) minEnd = eT;

		if (maxStart >= minEnd) return false;
	}

	if (length < maxStart)
		return false;	

	resHitPos.x = startP.x + XMVectorGetX(lineNormal) * maxStart;
	resHitPos.y = startP.y + XMVectorGetY(lineNormal) * maxStart;
	resHitPos.z = startP.z + XMVectorGetZ(lineNormal) * maxStart;

	return true;
}

bool isHitVS(HitOBB* atk, HitOBB* def, XMFLOAT3& resHitPos)
{
	return false;
}

bool isHitVS(HitOBB* atk, HitSphere* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitOBB* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitOBB* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitSphere* atk, HitSphere* def, XMFLOAT3& resHitPos)
{
	float atk_r = atk->getRadius();
	float def_r = def->getRadius();

	XMFLOAT3 aC = atk->getCenter();
	XMFLOAT3 dC = def->getCenter();

	float dR = atk->getDoubleRadius() + def->getDoubleRadius() + 2.0f * atk_r * def_r;

	float dist_x, dist_y, dist_z;
	dist_x = dC.x - aC.x;
	dist_y = dC.y - aC.y;
	dist_z = dC.z - aC.z;

	float length = dist_x * dist_x + dist_y * dist_y + dist_z * dist_z;
	if (dR < length) return false;

	length = sqrtf(length);

	XMFLOAT3 nmlVect = { dist_x / length, dist_y / length, dist_z / length };

	if (length < def_r)
	{
		resHitPos.x = aC.x + atk_r * nmlVect.x;
		resHitPos.y = aC.y + atk_r * nmlVect.y;
		resHitPos.z = aC.z + atk_r * nmlVect.z;
	}
	else
	{
		resHitPos.x = dC.x - def_r * nmlVect.x;
		resHitPos.y = dC.y - def_r * nmlVect.y;
		resHitPos.z = dC.z - def_r * nmlVect.z;
	}

	return true;
}

bool isHitVS(HitSphere* atk, HitPillar* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitSphere* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	XMFLOAT3 lStart = def->getStart();
	XMFLOAT3 lEnd = def->getEnd();
	XMFLOAT3 spCenter = atk->getCenter();

	XMFLOAT3 vJ = { lStart.x - spCenter.x, lStart.y - spCenter.y, lStart.z - spCenter.z };
	XMFLOAT3 vK = { lEnd.x - lStart.x, lEnd.y - lStart.y, lEnd.z - lStart.z };

	float r = atk->getRadius() + def->getSize(); 

	float a = vK.x * vK.x + vK.y * vK.y + vK.z * vK.z;	    
	float b = 2 * (vJ.x * vK.x + vJ.y * vK.y + vJ.z * vK.z);	 
	float c = (vJ.x * vJ.x + vJ.y * vJ.y + vJ.z * vJ.z) - r * r;   

	float b24ac = b * b - 4 * a * c;

	if (b24ac < 0)
	{
		return false; 
	}

	float sqB24ac = sqrtf(b24ac);	
	float tP = (-b + sqB24ac) / (2 * a);
	float tM = (-b - sqB24ac) / (2 * a);

	if (tM >= 0.0f && tM <= 1.0f)
	{
		resHitPos.x = lStart.x + vK.x * tM;
		resHitPos.y = lStart.y + vK.y * tM;
		resHitPos.z = lStart.z + vK.z * tM;
		return true;
	}
	else if (tP >= 0.0f && tP <= 1.0f)
	{
		resHitPos.x = lStart.x + vK.x * tP;
		resHitPos.y = lStart.y + vK.y * tP;
		resHitPos.z = lStart.z + vK.z * tP;
		return true;
	} 
	else
	{
		if (tM < 0.0f && tP > 1.0f)
		{
			resHitPos.x = lStart.x + vK.x * 0.5f;
			resHitPos.y = lStart.y + vK.y * 0.5f;
			resHitPos.z = lStart.z + vK.z * 0.5f;

			return true;
		}
	}

	return false;
}

bool isHitVS(HitPillar* atk, HitPillar* def, XMFLOAT3& resHitPos)
{
	if (!isHitVS(atk->getCircle(), def->getCircle(), resHitPos))
	{
		return false;
	}

	if (atk->getBottom() > def->getTop() || atk->getTop() < def->getBottom())
		return false;	

	resHitPos.z = resHitPos.y;

	if (atk->getTop() < def->getTop() && atk->getBottom() > def->getBottom())
	{
		resHitPos.y = atk->getCenter();
	}
	else if (def->getTop() < atk->getTop() && def->getBottom() > atk->getBottom())
	{
		resHitPos.y = def->getCenter();
	}
	else
	{
		if (atk->getBottom() > def->getBottom())
		{
			resHitPos.y = def->getTop();
		}
		else
		{
			resHitPos.y = def->getBottom();
		}
	}

	return true;
}

bool isHitVS(HitPillar* atk, HitRayLine* def, XMFLOAT3& resHitPos, bool revCheck)
{
	return false;
}

bool isHitVS(HitRayLine* atk, HitRayLine* def, XMFLOAT3& resHitPos)
{
	return false;
}

void HitManager::flushHitList(std::list<HitStructure*>* p_hitlist)
{
	std::for_each(p_hitlist->begin(), p_hitlist->end(),
		[](HitStructure* hitstr)
		{
			delete(hitstr);	
		}
	);

	p_hitlist->clear();
}

void HitManager::initHitList(UINT hitTypes)
{
	m_numOfHitTypes = hitTypes;

	m_hitArray.clear();

	for (UINT i = 0; i < m_numOfHitTypes; i++)
	{
		m_hitArray.push_back(std::make_unique<list<HitStructure*>>());
	}
}

void HitManager::setHitOrder(UINT atk, UINT def)
{
	if (atk != def && atk < m_numOfHitTypes && def < m_numOfHitTypes)
	{
		m_hitOrders.push_back({ atk, def });
	}
}

void HitManager::refreshHitSystem()	
{
	for_each(m_hitArray.begin(), m_hitArray.end(), [this](unique_ptr<list<HitStructure*>>& u_list) {
		flushHitList(u_list.get());
		});
}

void HitManager::setHitArea(GameComponent* cmp, HitAreaBase* box)
{
	UINT hitType = box->getHitType();
	if (hitType < m_numOfHitTypes)
	{
		HitStructure* hit = new HitStructure(cmp, box);	
		unique_ptr<list<HitStructure*>>& u_list = m_hitArray[hitType];
		u_list->push_back(hit);
	}
}

GameComponent* HitManager::HitStructure::getGameComponent()	
{
	return m_pGameComponent;
}

HitAreaBase* HitManager::HitStructure::getHitArea()		
{
	return m_pHitArea;
}

void HitManager::hitFrameAction()
{
	for_each(m_hitOrders.begin(), m_hitOrders.end(),

		[this](DirectX::XMUINT2& order)
		{
			unique_ptr<list<HitStructure*>>& atk = m_hitArray[order.x];

			for_each(atk->begin(), atk->end(),

				[this, order](HitStructure* p_plHit)
				{
					unique_ptr<list<HitStructure*>>& def = m_hitArray[order.y];

					for_each(def->begin(), def->end(),
						[this, p_plHit](HitStructure* p_enHit)
						{
							HitAreaBase* atkHit = nullptr;
							HitAreaBase* defHit = nullptr;

							if (p_plHit->getHitArea()->getHitShape() <= p_enHit->getHitArea()->getHitShape())
							{
								atkHit = p_plHit->getHitArea();
								defHit = p_enHit->getHitArea();
							}
							else
							{
								atkHit = p_enHit->getHitArea();
								defHit = p_plHit->getHitArea();
							}

							if (isHit(atkHit, defHit))
							{
								p_enHit->getGameComponent()->hitReaction(p_plHit->getGameComponent()->getGameObject(), p_plHit->getHitArea());
								p_plHit->getGameComponent()->hitReaction(p_enHit->getGameComponent()->getGameObject(), p_enHit->getHitArea());
							}
						}
					);
				}
			);
		}

	);
}

bool HitManager::isHit(HitAreaBase* atkHit, HitAreaBase* defHit)
{
	bool hitcheck = false;

	bool revCheck = (atkHit->getHitShape() > defHit->getHitShape());

	if (revCheck)
	{
		HitAreaBase* tempHit = defHit;
		defHit = atkHit;
		atkHit = tempHit;
	}

	switch (atkHit->getHitShape())
	{
	case HIT_SHAPE::HS_QUAD:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_QUAD:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitQuad*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_CIRCLE:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitCircle*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_TRIANGLE:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitTriangle*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_AABB:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitAABB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_OBB:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitOBB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitQuad*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_CIRCLE:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_CIRCLE:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitCircle*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_TRIANGLE:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitTriangle*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_AABB:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitAABB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_OBB:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitOBB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitCircle*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_TRIANGLE:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_TRIANGLE:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitTriangle*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_AABB:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitAABB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_OBB:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitOBB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitTriangle*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos);
			break;
		}
		break;

	case HIT_SHAPE::HS_AABB:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_AABB:
			hitcheck = isHitVS(static_cast<HitAABB*>(atkHit), static_cast<HitAABB*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_OBB:
			hitcheck = isHitVS(static_cast<HitAABB*>(atkHit), static_cast<HitOBB*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitAABB*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitAABB*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitAABB*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_OBB:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_OBB:
			hitcheck = isHitVS(static_cast<HitOBB*>(atkHit), static_cast<HitOBB*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitOBB*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos, revCheck);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitOBB*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitOBB*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_SPHERE:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_SPHERE:
			hitcheck = isHitVS(static_cast<HitSphere*>(atkHit), static_cast<HitSphere*>(defHit), m_lastHitPos);
			break;
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitSphere*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos, revCheck);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitSphere*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_PILLAR:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_PILLAR:
			hitcheck = isHitVS(static_cast<HitPillar*>(atkHit), static_cast<HitPillar*>(defHit), m_lastHitPos);
			break;

		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitPillar*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos, revCheck);
			break;
		}
		break;

	case HIT_SHAPE::HS_LINE:
		switch (defHit->getHitShape())
		{
		case HIT_SHAPE::HS_LINE:
			hitcheck = isHitVS(static_cast<HitRayLine*>(atkHit), static_cast<HitRayLine*>(defHit), m_lastHitPos);
			break;
		}
		break;
	}

	return hitcheck;
}


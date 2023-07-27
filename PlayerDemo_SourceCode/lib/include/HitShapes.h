#pragma once
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

enum class HIT_SHAPE
{
	HS_QUAD,		 
	HS_CIRCLE,		 
	HS_TRIANGLE,	 
	HS_AABB,		 
	HS_OBB,			 
	HS_SPHERE,		 
	HS_PILLAR,		 
	HS_LINE,		 
};

class HitAreaBase
{
protected:
	HIT_SHAPE	m_hitShape;					
	UINT		m_hitType;					
	int			m_hitPower;					

public:
	virtual void setAttackType(UINT area, int pow)	
	{
		m_hitType = area;
		m_hitPower = pow;
	}

	HIT_SHAPE getHitShape()
	{
		return m_hitShape;
	}

	UINT getHitType()						
	{
		return m_hitType;
	}

	int getHitPower()						
	{
		return m_hitPower;
	}
};

class HitQuad : public HitAreaBase
{
private:
	XMFLOAT2		m_hitCenter;				
	XMFLOAT2		m_hitSize;					
	XMFLOAT4		m_hitRect;					

	void updateHitRect();						 

public:
	HitQuad ()
	{
		m_hitShape = HIT_SHAPE::HS_QUAD;
	}

	void setHitSize(float width, float height)	
	{
		m_hitSize.x = width;
		m_hitSize.y = height;
		updateHitRect();						
	}

	void setPosition(float x, float y)			
	{
		m_hitCenter.x = x;
		m_hitCenter.y = y;
		updateHitRect();						
	}

	XMFLOAT4 getHitRect()					
	{
		return m_hitRect;
	}
};

class HitCircle : public HitAreaBase
{
private:
	float radius;							
	XMFLOAT2 center;						
	float doubledR;							

public:
	HitCircle()
	{
		m_hitShape = HIT_SHAPE::HS_CIRCLE;
	}

	void setCenter(float x, float y)		
	{
		center.x = x;
		center.y = y;
	}

	void setRadius(float r)					
	{
		radius = r;
		doubledR = r * r;					
	}

	float getRadius()						Å@
	{
		return radius;
	}

	float getDoubledRadius()				
	{
		return doubledR;
	}

	XMFLOAT2 getCenterPosition()
	{
		return center;
	}
};
class HitTriangle : public HitAreaBase
{
private:
	XMFLOAT3	m_vertexes[3];
	XMVECTOR	m_normal;

	XMVECTOR	m_vectAB;
	XMVECTOR	m_vectAC;

public:
	HitTriangle()
	{
		m_hitShape = HIT_SHAPE::HS_TRIANGLE;
	}

	void setTriangle(const XMFLOAT3& a, const XMFLOAT3& b, const XMFLOAT3& c);

	XMFLOAT3* getVertex()
	{
		return m_vertexes;
	}

	XMVECTOR getNormal()
	{
		return m_normal;
	}

	XMVECTOR getVectAB()
	{
		return m_vectAB;
	}

	XMVECTOR getVectAC()
	{
		return m_vectAC;
	}
};

class HitAABB : public HitAreaBase
{
private:
	XMFLOAT3 m_min;
	XMFLOAT3 m_max;

	XMFLOAT3 m_center;
	XMFLOAT3 m_halfSize;

	void updateAABB();

public:
	HitAABB()
	{
		m_hitShape = HIT_SHAPE::HS_AABB;
	}

	void setAABB(const XMFLOAT3& center, const XMFLOAT3& size);
	void setAABBMinMax(const XMFLOAT3& min, const XMFLOAT3& max);
	void setCenter(const XMFLOAT3& center);
	void setSize(const XMFLOAT3& size);

	XMFLOAT3 getMin()
	{
		return m_min;
	}

	XMFLOAT3 getMax()
	{
		return m_max;
	}

	XMFLOAT3 getCenter()
	{
		return m_center;
	}

	XMFLOAT3 getHalfSize()
	{
		return m_halfSize;
	}
};

class HitOBB : public HitAreaBase
{
public:
	HitOBB()
	{
		m_hitShape = HIT_SHAPE::HS_OBB;
	}
};

class HitSphere : public HitAreaBase
{
private:
	XMFLOAT3	m_center;
	float		m_radius;
	float		m_doubleRadius;

public:
	HitSphere()
	{
		m_hitShape = HIT_SHAPE::HS_SPHERE;
	}

	void setSphereMinMax(const XMFLOAT3& min, const XMFLOAT3& max);

	void setCenter(float x, float y, float z)
	{
		m_center.x = x;
		m_center.y = y;
		m_center.z = z;
	}

	void setRadius(float r)
	{
		m_radius = r;
		m_doubleRadius = r * r;
	}

	XMFLOAT3 getCenter()
	{
		return m_center;
	}

	float getRadius()
	{
		return m_radius;
	}

	float getDoubleRadius()
	{
		return m_doubleRadius;
	}
};

class HitPillar : public HitAreaBase
{
private:
	float m_height;		
	XMFLOAT3 m_center;	
	HitCircle m_circle;	

	float m_top;		
	float m_btm;		

	void updatePillar()
	{
		m_btm = m_center.y - m_height * 0.5f;
		m_top = m_center.y + m_height * 0.5f;
	}
public:
	HitPillar()
	{
		m_hitShape = HIT_SHAPE::HS_PILLAR;
	}

	void setHeight(float height)
	{
		m_height = height;
		updatePillar();
	}

	void setCenter(float x, float y, float z)
	{
		m_center.x = x;
		m_center.y = y;
		m_center.z = z;

		m_circle.setCenter(x, z);	
		updatePillar();
	}

	float setRadius(float radius)
	{
		m_circle.setRadius(radius);
	}

	HitCircle* getCircle()
	{
		return &m_circle;
	}

	float getBottom()
	{
		return m_btm;
	}

	float getTop()
	{
		return m_top;
	}

	float getCenter()
	{
		return m_center.y;
	}
};

class HitRayLine : public HitAreaBase
{
private:
	XMFLOAT3	m_start;
	XMFLOAT3	m_end;
	XMVECTOR	m_normal;
	float		m_length;
	float		m_radius;	

public:
	HitRayLine()
	{
		m_hitShape = HIT_SHAPE::HS_LINE;
	}

	void setLine(const XMFLOAT3& start, const XMFLOAT3& end, float radius);

	XMFLOAT3 getStart()
	{
		return m_start;
	}

	XMFLOAT3 getEnd()
	{
		return m_end;
	}

	XMVECTOR getNormal()
	{
		return m_normal;
	}

	float getLength()
	{
		return m_length;
	}

	float getSize()
	{
		return m_radius;
	}
};
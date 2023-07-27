#pragma once
#include "GameObject.h"

class PlayerComponent : public GameComponent
{
private:
	int m_playerHitpoint;		Å@
	int m_hitDamage;			

	HitQuad bodyHit;			
public:
	void initAction() override;
	bool frameAction() override;
	void finishAction() override;

	void hitReaction(GameObject* obj, HitAreaBase* hit) override;	Å@
};

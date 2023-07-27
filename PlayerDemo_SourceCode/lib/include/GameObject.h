#pragma once
#include <DirectXMath.h>
#include <list>
#include "HitManager.h"
#include "HitShapes.h"
#include "CharacterData.h"
#include "PipeLineManager.h"

using namespace std;

class GameObject;								
class HitQuad;								
class GameComponent								
{
private:
	bool m_activeFlg;
	GameObject* m_gObject;

	virtual void initAction() = 0;		

public:
	void initFromGameObject(GameObject* objData)
	{
		m_gObject = objData;
		m_activeFlg = true;
		initAction();
	}

	bool isActive()
	{
		return m_activeFlg;
	}

	void setActive(bool flg)
	{
		m_activeFlg = flg;
	}

	virtual bool frameAction() = 0;		Å@
	virtual void finishAction() = 0;	

	virtual void hitReaction(GameObject* targetGo, HitAreaBase * hit) {};
	GameObject* getGameObject() { return m_gObject; }
};

class GameObject								
{
protected:
	bool m_enableFlg;

	unique_ptr<CharacterData> characterData;	
	list<GameComponent*> components;			

public:
	GameObject(CharacterData* cData)
	{
		m_enableFlg = true;
		characterData.reset(cData);
	}

	CharacterData* getCharacterData()			
	{
		return characterData.get();
	}

	void addComponent(GameComponent* com)		
	{
		com->initFromGameObject(this);					
		components.push_back(com);
	}

	list<GameComponent*>& getComponents()
	{
		return components;
	}

	void removeComponent(GameComponent* com)	
	{
		com->finishAction();					
		components.remove(com);
		delete(com);							
	}

	bool isEnable()
	{
		return m_enableFlg;
	}

	void setEnable(bool flg)
	{
		m_enableFlg = flg;
	}

	virtual void cleanupGameObject();			Å@
	virtual bool action();						 
	virtual void init();						

};

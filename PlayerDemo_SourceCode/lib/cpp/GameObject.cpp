#include "GameObject.h"
#include <MyAccessHub.h>
void GameObject::init()
{
}

bool GameObject::action()	
{
	list<GameComponent*> deleteComponents;	
	if (!isEnable()) return true;			

	for (auto comp : components) 
	{
		if (comp->isActive() && !comp->frameAction())	
		{
			deleteComponents.push_back(comp);
		}
	}

	if (!deleteComponents.empty())
	{
		for (auto deleteComp : deleteComponents)			 
		{
			removeComponent(deleteComp);					
		}

		deleteComponents.clear();		

		if (components.empty())			
		{
			return false;				
		}
	}

	return true;						
}

void GameObject::cleanupGameObject()							
{
	for (auto comp : components)
	{
		comp->finishAction();	
		delete(comp);			
	}

	components.clear();											
}

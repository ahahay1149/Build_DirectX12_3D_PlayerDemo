#include "PlayerObject.h"
#include "PlayerComponent.h"
#include "PlayerWeapon.h"
#include "MyGameEngine.h"
#include <DirectXMath.h>

using namespace DirectX;

PlayerObject::PlayerObject() : GameObject(new SpriteCharacter())
{
}

void PlayerObject::init()
{
	addComponent(new PlayerComponent());			
	addComponent(new PlayerWeaponComponent());		
}

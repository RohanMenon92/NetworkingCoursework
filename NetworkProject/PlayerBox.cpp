#include "PlayerBox.hpp"

//void PlayerBox::AimAtPosition(sf::Vector2f aimPos) {
//	sf::Vector2f curPos = ;
//	sf::Vector2i position = sf::Mouse::getPosition(win);
//
//	// now we have both the sprite position and the cursor
//	// position lets do the calculation so our sprite will
//	// face the position of the mouse
//	const float PI = 3.14159265;
//
//	float dx = curPos.x - position.x;
//	float dy = curPos.y - position.y;
//
//	float rotation = (atan2(dy, dx)) * 180 / PI;
//
//	this.setRotation(rotation + 180);
//}

void PlayerBox::OnAttackPressed()
{
	isAttacking = true;
}

void PlayerBox::OnAttackUnpressed()
{
	isAttacking = false;
}

void PlayerBox::OnBlockPressed()
{
	isBlocking = true;
}

void PlayerBox::OnBlockUnpressed()
{
	isBlocking = false;
}

void PlayerBox::OnForwardPressed()
{
	isMovingForward = true;
}

void PlayerBox::OnForwardUnpressed()
{
	isMovingForward = false;
}

void PlayerBox::Update(sf::Time dt)
{

}

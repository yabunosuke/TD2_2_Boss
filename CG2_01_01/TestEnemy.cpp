#include "TestEnemy.h"

void TestEnemy::Initialize()
{
	move = { 0,0,0 };
}


void TestEnemy::Update()
{
	r = hp * 1.5f;
	//移動量初期化
	if (move.Length() > 0.1f) {
		move = move * 0.95f;
	}
	else {
		move = { 0,0,0 };
	}

	pos += move;

}

void TestEnemy::Reflection()
{
	object->SetScale(scale);
	object->SetPos(pos);
	object->Update();

}

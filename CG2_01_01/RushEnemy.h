#pragma once
#include "BaseEnemy.h"
class RushEnemy : public BaseEnemy
{
public:
	RushEnemy(Vector3 pos, float hp, float r) :
		BaseEnemy(pos, hp, r, ModelManager::ModelName::TestEnemy) {

	}

	void Initialize() override;
	void Update() override;

private:
	const size_t rushInterval = 60; //突進する間隔
	size_t flame; //フレーム

};


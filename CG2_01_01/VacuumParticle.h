#pragma once
#include "BaseParticle.h"
#include "Vector3.h"

class VacuumParticle : public BaseParticle
{
public:
	VacuumParticle();
	~VacuumParticle();

public: // メンバ関数
	void Initialize(Vector3* normal);
	void Update(const bool& isCreate, const Vector3& offset, const float& range);

public: // メンバ変数
	Vector3* normal;

};

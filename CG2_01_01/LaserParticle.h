#pragma once
#include "BaseParticle.h"
#include "Vector3.h"

class LaserParticle : public BaseParticle
{
public:
	LaserParticle();
	~LaserParticle();

public: // メンバ関数
	void Initialize(Vector3* normal);
	void Update(const bool& isCreate, const Vector3& offset = Vector3()) override;

public: // メンバ変数
	Vector3* normal;

};

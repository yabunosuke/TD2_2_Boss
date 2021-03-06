#pragma once
#include "Vector3.h"
#include "ModelManager.h"
#include "Object3d.h"


class GameObjCommon
{
public:
	GameObjCommon(Vector3 startPos, float hp, float N, float e, float r, ModelManager::ModelName modelName,string tag = "") :
		pos(startPos),
		move( 0,0,0 ),
		rotate(0, 0, 0),
		hp(hp),
		maxHp(hp),
		r(r),
		scale(Vector3{ hp,hp,hp }),
		isAlive(true),
		isDelete(false),
		N(N),
		e(e),
		damage(1.0f),
		Tag(tag)
	{
		object = nullptr;
		object = Object3d::Create();
		object->SetModel(ModelManager::GetIns()->GetModel(modelName));
		object->SetPos(pos);
		object->SetRotation(rotate);
		object->SetScale(scale);
		object->Update();
		Initialize();
	}

	~GameObjCommon()
	{
		delete object;
		object = nullptr;
	}

	virtual void Initialize();
	virtual void Update();
	virtual void Reflection();
	virtual void Draw() const;

	virtual void Damage(float damage);
	virtual void Dead();

	static vector<GameObjCommon *> enemys;

public:
	Vector3 pos;	//座標
	Vector3 move;	//移動量
	Vector3 scale;	//サイズ
	float N;		//質量
	float e;		//反発係数
	float r;		//半径
	bool isAlive;	//生存フラグ
	bool isDelete;	//生存フラグ
	float hp;		//体力（サイズ）
	float damage;	//接触時に与えるダメージ

	string Tag;
protected:

	Vector3 rotate;			//エネミーの向きベクトル
	float angle = 0.0f;		//エネミーのy軸角度
	Object3d *object;
	float maxHp;	//体力最大

};


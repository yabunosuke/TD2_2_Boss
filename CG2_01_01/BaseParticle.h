#pragma once
#include "Particle/ParticleManager.h"
#include "Vector3.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <forward_list>

class BaseParticle
{
public:
	BaseParticle();
	~BaseParticle();

public: // 仮想関数
	// 初期化処理
	virtual void Initialize() {}
	// 更新処理
	virtual void Update() {}
	virtual void Update(const bool& isCreate, const Vector3& offset = Vector3()) {}
	// 描画処理
	virtual void Draw() const;
	// 終了処理
	virtual void Finalize();

public: // 静的メンバ関数
	static void StaticUpdate();

protected: // 静的メンバ変数
	static std::forward_list<ParticleManager*> UpdateCount; //1フレームの更新処理の回数
private:
	static std::vector<std::string> texFilepath; //テクスチャの名前群

public:
	// 演出中のパーティクルの数
	size_t GetParticleNum();
protected: // メンバ関数
	// ParticleManagerの生成処理
	void CreateManager(std::string texFilepath);

public: // メンバ変数
	int lifeTime; //生存時間

	Vector3 pos;   //座標
	Vector3 speed; //速度
	Vector3 accel; //加速度

	Vector3 startAngle; //開始時の角度
	Vector3 endAngle;   //終了時の角度

	float startScale; //開始時スケール
	float endScale;   //終了時スケール

	DirectX::XMFLOAT4 startColor; //開始時の色
	DirectX::XMFLOAT4 endColor;   //終了時の色

	float rangeScale;
protected:
	ParticleManager* manager;

};

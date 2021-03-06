#include "TestScene.h"
#include "Object3d.h"
#include "Collision.h"
#include "Input.h"
#include <DirectXMath.h>
#include "LoadStage.h"
#include "Easing.h"
#include "EnemyBomb.h"

#include "ImageManager.h"
#include "AudioManager.h"

TestScene::TestScene(IoChangedListener *impl)
	: AbstractScene(impl)
{
	stage.Initialize();
	player.Initialize();
	reticle.Initialize();
	testParticle.Initialize();
	deathParticle.Initialize();

	logo = Sprite::Create(ImageManager::Logo);


	//enemys.push_back(new TestEnemy({ 0,0,500 }, 7 ,				10.0f,0.5f,	20.0f));
	LoadStage::LoadStageEnemy("./Resources/stageCSV/stage1Enemy.csv", GameObjCommon::enemys, &player);
	ui.Initialize(&player);
}

void TestScene::Initialize()
{
	//BGMを止めてから再生
	AudioManager::SoundPlayWave(AudioManager::GameBgm, true);

	Object3d::SetCamPos(Vector3(-3360.0f, 600.0f, 0.0f));
	Object3d::SetCamTarget(Vector3(-3360.0f, 0.0f, 50.0f));
	stage.Initialize();
	player.Initialize();
	reticle.Initialize();
	testParticle.Initialize();
	deathParticle.Initialize();
	EnemyBomb::StaticInitialize(&player);
	//敵をすべて初期化
	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		GameObjCommon::enemys[i]->Initialize();
	}
	shakePos = { 0,0,0 };
	isShake = false;
}

void TestScene::Finalize()
{
	testParticle.Finalize();
	deathParticle.Finalize();
}

void TestScene::Update()
{
	static int gameCounter = 0;	//経過フレームのカウンター
	gameCounter++;

	ui.Update();
	stage.Update();

	//敵をすべて更新
	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		GameObjCommon::enemys[i]->Update();
		deathParticle.Update(GameObjCommon::enemys[i]->isAlive == false, GameObjCommon::enemys[i]->pos);
	}
	BaseParticle::StaticUpdate();
	//敵の削除
	for (int i = GameObjCommon::enemys.size() - 1; i >= 0; i--) {
		if (GameObjCommon::enemys[i]->isDelete) {

			delete GameObjCommon::enemys[i];//newはdeleteしてから消す
			GameObjCommon::enemys.erase(GameObjCommon::enemys.begin() + i);
		}
	}

	//ロックオン
	if (Input::LockOn()) {
		player.LockOn(GameObjCommon::enemys);
	}
	else {
		player.isLockOn = false;
	}
	player.Update();
	testParticle.Update(Input::Test());
	
	//当たり判定
	HitCollision();


	//移動を適用
	player.Reflection();
	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		GameObjCommon::enemys[i]->Reflection();
	}
	if (player.isLockOn) {
		reticle.pos = player.LockOnTarget->pos;
		reticle.scale = player.LockOnScale;
		reticle.Update();
	}
	UpdateCamera();
}

void TestScene::Draw() const
{
	stage.Draw();
	player.Draw();
	//敵をすべて描画
	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		GameObjCommon::enemys[i]->Draw();
	}
	testParticle.Draw();
	deathParticle.Draw();
	if(player.isLockOn){
		reticle.Draw();
	}
	ui.Draw();
}

void TestScene::HitCollision()
{
	HitWall();

	//プレイヤーとエネミー
	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		if (!Collision::IsPredictCollisionBall(player.pos, player.move, player.r * 2, GameObjCommon::enemys[i]->pos, GameObjCommon::enemys[i]->move, GameObjCommon::enemys[i]->r * 2)) {
			continue;
		}
		float hitTime = 0.0f;
		Vector3 collisionPos;
		Vector3 collisionPosA;
		Vector3 collisionPosB;
		if (Collision::sphereSwept(player.pos, player.move, player.r, GameObjCommon::enemys[i]->pos, GameObjCommon::enemys[i]->move, GameObjCommon::enemys[i]->r,
			hitTime,collisionPos,&collisionPosA,&collisionPosB)) {
			isShake = true;
			//どちらがダメージを負うか
			if (player.move.Length() < GameObjCommon::enemys[i]->move.Length()) {
				player.Damage(GameObjCommon::enemys[i]->damage);
				shakeRange = GameObjCommon::enemys[i]->damage;
			}
			else {
				GameObjCommon::enemys[i]->Damage(player.damage);
				shakeRange = player.damage;
			}
			deathParticle.Update(true, collisionPos);

			//float PtoE = Vector3(player.pos - GameObjCommon::enemys[i]->pos).Length();
			//float tR = player.r + GameObjCommon::enemys[i]->r;
			AudioManager::SoundPlayWave(AudioManager::Hit, false);
			Bound(hitTime, player, *GameObjCommon::enemys[i],&collisionPosA,&collisionPosB,collisionPos);
			player.Hit();
		}
	}

	//エネミー同士
	for (int l = 0; l < GameObjCommon::enemys.size(); l++) {
		for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
			if (l <= i) {
				continue;
			}
			if (!Collision::IsPredictCollisionBall(GameObjCommon::enemys[l]->pos, GameObjCommon::enemys[l]->move, GameObjCommon::enemys[l]->r * 2, GameObjCommon::enemys[i]->pos, GameObjCommon::enemys[i]->move, GameObjCommon::enemys[i]->r * 2)) {
				//Vector3 check = enemys[i]->pos -player.pos;
				//float chackL = check.Length();
				continue;
			}

			float hitTime = 0.0f;
			Vector3 collisionPos;
			Vector3 collisionPosA;
			Vector3 collisionPosB;
			if (Collision::sphereSwept(GameObjCommon::enemys[l]->pos, GameObjCommon::enemys[l]->move, GameObjCommon::enemys[l]->r, GameObjCommon::enemys[i]->pos, GameObjCommon::enemys[i]->move, GameObjCommon::enemys[i]->r,
				hitTime, collisionPos, &collisionPosA, &collisionPosB)) {
				isShake = true;
				shakeRange = (GameObjCommon::enemys[l]->damage + GameObjCommon::enemys[i]->damage) / 2;
				GameObjCommon::enemys[i]->Damage(GameObjCommon::enemys[l]->damage);
				GameObjCommon::enemys[l]->Damage(GameObjCommon::enemys[i]->damage);
				//衝突後処理
				AudioManager::SoundPlayWave(AudioManager::Hit, false);
				Bound(hitTime, *GameObjCommon::enemys[l], *GameObjCommon::enemys[i], &collisionPosA, &collisionPosB, collisionPos);
			}
		}
	}
}

void TestScene::HitWall()
{
	static const float upWall = 790.0f;
	static const float downWall = -790.0f;
	static const float leftWall = -3990.0f;
	static const float rightWall = 3990.0f;
	Vector3 hitWallPlayer = player.pos;
	bool playerHit = false;
	Vector3 afterMove = player.move;
	//上
	if (player.pos.z + player.move.z + player.r >= upWall) {
		afterMove.z = -player.move.z - (player.pos.z -(upWall - player.r));
		player.pos.z = upWall - player.r;
		player.move = afterMove;
		hitWallPlayer.z = upWall;
		playerHit = true;
	}
	//下
	if (player.pos.z + player.move.z - player.r <= downWall) {
		afterMove.z = -player.move.z - (player.pos.z - (downWall + player.r));
		player.pos.z = downWall + player.r;
		player.move = afterMove;
		hitWallPlayer.z = downWall;
		playerHit = true;

	}
	//右
	if (player.pos.x + player.move.x + player.r >= rightWall) {
		afterMove.x = -player.move.x - (player.pos.x - (rightWall - player.r));
		player.pos.x = rightWall - player.r;
		player.move = afterMove;
		hitWallPlayer.x = rightWall;
		playerHit = true;
	}
	//左
	if (player.pos.x + player.move.x - player.r <= leftWall) {
		afterMove.x = -player.move.x - (player.pos.x - (leftWall + player.r));
		player.pos.x = leftWall + player.r;
		player.move = afterMove;
		hitWallPlayer.x = leftWall;
		playerHit = true;
	}

	if (playerHit && player.move.Length() >= 8.0f) {
		deathParticle.Update(true, hitWallPlayer);
		AudioManager::SoundPlayWave(AudioManager::HitWall, false);
	}

	for (int i = 0; i < GameObjCommon::enemys.size(); i++) {
		bool enemyHit = false;
		Vector3 hitWallEnemy = GameObjCommon::enemys[i]->pos;
		Vector3 afterMoveEnemy = GameObjCommon::enemys[i]->move;

		if (GameObjCommon::enemys[i]->pos.z + GameObjCommon::enemys[i]->move.z + GameObjCommon::enemys[i]->r >= upWall) {
			afterMoveEnemy.z = GameObjCommon::enemys[i]->move.z - (upWall - GameObjCommon::enemys[i]->pos.z);
			GameObjCommon::enemys[i]->pos.z = upWall - GameObjCommon::enemys[i]->r;
			GameObjCommon::enemys[i]->move = afterMoveEnemy;
			hitWallEnemy.z = upWall;
			enemyHit = true;
		}

		if (GameObjCommon::enemys[i]->pos.z + GameObjCommon::enemys[i]->move.z - GameObjCommon::enemys[i]->r <= downWall) {
			afterMove.z = -GameObjCommon::enemys[i]->move.z - (GameObjCommon::enemys[i]->pos.z - (downWall + GameObjCommon::enemys[i]->r));
			GameObjCommon::enemys[i]->pos.z = downWall + GameObjCommon::enemys[i]->r;
			GameObjCommon::enemys[i]->move = afterMove;
			hitWallEnemy.z = downWall;
			enemyHit = true;
		}
		//右
		if (GameObjCommon::enemys[i]->pos.x + GameObjCommon::enemys[i]->move.x + GameObjCommon::enemys[i]->r >= rightWall) {
			afterMove.x = -GameObjCommon::enemys[i]->move.x - (GameObjCommon::enemys[i]->pos.x - (rightWall - GameObjCommon::enemys[i]->r));
			GameObjCommon::enemys[i]->pos.x = rightWall - GameObjCommon::enemys[i]->r;
			GameObjCommon::enemys[i]->move = afterMove;
			hitWallEnemy.x = rightWall;
			enemyHit = true;
		}
		//左
		if (GameObjCommon::enemys[i]->pos.x + GameObjCommon::enemys[i]->move.x - GameObjCommon::enemys[i]->r <= leftWall) {
			afterMove.x = -GameObjCommon::enemys[i]->move.x - (GameObjCommon::enemys[i]->pos.x - (leftWall + GameObjCommon::enemys[i]->r));
			GameObjCommon::enemys[i]->pos.x = leftWall + GameObjCommon::enemys[i]->r;
			GameObjCommon::enemys[i]->move = afterMove;
			hitWallEnemy.x = leftWall;
			enemyHit = true;
		}
		else {
			enemyHit = false;
		}


		if (enemyHit && GameObjCommon::enemys[i]->move.Length() >= 8.0f) {
			deathParticle.Update(true, hitWallEnemy);
			AudioManager::SoundPlayWave(AudioManager::HitWall, false);
		}
	}
}

void TestScene::Bound(float hitTime, GameObjCommon &a, GameObjCommon &b, Vector3 *collisionA, Vector3 *collisionB, const Vector3 &collisionPos)
{
	/*if (Vector3(*collisionA - *collisionB).Length() < a.r + b.r) {
		a.pos = collisionPos + -Direction * a.r;
		b.pos = collisionPos + Direction * b.r;
		return;
	}*/
	//位置決定
	//合計質量
	float TotalN = a.N + b.N;
	//反発率
	float RefRate = (1 + a.e*b.e);
	//衝突軸
	Vector3 Direction = *collisionB - *collisionA;
	//ノーマライズ
	Direction.Normalize();
	//移動量の内積
	Vector3 moveVec = (a.move - b.move);
	float Dot = moveVec.VDot(Direction);
	//定数ベクトル
	Vector3 ConstVec = Direction * RefRate * Dot / TotalN;
	
	//衝突後の移動量
	a.move = ConstVec * -b.N + a.move;
	b.move = ConstVec * a.N + b.move;

	//衝突後位置
	a.pos = (a.move * hitTime) + *collisionA;
	b.pos = (b.move * hitTime) + *collisionB;

	Vector3 AtoB = b.pos- a.pos;
	if (AtoB.Length() < a.r + b.r) {
		b.pos = a.pos + Direction * (a.r + b.r);
	}

}

void TestScene::UpdateCamera()
{
	Vector3 CamPos = {
		(float)Ease(In,Linear,0.1f,Object3d::GetCamPos().x,player.pos.x),
		600,
		(float)Ease(In,Linear,0.1f,Object3d::GetCamPos().z,player.pos.z),
	};
	Vector3 CamTarget = CamPos;
	CamTarget.y = 0.0f;
	CamTarget.z +=50;
	//カメラの揺れ
	Shake(shakeRange);

	Object3d::SetCamPos(CamPos+shakePos);
	Object3d::SetCamTarget(CamTarget +shakePos);
	//ParticleManager::SetEye(CamPos +shakePos);
	//ParticleManager::SetTarget(CamTarget +shakePos);

	Object3d::UpdateViewMatrix();
}

void TestScene::Shake(float damage)
{
	if (!isShake)	return;

	static float shakeCounter = 1.0f;
	shakeCounter-= 0.05f;
	float shakeRange = abs(damage * 10);
	shakePos = {
		(rand() % (int)(shakeRange + 1) - (shakeRange + 1)/2) * shakeCounter,
		(rand() % (int)(shakeRange + 1) - (shakeRange + 1)/2) * shakeCounter,
		(rand() % (int)(shakeRange + 1) - (shakeRange + 1)/2) * shakeCounter,
	};
	if (shakeCounter <= 0.0f) {
		shakeCounter = 1.0f;
		isShake = false;
	}
}

#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <DirectXTex.h>
using namespace DirectX;

#include "Vector3.h"
//ComPtr
#include <wrl.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"

using namespace std;

class Object3d
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;	//Microsoft::WRL::を省略
public:	 //サブクラス
	//パイプライン構造体
	struct PipelineSet {
		ComPtr<ID3D12PipelineState> pipelinestate;
		ComPtr<ID3D12RootSignature> rootsignature;
	};

	//オブジェクトの共通データ
	static const int spriteSRVCount = 512;
	struct ObjectCommon {
		ID3D12Device *dev;			//デバイス
		ID3D12GraphicsCommandList *cmdList;		//コマンドリスト

		PipelineSet pipelineSet;				//パイプラインセット
		XMMATRIX matView;			//ビュー行列
		XMMATRIX matProjection{};	//射影行列

		XMFLOAT3 eye;				//視点座標
		XMFLOAT3 target;			//注視点座標
		XMFLOAT3 up;				//上方向ベクトル
	};

	//定数バッファ用データ構造体B0
	struct ConstBufferDataB0 {
		XMMATRIX mat;	//3D変換行列
		XMMATRIX matBillboard;	// ビルボード行列

	};
	//定数バッファ用データ構造体B1
	struct ConstBufferDataB1 {
		XMFLOAT3 ambient;	//アンビエイト係数
		float pad1;			//パディング
		XMFLOAT3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		XMFLOAT3 specular;	//スペキュラー係数
		float alpha;		//パディング
	};

	enum BLENDMODE {
		NORMAL,
		ADD,
		SUB,
	};

public:	//静的メンバ関数
	//静的初期化
	static bool StaticInitialize(ID3D12Device *dev, int window_width, int window_height);
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList *cmdList);
	//描画後処理
	static void PostDraw();
	//オブジェクト初期化処理
	static Object3d *Create(bool isBillboard = false);

	//ビュー行列更新
	static void UpdateViewMatrix();

	//ブレンドモード変更
	static void BlendMode(BLENDMODE blendM);

	static Vector3 GetCamPos() { return objectCommon.eye; }
	static void SetCamPos(Vector3 pos) { objectCommon.eye = pos; }
	static void SetCamTarget(Vector3 target) { objectCommon.target = target; }

private:	//静的メンバ関数
	//カメラ生成
	static void InitializeCamera(int window_width, int window_height);
	//3Dオブジェクト用パイプライン
	static bool InitializeGraphicsPipeline();

public:	//静的メンバ変数
	static ObjectCommon objectCommon;				//よく使う物
	//ビルボード行列
	static XMMATRIX matBillboard;
	//Y軸周りビルボード行列
	static XMMATRIX matBillboardY;

public:		//メンバ関数
	//初期化
	bool Initialize();
	//モデルセット
	void SetModel(Model *model);
	//毎フレーム更新処理
	void Update();
	//描画
	void Draw();

	void SetPos(Vector3 nextPos) { position = nextPos; }
	void SetScale(Vector3 nextScale) { scale = nextScale; }
	void SetRotation(Vector3 nextRotation) { rotation = nextRotation; }
	void SetParent(Object3d* nextParent) { parent = nextParent; }
	void SetBillboard(bool isBillboard) { Object3d::isBillboard = isBillboard; }
	Vector3 GetPos() { return position; }
	Vector3 GetScale() { return scale; }
	Vector3 GetRotation() { return rotation; }
	Object3d* GetParent() { return parent; }

private:	//メンバ変数
	XMFLOAT4 color = { 1,1,1,1 };	//色
	Vector3 scale = { 1,1,1 };		//スケール	
	Vector3 rotation = { 0,0,0 };	//回転
	Vector3 position = { 0,0,0 };	//座標
	bool isBillboard;

	Model *objectModel = nullptr;					//モデルポインター
	ComPtr<ID3D12Resource> constBuffB0;		//定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;		//定数バッファ
	XMMATRIX matWorld;						//ワールド変換行列
	UINT texNumber = 0;						//テクスチャ番号

	Object3d *parent = nullptr;			//親オブジェクトへのポインタ

};
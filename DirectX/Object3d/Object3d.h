#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#pragma comment(lib, "d3dcompiler.lib")
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
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;	//Microsoft::WRL::���ȗ�
public:	 //�T�u�N���X
	//�p�C�v���C���\����
	struct PipelineSet {
		ComPtr<ID3D12PipelineState> pipelinestate;
		ComPtr<ID3D12RootSignature> rootsignature;
	};

	//�I�u�W�F�N�g�̋��ʃf�[�^
	static const int spriteSRVCount = 512;
	struct ObjectCommon {
		ID3D12Device *dev;			//�f�o�C�X
		ID3D12GraphicsCommandList *cmdList;		//�R�}���h���X�g

		PipelineSet pipelineSet;				//�p�C�v���C���Z�b�g
		XMMATRIX matView;			//�r���[�s��
		XMMATRIX matProjection{};	//�ˉe�s��

		XMFLOAT3 eye;				//���_���W
		XMFLOAT3 target;			//�����_���W
		XMFLOAT3 up;				//������x�N�g��
	};

	//�萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0 {
		//XMFLOAT4 color; //�F(RGBA)
		XMMATRIX mat;	//3D�ϊ��s��
	};
	//�萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1 {
		XMFLOAT3 ambient;	//�A���r�G�C�g�W��
		float pad1;			//�p�f�B���O
		XMFLOAT3 diffuse;	//�f�B�t���[�Y�W��
		float pad2;			//�p�f�B���O
		XMFLOAT3 specular;	//�X�y�L�����[�W��
		float alpha;		//�p�f�B���O
	};

	enum BLENDMODE {
		NORMAL,
		ADD,
		SUB,
	};

public:	//�ÓI�����o�֐�
	//�ÓI������
	static bool StaticInitialize(ID3D12Device *dev, int window_width, int window_height);
	//�`��O����
	static void PreDraw(ID3D12GraphicsCommandList *cmdList);
	//�`��㏈��
	static void PostDraw();
	//�I�u�W�F�N�g����������
	static Object3d *Create();

	//�r���[�s��X�V
	static void UpdateViewMatrix();

	//�u�����h���[�h�ύX
	static void BlendMode(BLENDMODE blendM);

private:	//�ÓI�����o�֐�
	//�J��������
	static void InitializeCamera(int window_width, int window_height);
	//3D�I�u�W�F�N�g�p�p�C�v���C��
	static bool InitializeGraphicsPipeline();
	//���f������
	static void CreateModel();

public:	//�ÓI�����o�ϐ�
	static ObjectCommon objectCommon;				//�悭�g����

public:		//�����o�֐�
	//������
	bool Initialize();
	//���f���Z�b�g
	void SetModel(Model *model);
	//���t���[���X�V����
	void Update();
	//�`��
	void Draw();


public:		//���������̔������郁���o�ϐ�
	XMFLOAT4 color = { 1,1,1,1 };	//�F
	Vector3 scale = { 1,1,1 };		//�X�P�[��	
	Vector3 rotation = { 1,0,0 };	//��]
	Vector3 position = { 0,0,0 };	//���W

private:	//�����o�ϐ�
	Model *objectModel = nullptr;					//���f���|�C���^�[
	ComPtr<ID3D12Resource> constBuffB0;		//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1;		//�萔�o�b�t�@
	XMMATRIX matWorld;						//���[���h�ϊ��s��
	UINT texNumber = 0;						//�e�N�X�`���ԍ�

	Object3d *parent = nullptr;			//�e�I�u�W�F�N�g�ւ̃|�C���^

};
#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"

#include "object/Bullet.h"
#include "object/Enemy.h"
#include "object/EnemyBullet.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(GameScene* gameScene);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Attack();

	float Angle(float angle);

	void Reticle3D();

	void HomeOnColision();

	int CheckAlive(Enemy enemy[]);

	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

private: // メンバ変数
	GameScene* gameScene_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	/// 
	uint32_t textureHandle_[10];

	Sprite* title = nullptr;
	Sprite* tutoliar = nullptr;
	Sprite* gameWin = nullptr;
	Sprite* gameOver = nullptr;
	Model* model_ = nullptr;
	Model* modelWall_ = nullptr;

	WorldTransform objHome_;
	int objHomeR = 4;			// 中心ブロック半径
	int homeLife = 20;
	int isDamage = 0;
	int damTimer = 0;

	WorldTransform floor_;
	WorldTransform playerWTF[3];
	ViewProjection viewProjection_;

	WorldTransform PopPos_[5];
	WorldTransform wall_;

	DebugCamera* debugCamera_ = nullptr;

	//カメラ上方向の角度
	float viewAngle = 0.0f;
	float focalLengs = 50;

	//弾
	//Bullet* bullet_[50];
	//弾
	std::list<std::unique_ptr<Bullet>> bullets_;
	Vector3 pos;
	Vector3 moveBul;
	Vector3 velo;
	Vector3 ai;

	float KEyeSpeed = 0.0f;
	float addspeed = 0.0f;


	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;
	float kDistancePlayerTo3DReticle = -80.0f;
	Vector3 ret3DPos;
	Vector3 myPos;
	Vector3 resultRet;
	// 敵
	Enemy enemys[50];
	//弾
	std::list<std::unique_ptr<EnemyBullet>> eneBullets_;
	// 敵
	int popTime;
	int coolTime;
	int killCounter;
	int popCount = 0;
	int wave = 0;
	int waitTimer = 250;

	// シーン
	int scene = 0;

	int a;
	float kCharacterSpeed = 0.1f;
	Vector3 posA, posB;
	Vector3 move = { 0,0,0 };

	int aliveNum = 0;

	int cameraState = 0;
	// 時間計測に必要なデータ
	long long startCount = 0;
	long long nowCount = 0;
	long long elapsedCount = 0;
	float elapsedTime=0;
	

	// 補間で使うデータ
	// start -> end を 5[ｓ] で完了させる
	Vector3 p0,p1,p2,p3;
	//Vector3 p0, p1, p2, p3;
	float maxTime = 50.0f;
	float timeRate;
	float maxTimeRate;

	// ゲームループで使う変数の宣言

	Vector3 BJEye,BJTarget;
};

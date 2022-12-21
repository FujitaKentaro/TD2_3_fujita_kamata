#include "Enemy.h"
#include <cassert>
#include "GameScene.h"

Enemy::Enemy() {
	worldTransForm.Initialize();
	worldTransForm.translation_ = { 100,0,100 };
	isDead = true;
	YTmp = { 0,1,0 };
	speed = 0.0004f;
	worldTransForm.matWorld_ = Affin::matUnit();
}

Enemy::~Enemy() {}

void Enemy::CalcVec(Vector3 obj) {
	//正面仮ベクトル
	enemyTmp = obj - worldTransForm.translation_;
	enemyTmp.normalize();
	//右ベクトル
	enemyRight = YTmp.cross(enemyTmp);
	enemyRight.normalize();
	//正面ベクトル
	enemyFront = enemyRight.cross(YTmp);
	enemyFront.normalize();
}

void Enemy::Update(Model* model_, Vector3 obj) {
	assert(model_);
	////デスフラグの立った弾を削除
	//bullets_.remove_if(
	//	[](std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });


	//ベクトル計算
	CalcVec(obj);
	if (coolTime == 0) {
		Attack(model_);
	}
	else {
		coolTime--;
	}
	////弾描画
	//for (std::unique_ptr<EnemyBullet>& Ebullet : bullets_) {
	//	if (Ebullet->IsDead()==false) {
	//		Ebullet->Update();
	//	}
	//}

	//行列計算

	worldTransForm.matWorld_ = Affin::matWorld(
		worldTransForm.translation_,
		worldTransForm.rotation_,
		worldTransForm.scale_);

	if (isDead == false) {
		time++;
		if (time == 6) {
			speed += 0.0001f;
			time = 0;
		}
		worldTransForm.translation_ += enemyFront * 0.1;
	}
	else if (isDead == true) {
		speed = 0.0008f;
		time = 0;
	}

	switch (seed_)
	{
	case 1:
		if (worldTransForm.translation_.x < 90) {
			worldTransForm.translation_.x = 90;
		}
		break;
	case 2:
		if (worldTransForm.translation_.x > -90) {
			worldTransForm.translation_.x = -90;
		}
		break;
	case 3:
		if (worldTransForm.translation_.z < 90) {
			worldTransForm.translation_.z = 90;
		}
		break;
	case 4:
		if (worldTransForm.translation_.z > -90) {
			worldTransForm.translation_.z = -90;
		}
		break;

	default:
		break;
	}



	//結果を反映
	worldTransForm.TransferMatrix();
	
	//Hit();
}

void Enemy::Draw(ViewProjection& viewProjection_, uint32_t textureHandle_) {
	
	////弾描画
	//for (std::unique_ptr<EnemyBullet>& Ebullet : bullets_) {
	//	Ebullet->Draw(viewProjection_, textureHandle_);
	//}
}

void Enemy::Pop(Vector3 WorTrans, int seed) {
	worldTransForm.Initialize();
	worldTransForm.translation_ = { 100,0,100 };
	YTmp = { 0,1,0 };
	worldTransForm.matWorld_ = Affin::matUnit();

	if (isDead == true) {
		isDead = false;
		seed_ = seed;

		//乱数生成装置
		std::random_device seed_gen;
		std::mt19937_64 engine(seed_gen());
		std::uniform_real_distribution<float>dist(20.0f, 50.0f);
		std::uniform_real_distribution<float>dist2(-1.0f, 1.0f);

		//乱数
		float value = dist(engine) * dist2(engine);

		//
		worldTransForm.translation_ = { WorTrans.x + value,WorTrans.y, WorTrans.z + value };
	}
}

void Enemy::Hit() {
	if (worldTransForm.translation_.x < 0.5 && worldTransForm.translation_.x > -0.5) {
		if (worldTransForm.translation_.z < 0.5 && worldTransForm.translation_.z > -0.5) {
			if (isDead == false) {
				isDead = true;
			}
		}
	}
}

void Enemy::OnColision() {
	isDead = true;
}

void Enemy::Attack(Model* model_)
{
	assert(model_);
	Vector3 pos;



	//弾を生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	//Bullet* newbullet = new Bullet();

	pos = worldTransForm.translation_;

	newBullet->Initialize(model_, pos, enemyFront);

	//弾を登録
	//bullets_.push_back(std::move(newBullet));
	gameScene->AddEnemyBullet(std::move(newBullet));

	//クールタイムをリセット
	coolTime = 250;


}

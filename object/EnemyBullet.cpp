#include "EnemyBullet.h"



void EnemyBullet::Initialize(Model* model, const Vector3& position, Vector3 vector)
{
	//NULLチェック
	assert(model);
	model_ = model;
	eneVec = vector;
	//eneVec.normalize();

	isDead = false;
	deadCount = 300;

	worldTransform_.Initialize();

	//worldTransform_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
	worldTransform_.translation_ = position;
	worldTransform_.matWorld_ *= Affin::matWorld(worldTransform_.translation_, worldTransform_.rotation_, worldTransform_.scale_);

	worldTransform_.TransferMatrix();
}

void EnemyBullet::Update()
{
	bulletSpe = eneVec;
	if (isDead==false) {
		if (deadCount > 0) {			

			worldTransform_.translation_ += bulletSpe*0.1;

			worldTransform_.matWorld_ *= Affin::matWorld(worldTransform_.translation_, worldTransform_.rotation_, worldTransform_.scale_);
			//行列の再計算
			worldTransform_.TransferMatrix();

			deadCount--;

		}
		else {
			isDead = true;
		}
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection, uint32_t textureHandle)
{
	//モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle);
}

void EnemyBullet::OnColision() {
	isDead = true;
}

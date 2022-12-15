#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "PrimitiveDrawer.h"

#include <random>
#include"Affin.h"
#define PI 3.14

float GameScene::Angle(float angle)
{
	return angle * PI / 180;
}



float Clamp(float min, float max, float num) {
	if (min > num) {
		return min;
	}
	else if (max < num) {
		return max;
	}
	return num;
}

GameScene::GameScene() {
	popTime = 0;
	coolTime = 0;
	killCounter = 0;
}

GameScene::~GameScene() {
	delete title;
	delete tutoliar;
	delete gameWin;
	delete gameOver;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを入れ込む
	textureHandle_[0] = TextureManager::Load("end.png");
	textureHandle_[1] = TextureManager::Load("floorSnow.png");
	textureHandle_[2] = TextureManager::Load("png.png");
	textureHandle_[3] = TextureManager::Load("inu.png");
	textureHandle_[4] = TextureManager::Load("ret.png");
	textureHandle_[5] = TextureManager::Load("Bullet.png");
	textureHandle_[6] = TextureManager::Load("Enemy.png");
	textureHandle_[7] = TextureManager::Load("title.png");
	textureHandle_[8] = TextureManager::Load("manual.png");
	textureHandle_[9] = TextureManager::Load("clear.png");

	//スプライトの生成
	title = Sprite::Create(textureHandle_[7], { 0,0 });
	tutoliar = Sprite::Create(textureHandle_[8], { 0,0 });
	gameWin = Sprite::Create(textureHandle_[9], { 0,0 });
	gameOver = Sprite::Create(textureHandle_[0], { 0,0 });
	//3Dモデルの生成
	model_ = Model::Create();
	modelWall_ = Model::CreateFromOBJ("zango");

	// 変数初期化

	PopPos_[0].Initialize();
	PopPos_[0].translation_.y = 0;
	//ワールドトランスフォームの初期化
	for (int i = 1; i < _countof(PopPos_); i++) {
		PopPos_[i].Initialize();
		PopPos_[i].parent_ = &PopPos_[0];
	}
	PopPos_[1].translation_ = { 150,0,0 };
	PopPos_[2].translation_ = { -150,0,0 };
	PopPos_[3].translation_ = { 0,0,150 };
	PopPos_[4].translation_ = { 0,0,-150 };


	wall_.Initialize();
	wall_.translation_ = { 0,0,0 };
	wall_.scale_ = { 5,5,5 };


	// 中心OBJ
	objHome_.Initialize();
	objHome_.scale_ = { 3,5,3 };
	objHome_.translation_ = { 0,1,0 };
	// 床OBJ
	floor_.Initialize();
	floor_.translation_ = { 0,-1,0 };
	floor_.scale_ = { 1500,0.1f,1500 };

	//ワールドトランスフォームの初期化
	playerWTF[0].Initialize();
	playerWTF[0].scale_ = { 3,3,3 };
	playerWTF[0].translation_ = { 0,1,0 };

	playerWTF[1].Initialize();
	playerWTF[1].translation_ = { 0,0,15 };
	playerWTF[1].parent_ = &playerWTF[0];

	playerWTF[2].Initialize();
	/*worldTransforms_[2].translation_ = { 0,0,100 };*/
	//worldTransforms_[2].parent_ = &worldTransforms_[1];	

	worldTransform3DReticle_.Initialize();

	//worldTransforms_[2].matWorld_ = worldTransforms_[1].matWorld_;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	ai = Affin::GetWorldTrans(playerWTF[1].matWorld_);
	viewProjection_.eye = { ai.x,ai.y,ai.z };
	viewProjection_.target = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);

	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();
}

void GameScene::Update() {
	srand((unsigned)time(nullptr));
	int popRand = rand() % 4 + 1;

	switch (scene)
	{
#pragma region TITLE
	case 0:
		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 1;
		}
		homeLife = 15000;
		popCount = 0;
		isDamage = false;
		damTimer = 0;
		killCounter = 0;
		wave = 0;
		waitTimer = 250;
		textureHandle_[2] = TextureManager::Load("png.png");

		break;

#pragma endregion

#pragma region GAME SCENE1
	case 1:
		if (isDamage == true) {
			damTimer++;
			if (damTimer == 30) {
				isDamage = false;
				textureHandle_[2] = TextureManager::Load("png.png");
				damTimer = 0;
			}
		}

		//デスフラグの立った弾を削除
		bullets_.remove_if([](std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); });

		//敵ポップ
		if (waitTimer == 0) {
			if (popCount > 0) {
				if (popTime == 0) {
					for (int i = 0; i < _countof(enemys); i++) {
						if (enemys[i].isDead == true) {
							enemys[i].Pop(Affin::GetWorldTrans(PopPos_[popRand].matWorld_), popRand);
							break;
						}
					}
					popCount--;
					popTime = 150;
				}
				else {
					popTime--;
				}
			}
			else {
				if (wave == 0) {
					wave = 1;
				}
			}
		}
		else {
			waitTimer--;
		}
		CheckAlive(enemys);
		//ウェーブ&勝利判定
		if (wave >= 0 && popCount == 0) {
			if (CheckAlive(enemys) == true) {
				if (wave < 3) {
					wave++;
					if (wave == 3) {
						popCount = 30;
					}
					else if (wave == 2) {
						popCount = 20;
					}
					else if (wave == 1) {
						popCount = 10;
					}
					waitTimer = 250;
				}
				else if (wave == 3) {
					scene = 2;
				}
			}
		}

		

		if (input_->PushKey(DIK_Q)) {
			playerWTF[1].translation_.y = 2;
		}
		else {
			playerWTF[1].translation_.y = 0;
		}


		//視点の移動速さ

		kCharacterSpeed = 0.1f;

		{	// 中心オブジェクト
			objHome_.matWorld_ = Affin::matUnit();
			objHome_.matWorld_ = Affin::matWorld(objHome_.translation_, objHome_.rotation_, objHome_.scale_);
			objHome_.TransferMatrix();
		}

		{
			addspeed = 0;
			// 回転処理
			if (input_->PushKey(DIK_A)) {

				if (KEyeSpeed > 0.0f) {
					KEyeSpeed *= -1;
				}
				else {
					addspeed -= 0.2;
				}
			}
			else if (input_->PushKey(DIK_D)) {
				if (KEyeSpeed < 0.0f) {
					KEyeSpeed *= -1;
				}
				else {
					addspeed += 0.2;
				}
			}
			// 親オブジェクト
			playerWTF[0].rotation_.y += addspeed;
		}


		for (int i = 0; i < _countof(playerWTF); i++) {

			playerWTF[i].matWorld_ = Affin::matUnit();
			playerWTF[i].matWorld_ = Affin::matWorld(
				playerWTF[i].translation_,
				playerWTF[i].rotation_,
				playerWTF[i].scale_);

			if (playerWTF[i].parent_ != nullptr) {
				playerWTF[i].matWorld_ *= playerWTF[i].parent_->matWorld_;
			}

			playerWTF[i].TransferMatrix();

		}

		for (int i = 0; i < _countof(PopPos_); i++) {

			PopPos_[i].matWorld_ = Affin::matUnit();
			PopPos_[i].matWorld_ = Affin::matWorld(
				PopPos_[i].translation_,
				PopPos_[i].rotation_,
				PopPos_[i].scale_);

			if (PopPos_[i].parent_ != nullptr) {
				PopPos_[i].matWorld_ *= PopPos_[i].parent_->matWorld_;
			}
			PopPos_[i].TransferMatrix();
		}
		{
			wall_.matWorld_ = Affin::matUnit();
			wall_.matWorld_ = Affin::matWorld(
				wall_.translation_,
				wall_.rotation_,
				wall_.scale_);

			if (wall_.parent_ != nullptr) {
				wall_.matWorld_ *= wall_.parent_->matWorld_;
			}
			wall_.TransferMatrix();
		}

		{	// 床
			floor_.matWorld_ = Affin::matUnit();
			floor_.matWorld_ = Affin::matWorld(floor_.translation_, floor_.rotation_, floor_.scale_);
			floor_.TransferMatrix();
		}


		//自機のワールド座標から3Dレティクルのワールド座標を計算
		//自機から3Dレティクルへの距離	

		if (input_->PushKey(DIK_DOWN) && kDistancePlayerTo3DReticle < 25) {
			kDistancePlayerTo3DReticle += 0.1;
		}
		else if (input_->PushKey(DIK_UP)) {
			kDistancePlayerTo3DReticle -= 0.1;
		}

		DebugText::GetInstance()->SetPos(30, 180);
		DebugText::GetInstance()->Printf(
			"Kill : %d", killCounter);
		DebugText::GetInstance()->SetPos(30, 60);
		DebugText::GetInstance()->Printf(
			"homeLife : %d", homeLife);
		DebugText::GetInstance()->SetPos(30, 40);
		DebugText::GetInstance()->Printf(
			"wave : %d", _countof(enemys));


		Reticle3D();

		if (input_->PushKey(DIK_Q)) {
			Attack();
		}


		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			bullet->Update(resultRet);
		}

		//敵更新
		for (int i = 0; i < _countof(enemys); i++) {
			enemys[i].Update(objHome_.translation_);
		}

		/// <summary>
		/// 弾と敵の当たり判定
		/// </summary>
		
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			posA = bullet->GetWorldPosition();
			//敵更新
			for (int i = 0; i < _countof(enemys); i++) {
				posB = enemys[i].GetWorldPosition();
				float a = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
					std::pow(posB.z - posA.z, 2.0f);
				float lenR = std::pow((enemys[i].r + bullet->r), 2.0);
				// 球と球の交差判定
				if (enemys[i].isDead == false) {
					if (a <= lenR) {
						// 自キャラの衝突時コールバックを呼び出す
						bullet->OnColision();
						// 敵弾の衝突時コールバックを呼び出す
						enemys[i].OnColision();
						killCounter++;
					}
				}
			}
			if (posA.y < -10) {
				bullet->OnColision();
			}
		}
		posA = Affin::GetWorldTrans(objHome_.matWorld_);
		//弾
		for (int i = 0; i < _countof(enemys); i++) {
			posB = enemys[i].GetWorldPosition();
			float a = std::pow(posB.x - posA.x, 2.0f) + std::pow(posB.y - posA.y, 2.0f) +
				std::pow(posB.z - posA.z, 2.0f);
			float lenR = std::pow((enemys[i].r + objHomeR), 2.0);
			// 球と球の交差判定
			if (a <= lenR) {
				if (enemys[i].isDead == false) {
					HomeOnColision();
				}
				// 敵弾の衝突時コールバックを呼び出す
				enemys[i].OnColision();
			}
		}



		if (input_->PushKey(DIK_R)) {
			ai = Affin::GetWorldTrans(playerWTF[1].matWorld_);
			focalLengs = 90;

			viewProjection_.eye = { ai.x,ai.y,ai.z };
			viewProjection_.target = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
		}
		else if (input_->PushKey(DIK_E)) {
			focalLengs = 20;

			viewProjection_.eye = Vector3(0, 150, 0);
			viewProjection_.target = Affin::GetWorldTrans(playerWTF[1].matWorld_);

		}
		else {
			ai = Affin::GetWorldTrans(playerWTF[1].matWorld_);
			focalLengs = 90;
			viewProjection_.eye = { ai.x,ai.y,ai.z };
			viewProjection_.target = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
		}
		viewProjection_.fovAngleY = 2 * atan(35.0f / (2 * focalLengs));

		viewProjection_.UpdateMatrix();
		viewProjection_.TransferMatrix();


		DebugText::GetInstance()->SetPos(300, 60);
		DebugText::GetInstance()->Printf(
			"viewEye : %f %f %f", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
		DebugText::GetInstance()->SetPos(300, 80);
		DebugText::GetInstance()->Printf(
			"viewEye : %f %f %f", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

		DebugText::GetInstance()->SetPos(300, 60);
		DebugText::GetInstance()->Printf(
			"viewEye : %f %f %f", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
		DebugText::GetInstance()->SetPos(300, 80);
		DebugText::GetInstance()->Printf(
			"viewEye : %f %f %f", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

		if (homeLife == 0) {
			scene = 3;
		}
#pragma endregion

		break;
	case 2:// victory

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 0;
		}

		break;
	case 3:// game over

		if (input_->TriggerKey(DIK_SPACE)) {
			scene = 0;
			for (int i = 0; i < _countof(enemys); i++) {
				if (enemys[i].isDead == false) {
					enemys[i].isDead = true;
				}
			}
		}
		break;

	case 4://操作説明
		if (input_->TriggerKey(DIK_SPACE)) {
			homeLife = 15;
			popCount = 0;
			isDamage = false;
			damTimer = 0;
			killCounter = 0;
			scene = 1;
			wave = 0;
			waitTimer = 250;
			textureHandle_[2] = TextureManager::Load("png.png");
		}
		break;
	}

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	if (scene == 1) {
		model_->Draw(objHome_, viewProjection_, textureHandle_[2]);
		model_->Draw(playerWTF[1], viewProjection_, textureHandle_[5]);
		model_->Draw(playerWTF[2], viewProjection_, textureHandle_[5]);
		model_->Draw(floor_, viewProjection_, textureHandle_[1]);
		model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_[4]);
		for (int i = 0; i < _countof(enemys); i++) {
			if (enemys[i].IsDead() == false) {
				model_->Draw(enemys[i].worldTransForm, viewProjection_, textureHandle_[6]);
			}
		}
		for (int i = 1; i < _countof(PopPos_); i++) {
			model_->Draw(PopPos_[i], viewProjection_);
		}

		modelWall_->Draw(wall_, viewProjection_);

		//弾描画
		for (std::unique_ptr<Bullet>& bullet : bullets_) {
			bullet->Draw(viewProjection_, textureHandle_[5]);
		}
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (scene) {
	case 0:
		title->Draw();
		break;
	case 1:
		break;
	case 2:
		gameWin->Draw();
		break;
	case 3:
		gameOver->Draw();
		break;
	case 4:
		tutoliar->Draw();
		break;
	}

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Attack()
{

	if (input_->PushKey(DIK_SPACE))
	{
		if (coolTime == 0) {
			//弾を生成し、初期化
			std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();

			//Bullet* newbullet = new Bullet();
			pos = Affin::GetWorldTrans(playerWTF[1].matWorld_);
			pos.y = 0;
			ret3DPos = Affin::GetWorldTrans(worldTransform3DReticle_.matWorld_);
			velo = ret3DPos - pos;
			velo.normalize();
			resultRet = velo * newBullet->speed;
			resultRet.normalize();
			newBullet->Initialize(model_, pos);

			//弾を登録
			bullets_.push_back(std::move(newBullet));

			//クールタイムをリセット
			coolTime = 25;
		}
		else {
			coolTime--;
		}
	}
}

void GameScene::Reticle3D() {
	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0.0f, 0, -1.0f };
	//自機のワールド行列の回転を反映
	offset = Affin::VecMat(offset, playerWTF[1].matWorld_);
	//ベクトルの長さを整える
	//offset.normalize();
	float len = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
	if (len != 0) {
		offset /= len;
	}
	offset *= kDistancePlayerTo3DReticle;
	worldTransform3DReticle_.translation_ = offset;
	worldTransform3DReticle_.scale_ = Vector3(0.5f, 0.5f, 0.5f);
	worldTransform3DReticle_.matWorld_ = Affin::matScale(worldTransform3DReticle_.scale_);
	worldTransform3DReticle_.matWorld_ = Affin::matTrans(worldTransform3DReticle_.translation_);

	worldTransform3DReticle_.TransferMatrix();

}

void GameScene::HomeOnColision() {
	textureHandle_[2] = TextureManager::Load("red.png");
	if (isDamage == false) {
		isDamage = true;
	}
	homeLife--;
}

int GameScene::CheckAlive(Enemy enemys_[]) {
	aliveNum = 0;

	for (int i = 0; i < _countof(enemys); i++) {
		if (enemys[i].isDead == false) {
			aliveNum++;
		}
	}
	
	if (aliveNum == 0) {
		return true;
	}
	else {
		return false;
	}
}

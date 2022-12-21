#pragma once
#include "model.h"
#include "WorldTransform.h"
#include "input.h"
#include "DebugText.h"
#include "ViewProjection.h"
#include <cassert>

#include "Affin.h"

class EnemyBullet
{
public:

	void Initialize(Model* model, const Vector3& position,Vector3 vector);

	void Update();

	void Draw(const ViewProjection& viewProjection, uint32_t textureHandle);

	void OnColision();

	Vector3 GetWorldPosition() { return Affin::GetWorldTrans(worldTransform_.matWorld_); };

	bool IsDead() const { return isDead; }

public:
	
	const int r = 1;
private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Vector3 eneVec;
	Vector3 bulletSpe/* = Vector3(0, 0, 10)*/;

	int deadCount = 0;
	bool isDead = false;
	float speed = 3;


};


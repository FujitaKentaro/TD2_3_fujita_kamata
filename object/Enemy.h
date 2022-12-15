#pragma once

#include "WorldTransform.h"
#include "Affin.h"
#include <random>

class Enemy {
public:
	WorldTransform worldTransForm;
	int isDead;
	Vector3 enemyTmp;
	Vector3 YTmp;
	//右ベクトル
	Vector3 enemyRight;
	//左ベクトル
	Vector3 enemyLeft;
	//正面ベクトル
	Vector3 enemyFront;
	//加速
	float speed;
	//time
	int time = 0;
	int seed_ = 0;

	int bulletcoolTime = 50;

	int r = 2;

public:
	Enemy();
	~Enemy();

	void Update(Vector3 obj);
	
	void CalcVec(Vector3 view);
	void Pop(Vector3 WorTrans, int seed_);
	void Hit();
	Vector3 GetWorldPosition() { return Affin::GetWorldTrans(worldTransForm.matWorld_); };
	void OnColision();
	bool IsDead() const { return isDead; }
};
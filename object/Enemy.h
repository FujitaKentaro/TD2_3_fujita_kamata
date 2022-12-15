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
	//�E�x�N�g��
	Vector3 enemyRight;
	//���x�N�g��
	Vector3 enemyLeft;
	//���ʃx�N�g��
	Vector3 enemyFront;
	//����
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
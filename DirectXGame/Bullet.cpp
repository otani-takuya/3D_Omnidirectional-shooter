
#include "Bullet.h"

#include "math.h"
#include <cmath>

using namespace KamataEngine;

// ==============================
// 初期化
// ==============================
void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& direction, int damage) {

	// モデルを保持
	model_ = model;

	// ==============================
	// 攻撃力の初期化
	// ==============================

	// 攻撃力がマイナスにならないようにする
	damage_ = (damage > 0) ? damage : 0;

	// ==============================
	// 命中状態の初期化
	// ==============================
	isHit_ = false;

	// ワールド変換初期化
	worldTransform_.Initialize();

	// 弾の初期座標
	worldTransform_.translation_ = position;

	// 前フレームの座標を初期化
	previousPosition_ = position;

	// 弾の大きさ
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};

	// ==============================
	// 発射方向を正規化
	// ==============================

	// XZ平面の方向ベクトル
	Vector3 normalizedDirection = {direction.x, 0.0f, direction.z};

	// ベクトルの長さ
	float length = std::sqrt(normalizedDirection.x * normalizedDirection.x + normalizedDirection.z * normalizedDirection.z);

	// 方向が指定されている場合
	if (length > 0.0001f) {

		normalizedDirection.x /= length;
		normalizedDirection.z /= length;

	} else {

		// 方向がゼロの場合は前方へ発射
		normalizedDirection = {0.0f, 0.0f, 1.0f};
	}

	// ==============================
	// 弾の移動速度
	// ==============================
	velocity_ = {normalizedDirection.x * kBulletSpeed, 0.0f, normalizedDirection.z * kBulletSpeed};

	// 弾の向きを移動方向に合わせる
	worldTransform_.rotation_.y = std::atan2(normalizedDirection.x, normalizedDirection.z);

	// ==============================
	// 弾の状態を初期化
	// ==============================
	isDead_ = false;
	isHit_ = false;
	lifeTimer_ = 0;

	// ワールド行列更新
	UpdateWorldTransform(worldTransform_);
	worldTransform_.TransferMatrix();
}

// ==============================
// 更新
// ==============================
void Bullet::Update() {

	// 削除対象なら更新しない
	if (isDead_) {
		return;
	}

	// ==============================
	// 移動前の座標を保存
	// ==============================
	previousPosition_ = worldTransform_.translation_;

	// ==============================
	// 弾を移動
	// ==============================
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// ==============================
	// 寿命管理
	// ==============================
	lifeTimer_++;

	// 一定時間経過したら削除対象にする
	if (lifeTimer_ >= kLifeTime) {
		RequestDestroy();
		return;
	}

	// ==============================
	// ワールド行列更新
	// ==============================
	UpdateWorldTransform(worldTransform_);
	worldTransform_.TransferMatrix();
}

// ==============================
// 弾の削除要求
// ==============================
void Bullet::RequestDestroy() {

	// 弾を削除対象にする
	isDead_ = true;
}

// ==============================
// 描画
// ==============================
void Bullet::Draw(Camera& camera) {

	// モデルがない場合は描画しない
	if (model_ == nullptr || isDead_) {
		return;
	}

	// 弾モデルを描画
	model_->Draw(worldTransform_, camera);
}

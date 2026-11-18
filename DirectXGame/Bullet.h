
#pragma once

#include "KamataEngine.h"

// ==============================
// プレイヤーの弾クラス
// ==============================
class Bullet {
public:
	// ==============================
	// 初期化
	// ==============================
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction);

	// ==============================
	// 更新
	// ==============================
	void Update();

	// ==============================
	// 描画
	// ==============================
	void Draw(KamataEngine::Camera& camera);

	// ==============================
	// 弾の状態を取得
	// ==============================
	bool IsDead() const { return isDead_; }

	// ==============================
	// 弾のワールド座標を取得
	// ==============================
	KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;

	// 弾の3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// 移動速度
	KamataEngine::Vector3 velocity_ = {};

	// 削除フラグ
	bool isDead_ = false;

	// 生存フレーム数
	int lifeTimer_ = 0;

	// 弾の移動速度
	static inline constexpr float kBulletSpeed = 0.6f;

	// 弾の生存時間（60FPSで約2秒）
	static inline constexpr int kLifeTime = 120;
};

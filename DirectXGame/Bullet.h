
#pragma once

#include "KamataEngine.h"

// ==============================
// プレイヤーの弾クラス
// ==============================
class Bullet {
public:
	// ==============================
	// 弾の基本設定
	// ==============================

	// 基本攻撃力
	static constexpr int kDefaultDamage = 20;

	// ==============================
	// 初期化
	// ==============================
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction, int damage = kDefaultDamage);

	// ==============================
	// 更新
	// ==============================
	void Update();

	// ==============================
	// 描画
	// ==============================
	void Draw(KamataEngine::Camera& camera);
	
	// ==============================
	// 弾の状態管理
	// ==============================

	// 削除対象かどうか
	bool IsDead() const { return isDead_; }

	// 敵に命中したかどうか
	bool IsHit() const { return isHit_; }

	// 命中済み状態にする
	void SetHit() { isHit_ = true; }

	// 弾の削除を要求する
	void RequestDestroy();

	// ==============================
	// 弾のワールド座標を取得
	// ==============================
	KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

	// ==============================
	// 弾の前フレームの座標を取得
	// ==============================
	KamataEngine::Vector3 GetPreviousPosition() const { return previousPosition_; }

	// ==============================
	// 攻撃力を取得
	// ==============================
	int GetDamage() const { return damage_; }

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;

	// 弾の3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// 移動速度
	KamataEngine::Vector3 velocity_ = {};

	// 前フレームのワールド座標
	KamataEngine::Vector3 previousPosition_ = {};

	// 削除フラグ
	bool isDead_ = false;

	// 生存フレーム数
	int lifeTimer_ = 0;

	// 弾の移動速度
	static inline constexpr float kBulletSpeed = 0.6f;

	// 弾の生存時間（60FPSで約2秒）
	static inline constexpr int kLifeTime = 120;

	// ==============================
	// 攻撃力管理
	// ==============================

	// この弾が持つ攻撃力
	int damage_ = kDefaultDamage;

	// ==============================
	// 命中状態
	// ==============================

	// 敵に命中したかどうか
	bool isHit_ = false;
};

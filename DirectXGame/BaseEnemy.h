#pragma once

#include "Collision.h"
#include "KamataEngine.h"
#include "math.h"

class MapChipField;
class Player;
class GameScene;

// ==============================
// 敵の基底クラス
// ==============================
class BaseEnemy {
public:
	// 仮想デストラクタ
	virtual ~BaseEnemy() = default;

	// ==============================
	// 基本処理
	// ==============================

	// 初期化
	virtual void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	virtual void Update();

	// 描画
	virtual void Draw();

	// ==============================
	// 外部クラスの設定
	// ==============================

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// ==============================
	// 座標・当たり判定
	// ==============================

	// ワールド座標取得
	KamataEngine::Vector3 GetWorldPosition() const;

	// AABB取得
	AABB GetAABB() const;

	// プレイヤーとの衝突
	virtual void OnCollision(Player* player);

	// 死亡状態取得
	bool IsDead() const { return isDead_; }

	// 衝突無効状態取得
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	// ==============================
	// HP・ダメージ管理
	// ==============================

	// ダメージを受ける
	virtual void TakeDamage(int damage);

	// 現在のHP取得
	int GetHP() const { return hp_; }

	// 最大HP取得
	int GetMaxHP() const { return maxHP_; }

	// HPの割合取得
	float GetHPRatio() const {

		if (maxHP_ <= 0) {
			return 0.0f;
		}

		return static_cast<float>(hp_) / static_cast<float>(maxHP_);
	}

protected:
	// ==============================
	// 敵の行動状態
	// ==============================
	enum class Behavior { 
		kWalk, 
		kDeath, 
		kUnknown 
	};

	// ==============================
	// 死亡状態への遷移を要求する
	// ==============================
	void RequestDeath();

	// ==============================
	// 各状態の初期化
	// ==============================
	virtual void BehaviorWalkInitialize();
	virtual void BehaviorDeathInitialize();

	// ==============================
	// 各状態の更新
	// ==============================
	virtual void BehaviorWalkUpdate();
	virtual void BehaviorDeathUpdate();

	// ==============================
	// ワールド変換
	// ==============================

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	MapChipField* mapChipField_ = nullptr;
	GameScene* gameScene_ = nullptr;

	// ==============================
	// 当たり判定サイズ
	// ==============================

	static inline const float kEnemyWidth = 0.8f;
	static inline const float kEnemyHeight = 0.8f;

	// ==============================
	// 移動管理
	// ==============================

	static inline const float kWalkSpeed = 0.05f;

	KamataEngine::Vector3 velocity_ = {};

	// ==============================
	// アニメーション管理
	// ==============================

	float walkTimer_ = 0.0f;
	float deathTimer_ = 0.0f;

	static inline const float kWalkMotionAngleStart = -45.0f;
	static inline const float kWalkMotionAngleEnd = 100.0f;
	static inline const float kWalkMotionTime = 6.0f;

	// ==============================
	// HP管理
	// ==============================

	// 基本の最大HP
	static constexpr int kDefaultMaxHP = 100;

	// 最大HP
	int maxHP_ = kDefaultMaxHP;

	// 現在のHP
	int hp_ = kDefaultMaxHP;

	// ==============================
	// 状態フラグ
	// ==============================

	bool isDead_ = false;
	bool isCollisionDisabled_ = false;

	// ==============================
	// 現在の行動状態
	// ==============================

	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;
};
#pragma once

#include "KamataEngine.h"

class Player;
class BulletManager;

// ==============================
// プレイヤー射撃管理クラス
// ==============================
class PlayerShooting {
public:
	// 初期化
	void Initialize(Player* player, BulletManager* bulletManager, KamataEngine::Camera* camera);

	// マウス照準・射撃更新
	void Update();

	// 照準方向を取得
	const KamataEngine::Vector3& GetAimDirection() const { return aimDirection_; }

	// 射撃入力状態をリセット
	void ResetInput();

private:
	// マウス座標から照準方向を計算
	bool UpdateAimDirection();

	// 弾を発射
	void Fire();

	// 参照先はGameSceneが管理する
	Player* player_ = nullptr;
	BulletManager* bulletManager_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// 照準方向（XZ平面）
	KamataEngine::Vector3 aimDirection_ = {0.0f, 0.0f, 1.0f};

	// ==============================
	// 射撃間隔の管理
	// ==============================

	// 連射間隔（フレーム数）
	static constexpr int kFireInterval = 10;

	// 次の射撃までの残りフレーム
	int fireCooldown_ = 0;

	// 前フレームの左クリック状態
	bool wasMousePressed_ = false;

	// ==============================
	// 弾の発射位置（プレイヤーローカル座標）
	// ==============================

	// X：左右、Y：高さ、Z：前後
	// プレイヤーの正面はローカル+Z方向
	static constexpr KamataEngine::Vector3 kMuzzleOffset = {
	    0.0f, // 左右
	    0.5f, // 高さ
	    1.0f  // 前方
	};

	// 左クリックを離すまで射撃を禁止
	bool waitForMouseRelease_ = false;
}
;
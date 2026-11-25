#pragma once

#include "Bullet.h"
#include "KamataEngine.h"

#include <list>
#include <memory>

// ==============================
// 弾の管理クラス
// ==============================
class BulletManager {
public:
	// ==============================
	// 初期化
	// ==============================
	void Initialize(KamataEngine::Model* bulletModel);

	// ==============================
	// 弾の生成
	// ==============================
	void Fire(const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction, int damage = Bullet::kDefaultDamage);

	// ==============================
	// 弾の更新
	// ==============================
	void Update();

	// ==============================
	// 削除対象の弾を取り除く
	// ==============================
	void RemoveDeadBullets();

	// ==============================
	// 弾の描画
	// ==============================
	void Draw(KamataEngine::Camera& camera);

	// ==============================
	// すべての弾を削除
	// ==============================
	void Clear();

	// ==============================
	// 弾の数を取得
	// ==============================
	size_t GetBulletCount() const;

	// ==============================
	// 弾のリストを取得
	// ==============================
	const std::list<std::unique_ptr<Bullet>>& GetBullets() const { return bullets_; }

private:
	// 弾のモデル
	KamataEngine::Model* bulletModel_ = nullptr;

	// 弾の管理リスト
	std::list<std::unique_ptr<Bullet>> bullets_;
};
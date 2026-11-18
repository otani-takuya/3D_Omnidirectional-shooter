
#pragma once

#include "Bullet.h"
#include "KamataEngine.h"

#include <list>
#include <memory>

// ==============================
// 弾管理クラス
// ==============================
class BulletManager {
public:
	// 初期化
	void Initialize(KamataEngine::Model* bulletModel);

	// 弾の生成
	void Fire(const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction);

	// 弾の更新・寿命切れ削除
	void Update();

	// 全弾の描画
	void Draw(KamataEngine::Camera& camera);

	// 全弾を削除
	void Clear();

	// 現在の弾数を取得
	size_t GetBulletCount() const;

private:
	// ResourceManagerが所有する共有モデル
	// このクラスではdeleteしない
	KamataEngine::Model* bulletModel_ = nullptr;

	// BulletManagerが弾を所有する
	std::list<std::unique_ptr<Bullet>> bullets_;
};

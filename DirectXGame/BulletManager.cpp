
#include "BulletManager.h"

#include <cassert>

using namespace KamataEngine;

// ==============================
// 初期化
// ==============================
void BulletManager::Initialize(Model* bulletModel) {

	assert(bulletModel != nullptr);

	// 再初期化時に既存の弾を削除
	Clear();

	// モデルはResourceManagerから借りる
	bulletModel_ = bulletModel;
}

// ==============================
// 弾の生成
// ==============================
void BulletManager::Fire(const Vector3& position, const Vector3& direction) {

	// モデル未設定の場合
	if (bulletModel_ == nullptr) {
		return;
	}

	// 新しい弾を生成
	auto bullet = std::make_unique<Bullet>();

	// 弾の初期化
	bullet->Initialize(bulletModel_, position, direction);

	// リストに追加
	bullets_.push_back(std::move(bullet));
}

// ==============================
// 弾の更新
// ==============================
void BulletManager::Update() {

	// すべての弾を更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 寿命が終了した弾を削除
	bullets_.remove_if([](const std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); });
}

// ==============================
// 弾の描画
// ==============================
void BulletManager::Draw(Camera& camera) {

	for (auto& bullet : bullets_) {
		bullet->Draw(camera);
	}
}

// ==============================
// 全弾削除
// ==============================
void BulletManager::Clear() { bullets_.clear(); }

// ==============================
// 現在の弾数
// ==============================
size_t BulletManager::GetBulletCount() const { return bullets_.size(); }

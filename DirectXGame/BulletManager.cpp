#include "BulletManager.h"

#include <cassert>
#include <utility>

// ==============================
// 初期化
// ==============================
void BulletManager::Initialize(KamataEngine::Model* bulletModel) {

	// モデルの有効性を確認
	assert(bulletModel);

	// 既存の弾を削除
	Clear();

	// モデルを保持
	bulletModel_ = bulletModel;
}

// ==============================
// 弾の生成
// ==============================
void BulletManager::Fire(const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction, int damage) {

	// モデルが無効な場合は生成しない
	if (!bulletModel_) {
		return;
	}

	// 弾のインスタンスを生成
	auto bullet = std::make_unique<Bullet>();

	// 弾の初期化
	bullet->Initialize(bulletModel_, position, direction, damage);

	// 管理リストに追加
	bullets_.push_back(std::move(bullet));
}

// ==============================
// 弾の更新
// ==============================
void BulletManager::Update() {

	for (auto& bullet : bullets_) {

		// 無効・削除対象の弾は更新しない
		if (!bullet || bullet->IsDead()) {
			continue;
		}

		// 弾を更新
		bullet->Update();
	}
}

// ==============================
// 削除対象の弾を取り除く
// ==============================
void BulletManager::RemoveDeadBullets() {

	bullets_.remove_if([](const std::unique_ptr<Bullet>& bullet) {
		// 無効なポインタを削除
		if (!bullet) {
			return true;
		}

		// 削除対象の弾を削除
		return bullet->IsDead();
	});
}

// ==============================
// 弾の描画
// ==============================
void BulletManager::Draw(KamataEngine::Camera& camera) {

	for (auto& bullet : bullets_) {

		// 無効・削除対象の弾は描画しない
		if (!bullet || bullet->IsDead()) {
			continue;
		}

		// 弾を描画
		bullet->Draw(camera);
	}
}

// ==============================
// すべての弾を削除
// ==============================
void BulletManager::Clear() { bullets_.clear(); }

// ==============================
// 弾の数を取得
// ==============================
size_t BulletManager::GetBulletCount() const { return bullets_.size(); }
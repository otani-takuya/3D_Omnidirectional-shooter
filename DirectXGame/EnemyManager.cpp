
#include "EnemyManager.h"

#include "Enemy.h"
#include "ShieldEnemy.h"

#include <cassert>
#include <utility>

// ==============================
// 初期化
// ==============================
void EnemyManager::Initialize(KamataEngine::Model* enemyModel, KamataEngine::Model* shieldEnemyModel, KamataEngine::Camera* camera, MapChipField* mapChipField, GameScene* gameScene) {

	// 引数の有効性を確認
	assert(enemyModel);
	assert(shieldEnemyModel);
	assert(camera);
	assert(mapChipField);
	assert(gameScene);

	// 既存の敵を削除
	Clear();

	// 敵モデルを保持
	enemyModel_ = enemyModel;
	shieldEnemyModel_ = shieldEnemyModel;

	// 外部オブジェクトの参照を保持
	camera_ = camera;
	mapChipField_ = mapChipField;
	gameScene_ = gameScene;
}

// ==============================
// 敵の生成
// ==============================
void EnemyManager::Spawn(EnemyType type, const KamataEngine::Vector3& position) {

	// 必要な情報が無効な場合は生成しない
	if (!camera_ || !mapChipField_ || !gameScene_) {
		return;
	}

	// 生成する敵の所有権を保持
	std::unique_ptr<BaseEnemy> enemy;

	// 敵の種類に応じて生成
	switch (type) {

	// ==============================
	// 通常敵
	// ==============================
	case EnemyType::kNormal: {

		// モデルの有効性を確認
		if (!enemyModel_) {
			return;
		}

		// 通常敵を生成
		auto normalEnemy = std::make_unique<Enemy>();

		// 初期化
		normalEnemy->Initialize(enemyModel_, camera_, position);

		// 所有権を移動
		enemy = std::move(normalEnemy);

		break;
	}

	// ==============================
	// シールド敵
	// ==============================
	case EnemyType::kShield: {

		// モデルの有効性を確認
		if (!shieldEnemyModel_) {
			return;
		}

		// シールド敵を生成
		auto shieldEnemy = std::make_unique<ShieldEnemy>();

		// 初期化
		shieldEnemy->Initialize(shieldEnemyModel_, camera_, position);

		// 所有権を移動
		enemy = std::move(shieldEnemy);

		break;
	}

	default:
		return;
	}

	// ==============================
	// 共通情報の設定
	// ==============================

	// マップ情報を設定
	enemy->SetMapChipField(mapChipField_);

	// ゲームシーンを設定
	enemy->SetGameScene(gameScene_);

	// 管理リストへ所有権を移動
	enemies_.push_back(std::move(enemy));
}

// ==============================
// 敵の更新
// ==============================
void EnemyManager::Update() {

	for (auto& enemy : enemies_) {

		// 無効・死亡完了した敵は更新しない
		if (!enemy || enemy->IsDead()) {
			continue;
		}

		// 敵を更新
		enemy->Update();
	}
}

// ==============================
// 死亡した敵を取り除く
// ==============================
void EnemyManager::RemoveDeadEnemies() {

	enemies_.remove_if([](const std::unique_ptr<BaseEnemy>& enemy) {
		// 無効なポインタを削除
		if (!enemy) {
			return true;
		}

		// 死亡演出が終了した敵を削除
		return enemy->IsDead();
	});
}

// ==============================
// 敵の描画
// ==============================
void EnemyManager::Draw() {

	for (auto& enemy : enemies_) {

		// 無効・死亡完了した敵は描画しない
		if (!enemy || enemy->IsDead()) {
			continue;
		}

		// 敵を描画
		enemy->Draw();
	}
}

// ==============================
// すべての敵を削除
// ==============================
void EnemyManager::Clear() {

	// unique_ptrによって自動的に解放される
	enemies_.clear();
}

// ==============================
// 敵の数を取得
// ==============================
std::size_t EnemyManager::GetEnemyCount() const { return enemies_.size(); }

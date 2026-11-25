
#pragma once

// ==============================
// インクルード
// ==============================
#include "BaseEnemy.h"
#include "KamataEngine.h"

#include <cstddef>
#include <list>
#include <memory>

// ==============================
// 前方宣言
// ==============================
class MapChipField;
class GameScene;

// ==============================
// 敵の管理クラス
// ==============================
class EnemyManager {
public:
	// ==============================
	// 敵の種類
	// ==============================
	enum class EnemyType {
		kNormal, // 通常敵
		kShield  // シールド敵
	};

	// ==============================
	// 初期化
	// ==============================
	void Initialize(KamataEngine::Model* enemyModel, KamataEngine::Model* shieldEnemyModel, KamataEngine::Camera* camera, MapChipField* mapChipField, GameScene* gameScene);

	// ==============================
	// 敵の生成
	// ==============================
	void Spawn(EnemyType type, const KamataEngine::Vector3& position);

	// ==============================
	// 敵の更新
	// ==============================
	void Update();

	// ==============================
	// 死亡した敵を取り除く
	// ==============================
	void RemoveDeadEnemies();

	// ==============================
	// 敵の描画
	// ==============================
	void Draw();

	// ==============================
	// すべての敵を削除
	// ==============================
	void Clear();

	// ==============================
	// 敵の数を取得
	// ==============================
	std::size_t GetEnemyCount() const;

	// ==============================
	// 敵の管理リストを取得
	// ==============================
	const std::list<std::unique_ptr<BaseEnemy>>& GetEnemies() const { return enemies_; }

private:
	// ==============================
	// 敵モデル
	// ==============================

	// 通常敵のモデル
	KamataEngine::Model* enemyModel_ = nullptr;

	// シールド敵のモデル
	KamataEngine::Model* shieldEnemyModel_ = nullptr;

	// ==============================
	// 外部オブジェクトの参照
	// ==============================

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// ==============================
	// 敵の管理リスト
	// ==============================
	std::list<std::unique_ptr<BaseEnemy>> enemies_;
};

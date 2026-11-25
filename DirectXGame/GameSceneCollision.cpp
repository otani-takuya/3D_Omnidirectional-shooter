#include "BaseEnemy.h"
#include "Bullet.h"
#include "Collision.h"
#include "GameScene.h"
#include "GuardEffect.h"
#include "HitEffect.h"

using namespace KamataEngine;

// ==============================
// CheckAllCollisions
// ゲーム内の衝突判定をまとめて呼び出す。現在は弾と敵の判定を担当する。
// ==============================
void GameScene::CheckAllCollisions() {

	// 弾と敵の当たり判定
	CheckBulletEnemyCollisions();
}

// ==============================
// CheckBulletEnemyCollisions
// 弾の移動線分と敵のAABBを比較し、命中時にダメージと削除要求を設定する。
// ==============================
void GameScene::CheckBulletEnemyCollisions() {

	// 毎フレームの衝突情報をリセット
	bulletHitCount_ = 0;
	isBulletHit_ = false;

	// ==============================
	// すべての弾を確認
	// ==============================

	for (const auto& bullet : bulletManager_.GetBullets()) {

		// 削除対象・命中済みの弾は判定しない
		if (!bullet || bullet->IsDead() || bullet->IsHit()) {

			continue;
		}

		// 弾の移動前後の座標
		// 現在位置だけでなく移動区間を調べ、高速な弾のすり抜けを抑える。
		const Vector3 start = bullet->GetPreviousPosition();

		const Vector3 end = bullet->GetWorldPosition();

		// ==============================
		// すべての敵を確認
		// ==============================

		for (const auto& enemy : enemyManager_.GetEnemies()) {

			// 無効な敵は判定しない
			if (enemy == nullptr || enemy->IsDead() || enemy->IsCollisionDisabled() || enemy->GetHP() <= 0) {

				continue;
			}

			// 敵のAABBを取得
			const AABB enemyAABB = enemy->GetAABB();

			// 線分とAABBの衝突判定
			if (!IsCollisionSegmentAABB(start, end, enemyAABB)) {

				continue;
			}

			// ==============================
			// ダメージ処理
			// ==============================

			// 弾の攻撃力を取得
			const int damage = bullet->GetDamage();

			// 敵にダメージを与える
			// HPと死亡要求の管理は敵自身に任せ、シーンから内部状態を直接変更しない。
			enemy->TakeDamage(damage);

			// 弾を命中済みにする
			bullet->SetHit();

			// 弾の削除を要求する
			// ここでは削除フラグのみ設定する。実際の解放は衝突判定終了後に行う。
			bullet->RequestDestroy();

			// ==============================
			// 衝突情報の更新
			// ==============================

			isBulletHit_ = true;

			++bulletHitCount_;
			++totalBulletHitCount_;

			// ==============================
			// ヒットエフェクトの生成
			// ==============================

			if (isDrawBulletHitEffect_) {

				// 既存仕様に合わせ、線分の終点を演出位置として使用する（厳密な交点ではない）。
				CreateHitEffect(end);
			}

			// 1発の弾で複数の敵に
			// ダメージを与えないようにする
			break;
		}
	}
}

// ==============================
// CreateHitEffect
// 指定されたワールド座標に命中エフェクトを生成し、シーンの管理リストに登録する。
// ==============================
void GameScene::CreateHitEffect(const Vector3& position) {

	BaseEffect* effect = HitEffect::Create(position);

	// 生成されたエフェクトの解放責任はGameSceneが持つ。
	effects_.push_back(effect);
}

// ==============================
// CreateGuardEffect
// 指定されたワールド座標にガードエフェクトを生成し、シーンの管理リストに登録する。
// ==============================
void GameScene::CreateGuardEffect(const Vector3& position) {

	BaseEffect* effect = GuardEffect::Create(position);

	// 生成されたエフェクトの解放責任はGameSceneが持つ。
	effects_.push_back(effect);
}

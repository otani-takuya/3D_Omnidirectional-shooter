#include "GameScene.h"
#ifdef _DEBUG
#include "EnemyDebugUI.h"
#include <imgui.h>
#endif

using namespace KamataEngine;

// ==============================
// Draw
// 既存の描画順序でフィールド、キャラクター、演出を描画する。
// ==============================
void GameScene::Draw() {

	// 描画開始
	Model::PreDraw();

	// ==============================
	// 床の描画
	// ==============================

	if (groundModel_ != nullptr) {

		groundModel_->Draw(groundWorldTransform_, camera_);
	}

	// ==============================
	// ブロック描画
	// ==============================

	for (auto& line : worldTransformBlocks_) {
		for (auto& wt : line) {
			if (!wt)
				continue;

			modelBlock_->Draw(*wt, camera_);
		}
	}

	// スカイドーム描画
	skydome_->Draw();

	// ==============================
	// 敵の描画
	// ==============================

	enemyManager_.Draw();

	// エフェクト描画
	for (BaseEffect* effect : effects_) {
		if (effect) {
			effect->Draw();
		}
	}

	player_->Draw();

	// 弾の描画
	bulletManager_.Draw(camera_);

	// やられパーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// 既存の描画順序を維持し、フェード演出を最後に描画する。
	fade_->Draw();

	// 描画終了
	Model::PostDraw();
}

// ==============================
// DrawDebugUI
// Debug構成でシーン設定、衝突情報、敵情報を表示する。
// ==============================
void GameScene::DrawDebugUI() {
#ifdef _DEBUG

	// ==============================
	// ゲームシーン設定
	// ==============================

	ImGui::Begin("GameScene");

	if (ImGui::Button("Reload Map")) {
		reloadRequested_ = true;
	}

	ImGui::End();

	// ==============================
	// 衝突判定デバッグ
	// ==============================

	ImGui::Begin("Collision Debug");

	// 現在の弾数
	ImGui::Text("Bullet Count : %d", static_cast<int>(bulletManager_.GetBulletCount()));

	// 現在の敵数
	ImGui::Text("Enemy Count : %d", static_cast<int>(enemyManager_.GetEnemyCount()));

	ImGui::Separator();

	// 現在のフレームの衝突回数
	ImGui::Text("Hit Count : %d", bulletHitCount_);

	// 累計衝突回数
	ImGui::Text("Total Hit Count : %d", totalBulletHitCount_);

	// 衝突状態を色付きで表示
	if (isBulletHit_) {

		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.3f, 1.0f), "Collision : HIT");

	} else {

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Collision : NONE");
	}

	ImGui::Separator();

	// ヒットエフェクトの表示切り替え
	ImGui::Checkbox("Draw Hit Effect", &isDrawBulletHitEffect_);

	// 累計衝突回数をリセット
	if (ImGui::Button("Reset Hit Count")) {

		totalBulletHitCount_ = 0;
	}

	ImGui::End();

	// ==============================
	// 敵のデバッグ情報を表示
	// ==============================

	EnemyDebugUI::Draw(enemyManager_.GetEnemies());

#endif
}

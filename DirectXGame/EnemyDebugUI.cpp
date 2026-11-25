
#include "EnemyDebugUI.h"
#include "BaseEnemy.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

// ==============================
// 敵全体のデバッグ情報を表示
// ==============================
void EnemyDebugUI::Draw(const std::list<std::unique_ptr<BaseEnemy>>& enemies) {

#ifdef _DEBUG

	// ==============================
	// 敵の状態を集計
	// ==============================
	int enemyCount = 0;
	int aliveCount = 0;
	int zeroHPCount = 0;

	for (const auto& enemy : enemies) {

		// 無効なポインタは除外
		if (!enemy) {
			continue;
		}

		++enemyCount;

		// HPが0になった敵
		if (enemy->GetHP() <= 0) {

			++zeroHPCount;

		} else if (!enemy->IsDead()) {

			// 生存している敵
			++aliveCount;
		}
	}

	// ==============================
	// ImGuiウィンドウ
	// ==============================
	if (ImGui::Begin("Enemy Debug")) {

		// 敵全体の情報
		ImGui::Text("Enemy Count : %d", enemyCount);

		ImGui::Text("Alive Count : %d", aliveCount);

		ImGui::Text("Zero HP Count : %d", zeroHPCount);

		ImGui::Separator();

		// ==============================
		// 敵ごとの情報を表示
		// ==============================
		int index = 0;

		for (const auto& enemy : enemies) {

			// 無効なポインタは除外
			if (!enemy) {
				continue;
			}

			// 敵1体分の情報を表示
			DrawEnemyInfo(*enemy, index);

			++index;
		}
	}

	ImGui::End();

#else

	// Release時の未使用引数警告対策
	(void)enemies;

#endif
}

// ==============================
// 敵1体分のデバッグ情報
// ==============================
void EnemyDebugUI::DrawEnemyInfo(const BaseEnemy& enemy, int index) {

#ifdef _DEBUG

	// ImGuiのID重複を防止
	ImGui::PushID(&enemy);

	// ==============================
	// 敵ごとの表示領域
	// ==============================
	if (ImGui::TreeNode("Enemy", "Enemy [%d]", index)) {

		// ==============================
		// HP情報の取得
		// ==============================
		const int hp = enemy.GetHP();
		const int maxHP = enemy.GetMaxHP();

		const float hpRatio = enemy.GetHPRatio();

		// 現在HP・最大HP
		ImGui::Text("HP : %d / %d", hp, maxHP);

		// ==============================
		// HPゲージの色を設定
		// ==============================
		ImVec4 hpColor;

		if (hpRatio > 0.5f) {

			// 通常状態：緑
			hpColor = ImVec4(0.2f, 0.8f, 0.3f, 1.0f);

		} else if (hpRatio > 0.2f) {

			// 注意状態：黄色
			hpColor = ImVec4(0.9f, 0.7f, 0.2f, 1.0f);

		} else {

			// 瀕死状態：赤
			hpColor = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
		}

		// ==============================
		// HPゲージの描画
		// ==============================
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, hpColor);

		ImGui::ProgressBar(hpRatio, ImVec2(-1.0f, 20.0f));

		ImGui::PopStyleColor();

		// ==============================
		// 敵の状態表示
		// ==============================
		if (enemy.IsDead()) {

			ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Status : DEAD");

		} else if (hp <= 0) {

			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.2f, 1.0f), "Status : ZERO HP");

		} else {

			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.3f, 1.0f), "Status : ALIVE");
		}

		// ==============================
		// ワールド座標の表示
		// ==============================
		const KamataEngine::Vector3 position = enemy.GetWorldPosition();

		ImGui::Text("Position X : %.2f", position.x);

		ImGui::Text("Position Y : %.2f", position.y);

		ImGui::Text("Position Z : %.2f", position.z);

		// ==============================
		// 当たり判定状態
		// ==============================
		ImGui::Text("Collision Disabled : %s", enemy.IsCollisionDisabled() ? "TRUE" : "FALSE");

		ImGui::TreePop();
	}

	ImGui::PopID();

#else

	// Release時の未使用引数警告対策
	(void)enemy;
	(void)index;

#endif
}

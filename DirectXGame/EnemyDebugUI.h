#pragma once

#include <list>
#include <memory>

// ==============================
// 前方宣言
// ==============================
class BaseEnemy;

// ==============================
// 敵のデバッグUI
// ==============================
class EnemyDebugUI {
public:
	// ==============================
	// 敵全体のデバッグ情報を表示
	// ==============================
	static void Draw(const std::list<std::unique_ptr<BaseEnemy>>& enemies);

private:
	// ==============================
	// 敵1体分のデバッグ情報を表示
	// ==============================
	static void DrawEnemyInfo(const BaseEnemy& enemy, int index);
};

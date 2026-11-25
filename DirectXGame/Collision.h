#pragma once

#include "KamataEngine.h"

#include <algorithm>
#include <cmath>

// ==============================
// AABB構造体
// ==============================
struct AABB {

	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
};

// ==============================
// 線分とAABBの衝突判定
// ==============================
inline bool IsCollisionSegmentAABB(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, const AABB& aabb) {

	// 線分の判定範囲
	float tMin = 0.0f;
	float tMax = 1.0f;

	// 線分の開始位置
	const float origins[3] = {start.x, start.y, start.z};

	// 線分の方向
	const float directions[3] = {end.x - start.x, end.y - start.y, end.z - start.z};

	// AABBの最小座標
	const float minBounds[3] = {aabb.min.x, aabb.min.y, aabb.min.z};

	// AABBの最大座標
	const float maxBounds[3] = {aabb.max.x, aabb.max.y, aabb.max.z};

	// ==============================
	// X・Y・Zの各軸を判定
	// ==============================
	for (int i = 0; i < 3; ++i) {

		// 軸方向に移動していない場合
		if (std::abs(directions[i]) < 0.000001f) {

			// AABBの範囲外なら衝突しない
			if (origins[i] < minBounds[i] || origins[i] > maxBounds[i]) {

				return false;
			}

			continue;
		}

		// 方向成分の逆数
		const float inverse = 1.0f / directions[i];

		// AABBに入る位置と出る位置
		float t1 = (minBounds[i] - origins[i]) * inverse;

		float t2 = (maxBounds[i] - origins[i]) * inverse;

		// 大小関係を整理
		if (t1 > t2) {
			std::swap(t1, t2);
		}

		// 衝突範囲を更新
		tMin = (std::max)(tMin, t1);
		tMax = (std::min)(tMax, t2);

		// 衝突範囲が存在しない場合
		if (tMin > tMax) {
			return false;
		}
	}

	// すべての軸で交差している
	return true;
}
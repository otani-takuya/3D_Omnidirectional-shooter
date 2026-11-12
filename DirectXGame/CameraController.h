
#pragma once

#include "KamataEngine.h"

// プレイヤークラスの前方宣言
class Player;

// ==============================
// カメラコントローラー
// ==============================
class CameraController {
public:
	// ==============================
	// 初期化
	// ==============================
	void Initialize(KamataEngine::Camera* camera);

	// ==============================
	// 更新
	// ==============================
	void Update();

	// ==============================
	// 追従対象の設定
	// ==============================
	void Settarget(Player* target) { target_ = target; }

	// ==============================
	// カメラ位置のリセット
	// ==============================
	void Reset();

private:
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 追従対象
	Player* target_ = nullptr;

	// ==============================
	// カメラのオフセット
	// ==============================

	// プレイヤーから見たカメラ位置
	// Y：高さ
	// Z：プレイヤー後方への距離
	KamataEngine::Vector3 targetoffset_ = {0.0f, 15.0f, -15.0f};

	// カメラの目標座標
	KamataEngine::Vector3 cameraTargetPosition_ = {};
};


#include "CameraController.h"
#include "Player.h"

#include <cmath>

// ==============================
// 初期化
// ==============================
void CameraController::Initialize(KamataEngine::Camera* camera) {

	// カメラを保持
	camera_ = camera;

	// カメラ初期化
	camera_->Initialize();

	// ==============================
	// カメラ回転
	// ==============================

	// 斜め上からプレイヤーを見下ろす
	camera_->rotation_ = {std::atan2(-targetoffset_.z, targetoffset_.y), 0.0f, 0.0f};

	camera_->UpdateMatrix();
}

// ==============================
// 更新
// ==============================
void CameraController::Update() {

	// 安全チェック
	if (camera_ == nullptr || target_ == nullptr) {
		return;
	}

	// プレイヤーのワールド座標
	KamataEngine::Vector3 playerPosition = target_->GetWorldPosition();

	// ==============================
	// カメラの目標位置
	// ==============================

	cameraTargetPosition_ = {playerPosition.x + targetoffset_.x, playerPosition.y + targetoffset_.y, playerPosition.z + targetoffset_.z};

	// ==============================
	// カメラを即座に追従
	// ==============================

	// Lerp補間を使用しない
	// プレイヤーを常に画面中央に維持
	camera_->translation_ = cameraTargetPosition_;

	// カメラ行列更新
	camera_->UpdateMatrix();
}

// ==============================
// リセット
// ==============================
void CameraController::Reset() {

	// 安全チェック
	if (camera_ == nullptr || target_ == nullptr) {
		return;
	}

	// プレイヤーのワールド座標
	KamataEngine::Vector3 playerPosition = target_->GetWorldPosition();

	// ==============================
	// カメラ位置を初期化
	// ==============================

	camera_->translation_ = {playerPosition.x + targetoffset_.x, playerPosition.y + targetoffset_.y, playerPosition.z + targetoffset_.z};

	// 行列更新
	camera_->UpdateMatrix();
}

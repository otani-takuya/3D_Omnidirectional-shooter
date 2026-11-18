
#define NOMINMAX

#include "PlayerShooting.h"
#include "BulletManager.h"
#include "Player.h"

#include <Windows.h>
#include <cassert>
#include <cmath>

using namespace KamataEngine;

// ==============================
// 初期化
// ==============================
void PlayerShooting::Initialize(Player* player, BulletManager* bulletManager, Camera* camera) {

	assert(player != nullptr);
	assert(bulletManager != nullptr);
	assert(camera != nullptr);

	player_ = player;
	bulletManager_ = bulletManager;
	camera_ = camera;

	// 射撃状態を初期化
	fireCooldown_ = 0;
	wasMousePressed_ = false;

	// 初期照準方向
	aimDirection_ = {0.0f, 0.0f, 1.0f};

	waitForMouseRelease_ = false;

}

// ==============================
// マウス照準方向の更新
// ==============================
bool PlayerShooting::UpdateAimDirection() {

	// カメラ・プレイヤーの確認
	if (!player_ || !camera_) {
		return false;
	}

	// ==============================
	// マウス座標取得
	// ==============================

	// ゲームのウィンドウを取得
	HWND hwnd = GetActiveWindow();

	if (hwnd == nullptr) {
		return false;
	}

	// マウスのスクリーン座標を取得
	POINT mousePosition{};

	if (!GetCursorPos(&mousePosition)) {
		return false;
	}

	// スクリーン座標からクライアント座標へ変換
	if (!ScreenToClient(hwnd, &mousePosition)) {
		return false;
	}

	// 描画領域のサイズを取得
	RECT clientRect{};

	if (!GetClientRect(hwnd, &clientRect)) {
		return false;
	}

	const float width = static_cast<float>(clientRect.right - clientRect.left);

	const float height = static_cast<float>(clientRect.bottom - clientRect.top);

	if (width <= 0.0f || height <= 0.0f) {
		return false;
	}

	// ==============================
	// NDC座標へ変換
	// ==============================

	const float ndcX = (static_cast<float>(mousePosition.x) / width) * 2.0f - 1.0f;

	const float ndcY = 1.0f - (static_cast<float>(mousePosition.y) / height) * 2.0f;



	// ==============================
	// 投影行列からレイ方向を計算
	// ==============================

	// カメラの投影行列の拡大率を取得
	const float projectionX = camera_->matProjection.m[0][0];
	const float projectionY = camera_->matProjection.m[1][1];

	// ゼロ除算を防止
	if (std::abs(projectionX) < 0.0001f || std::abs(projectionY) < 0.0001f) {
		return false;
	}

	// マウス位置をカメラ空間へ変換
	const float localX = ndcX / projectionX;
	const float localY = ndcY / projectionY;

	// ==============================
	// カメラの回転を反映
	// ==============================

	// カメラのX軸回転
	const float pitch = camera_->rotation_.x;

	const float cosPitch = std::cos(pitch);
	const float sinPitch = std::sin(pitch);

	// カメラローカル空間からワールド方向へ変換
	// カメラは斜め下を向いている
	Vector3 rayDirection = {localX, localY * cosPitch - sinPitch, localY * sinPitch + cosPitch};

	// レイとプレイヤーの高さの水平面を交差
	const Vector3 playerPosition = player_->GetWorldPosition();

	if (std::abs(rayDirection.y) < 0.0001f) {
		return false;
	}

	const float t = (playerPosition.y - camera_->translation_.y) / rayDirection.y;

	if (t <= 0.0f) {
		return false;
	}

	const Vector3 aimPosition = {camera_->translation_.x + rayDirection.x * t, playerPosition.y, camera_->translation_.z + rayDirection.z * t};

	// プレイヤーから照準位置への方向
	Vector3 direction = {aimPosition.x - playerPosition.x, 0.0f, aimPosition.z - playerPosition.z};

	const float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	if (length < 0.0001f) {
		return false;
	}

	aimDirection_ = {direction.x / length, 0.0f, direction.z / length};

	return true;
}

// ==============================
// 射撃処理
// ==============================
void PlayerShooting::Fire() {

	// プレイヤーのワールド座標
	const Vector3 playerPosition = player_->GetWorldPosition();

	// プレイヤーのY軸回転
	const float rotationY = player_->GetWorldTransform()->rotation_.y;

	const float cosY = std::cos(rotationY);
	const float sinY = std::sin(rotationY);

	// ==============================
	// ローカル座標をワールド座標へ変換
	// ==============================

	// Y軸回転を考慮した発射位置
	const Vector3 muzzleOffset = {kMuzzleOffset.x * cosY + kMuzzleOffset.z * sinY, kMuzzleOffset.y, -kMuzzleOffset.x * sinY + kMuzzleOffset.z * cosY};

	// 最終的な弾の発射位置
	const Vector3 bulletPosition = {playerPosition.x + muzzleOffset.x, playerPosition.y + muzzleOffset.y, playerPosition.z + muzzleOffset.z};

	// 照準方向へ弾を発射
	bulletManager_->Fire(bulletPosition, aimDirection_);
}

// ==============================
// 毎フレーム更新
// ==============================
void PlayerShooting::Update() {

	// 必要な参照先を確認
	if (!player_ || !bulletManager_ || !camera_) {
		return;
	}

	// プレイヤー死亡中は射撃しない
	if (player_->IsDead()) {
		fireCooldown_ = 0;
		wasMousePressed_ = false;
		return;
	}

	// ==============================
	// マウス照準の更新
	// ==============================

	//マウス照準を更新
	const bool aimUpdated = UpdateAimDirection();

	// 計算に成功した場合だけ照準方向を更新
	if (aimUpdated) {
		player_->SetAimDirection(aimDirection_);
	}

	// ==============================
	// マウス入力
	// ==============================

	HWND hwnd = GetActiveWindow();

	// ゲームが非アクティブなら射撃しない
	if (hwnd == nullptr || GetForegroundWindow() != hwnd) {

		ResetInput();
		return;
	}

	// 左クリック状態を取得
	const bool isMousePressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	// ==============================
	// 入力再開待ち
	// ==============================

	if (waitForMouseRelease_) {

		// 左クリックが押されている間は射撃しない
		if (isMousePressed) {
			return;
		}

		// ボタンを離したら入力を再開
		waitForMouseRelease_ = false;
		fireCooldown_ = 0;
		wasMousePressed_ = false;

		return;
	}

	// マウスを離したら連射状態をリセット
	if (!isMousePressed) {
		fireCooldown_ = 0;
		wasMousePressed_ = false;
		return;
	}

	// 押した瞬間かどうか
	const bool isTrigger = isMousePressed && !wasMousePressed_;

	// 前フレームの入力を保存
	wasMousePressed_ = isMousePressed;

	// ==============================
	// 射撃クールタイム
	// ==============================

	// 押した瞬間は必ず射撃
	if (isTrigger) {

		Fire();
		fireCooldown_ = kFireInterval;
		return;
	}

	// 長押し中はクールタイムを減らす
	if (fireCooldown_ > 0) {
		--fireCooldown_;
	}

	// クールタイム終了で次の弾を発射
	if (fireCooldown_ == 0) {

		Fire();

		// 次の射撃までの待機時間
		fireCooldown_ = kFireInterval;
	}
}

// ==============================
// 射撃入力状態のリセット
// ==============================
void PlayerShooting::ResetInput() {

	fireCooldown_ = 0;
	wasMousePressed_ = false;

	// 入力がリセットされた後は
	// 一度クリックを離すまで射撃を禁止
	waitForMouseRelease_ = true;
}
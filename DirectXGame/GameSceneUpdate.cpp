#include "GameScene.h"
#include "math.h"

using namespace KamataEngine;

// ==============================
// Update
// 現在のフェーズに応じてゲームを更新し、最後にカメラとデバッグUIを更新する。
// ==============================
void GameScene::Update() {

	// 先に状態を遷移させ、そのフレームで実行するフェーズを決定する。
	ChangePhase();

	switch (phase_) {

	case Phase::kFadeIn:

		// スカイドーム更新
		skydome_->Update();

		// プレイヤー更新
		player_->Update();

		// ==============================
		// 敵管理クラスの更新
		// ==============================

		enemyManager_.Update();

		// エフェクト更新
		UpdateEffects();

		// ブロックのワールド行列更新
		UpdateBlocks();

		// カメラ更新
		cameraController_->Update();
		camera_.UpdateMatrix();

		// フェード更新
		fade_->Update();

		break;

	case Phase::kPlay:

		// スカイドーム更新
		skydome_->Update();

		// ==============================
		// プレイヤー更新
		// ==============================

		player_->Update();

		// ==============================
		// 敵管理クラスの更新
		// ==============================

		enemyManager_.Update();

		// ==============================
		// カメラ更新
		// ==============================

		cameraController_->Update();
		camera_.UpdateMatrix();

		// ==============================
		// 射撃処理
		// ==============================

		if (!isDebugCameraActive_) {

			// 通常カメラでは照準と射撃を更新
			// 直前に更新した通常カメラの行列を使ってマウス照準を計算する。
			playerShooting_.Update();

		} else {

			// デバッグカメラ使用中は射撃状態をリセット
			// デバッグカメラ中の射撃を抑止し、通常視点へ戻った際の入力状態を整える。
			playerShooting_.ResetInput();
		}

		// ==============================
		// 弾の更新
		// ==============================

		bulletManager_.Update();

		// ブロック更新
		UpdateBlocks();

		// すべての当たり判定
		// 弾と敵の座標更新後に判定し、この時点では管理リストを変更しない。
		CheckAllCollisions();

		// ==============================
		// 削除対象の弾を取り除く
		// ==============================

		// 衝突判定の走査が終了してから削除し、走査中の要素無効化を避ける。
		bulletManager_.RemoveDeadBullets();

		// エフェクト更新
		UpdateEffects();

		// 終了したエフェクトを削除
		RemoveDeadEffects();

		// ==============================
		// 死亡した敵を取り除く
		// ==============================

		// HPが0になった直後ではなく、死亡演出が終了した敵を取り除く。
		enemyManager_.RemoveDeadEnemies();

		break;

	case Phase::kDeath:

		// スカイドーム更新
		skydome_->Update();

		// ==============================
		// 敵管理クラスの更新
		// ==============================

		enemyManager_.Update();

		// やられパーティクル更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// カメラ更新
		camera_.UpdateMatrix();

		// ブロック更新
		UpdateBlocks();

		break;

	case Phase::kFadeOut:

		// フェードアウト更新
		fade_->Update();

		break;
	}

	// フェーズ固有の更新後に、最終的な描画用の視点を適用する。
	UpdateCamera();
	DrawDebugUI();
}

// ==============================
// ChangePhase
// フェードやプレイヤーの死亡状態に応じてゲームのフェーズを遷移させる。
// ==============================
void GameScene::ChangePhase() {

	switch (phase_) {

	case Phase::kFadeIn:

		// フェードイン終了後にゲーム開始
		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPlay;
		}

		break;

	case Phase::kPlay:

		// プレイヤーが死亡したらデス演出へ
		if (player_->IsDead()) {

			phase_ = Phase::kDeath;

			const KamataEngine::Vector3& deathParticlePosition = player_->GetWorldPosition();

			deathParticles_->Initialize(deathParticlesModel_, &camera_, deathParticlePosition);
		}

		break;

	case Phase::kDeath:

		// デス演出終了後にフェードアウト開始
		if (deathParticles_ && deathParticles_->IsFinished()) {

			fade_->Start(Fade::Status::FadeOut, kFadeDuration);

			phase_ = Phase::kFadeOut;
		}

		break;

	case Phase::kFadeOut:

		// フェードアウト終了後にゲームシーンを終了
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}
}

// ==============================
// UpdateBlocks
// 配置済みブロックのワールド行列を更新する。空セルは処理しない。
// ==============================
void GameScene::UpdateBlocks() {
	for (auto& line : worldTransformBlocks_) {
		for (auto* wt : line) {
			if (wt) {
				UpdateWorldTransform(*wt);
			}
		}
	}
}

// ==============================
// UpdateEffects
// 管理中のエフェクトを更新する。ここではリストから削除しない。
// ==============================
void GameScene::UpdateEffects() {
	for (auto* effect : effects_) {
		if (effect) {
			effect->Update();
		}
	}
}

// ==============================
// RemoveDeadEffects
// 終了済みエフェクトを解放し、管理リストから取り除く。
// ==============================
void GameScene::RemoveDeadEffects() {
	effects_.remove_if([](BaseEffect* effect) {
		if (!effect || effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});
}

// ==============================
// UpdateCamera
// F1による切り替えと最終的な描画用カメラ行列の適用を行う。
// ==============================
void GameScene::UpdateCamera() {
#ifdef _DEBUG

	// ==============================
	// デバッグカメラ切り替え
	// ==============================

	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// ==============================
	// カメラ適用
	// ==============================

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// デバッグカメラからコピーした行列を描画用の定数バッファへ反映する。
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}
}

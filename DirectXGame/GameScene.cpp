#include "GameScene.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "ResourceManager.h"
#include "StageManager.h"
#include "math.h"
#include <cassert>
#include <cstdint>
#include <string>

using namespace KamataEngine;

// ==============================
// Initialize
// 共有リソースを参照し、カメラ・マップ・各管理クラスを依存順に初期化する。
// ==============================
void GameScene::Initialize(StageManager* stageManager, ResourceManager* resourceManager) {

	assert(stageManager != nullptr);
	assert(resourceManager != nullptr);

	stageManager_ = stageManager;
	resourceManager_ = resourceManager;

	// リロード要求を解除
	reloadRequested_ = false;

	// ==============================
	// テクスチャ読み込み
	// ==============================

	// 描画前にGPUにテクスチャを登録しておく必要がある
	// ハンドルは保持しないが、既存のテクスチャ登録処理は互換性のため残す。
	TextureManager::Load("./Resources/uvChecker.png");

	// ==============================
	// ResourceManagerからモデルを取得
	// ==============================

	// フィールド
	modelBlock_ = resourceManager_->GetModel("block");
	groundModel_ = resourceManager_->GetModel("ground");

	// プレイヤー
	playerModel_ = resourceManager_->GetModel("player");
	attackModel_ = resourceManager_->GetModel("hit_effect");

	// ==============================
	// 弾管理クラスの初期化
	// ==============================

	bulletManager_.Initialize(resourceManager_->GetModel("bullet"));

	// ==============================
	// 衝突判定デバッグ情報の初期化
	// ==============================

	bulletHitCount_ = 0;
	totalBulletHitCount_ = 0;
	isBulletHit_ = false;
	isDrawBulletHitEffect_ = true;

	deathParticlesModel_ = resourceManager_->GetModel("deathparticle");

	// ==============================
	// カメラ初期化
	// ==============================

	// farZを広げる
	camera_.farZ = 1000.0f;

	camera_.Initialize();

	// デバッグ用カメラ（F1で切り替え）
	debugCamera_ = std::make_unique<KamataEngine::DebugCamera>(1280, 720);

	// マップチップフィールド生成
	mapChipField_ = std::make_unique<MapChipField>();

	// 現在のステージデータを取得
	const StageData& stageData = stageManager_->GetCurrentStageData();

	// CSVファイルパスを作成
	const std::string stageFileName = "./Resources/fields/" + stageData.name + ".csv";

	// 現在ステージのCSVを読み込む
	mapChipField_->LoadMapChipCsv(stageFileName);

	// ==============================
	// 敵管理クラスの初期化
	// ==============================

	// モデルはResourceManagerから借用する。マップとカメラが有効になってから初期化する。
	enemyManager_.Initialize(resourceManager_->GetModel("enemy"), resourceManager_->GetModel("shieldEnemy"), &camera_, mapChipField_.get(), this);

	// フィールドオブジェクト生成
	// EnemyManagerとマップの準備後に生成する。敵は生成時に両者の参照を必要とする。
	GenerateFieldObjects();

	// ==============================
	// 床のワールドトランスフォーム初期化
	// ==============================

	// ワールドトランスフォーム初期化
	groundWorldTransform_.Initialize();

	// 床の位置
	groundWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// 床の拡大率
	groundWorldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 床の回転
	groundWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// ワールド行列を更新
	UpdateWorldTransform(groundWorldTransform_);

	// ==============================
	// やられパーティクル生成
	// ==============================

	deathParticles_ = std::make_unique<DeathParticles>();
	deathParticles_->Initialize(deathParticlesModel_, &camera_, player_->GetWorldPosition());

	// ==============================
	// ヒットエフェクト生成
	// ==============================

	HitEffect::SetModel(resourceManager_->GetModel("particle"));
	HitEffect::SetCamera(&camera_);

	// ==============================
	// ガードエフェクト生成
	// ==============================

	GuardEffect::SetModel(resourceManager_->GetModel("ring"));
	GuardEffect::SetCamera(&camera_);

	// スカイドーム生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(resourceManager_->GetModel("skydome"), &camera_);

	// ==============================
	// カメラコントローラー
	// ==============================

	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();

	// 初期化
	cameraController_->Initialize(&camera_);

	// 追従対象をプレイヤーに設定
	// 追従対象は借用するだけで、CameraControllerへ所有権を移さない。
	cameraController_->Settarget(player_.get());

	// プレイヤーの位置にカメラを合わせる
	cameraController_->Reset();

	// 初期カメラを更新
	cameraController_->Update();

	// ==============================
	// PlayerShootingの初期化
	// ==============================

	// 射撃処理には非所有ポインタを渡す。Playerの所有権はGameSceneに残す。
	playerShooting_.Initialize(player_.get(), &bulletManager_, &camera_);

	// ==============================
	// フェード初期化
	// ==============================

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();

	// ゲーム開始時はフェードイン
	phase_ = Phase::kFadeIn;
	finished_ = false;

	fade_->Start(Fade::Status::FadeIn, kFadeDuration);
}

// ==============================
// GenerateFieldObjects
// CSVの各セルからブロック・プレイヤー・敵を生成する。初期化時に一度だけ呼ぶ。
// ==============================
void GameScene::GenerateFieldObjects() {

	// ブロック配列のサイズ確保
	worldTransformBlocks_.resize(kNumBlockVertical);

	for (uint32_t y = 0; y < kNumBlockVertical; ++y) {
		worldTransformBlocks_[y].resize(kNumBlockHorizontal, nullptr);
	}

	// プレイヤーの二重生成防止
	bool isPlayerCreated = false;

	// CSVの全マスを確認
	for (uint32_t y = 0; y < kNumBlockVertical; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {

			// マップチップ種別
			const MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(x, y);

			// マップチップのサブID
			const uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(x, y);

			// マップチップのワールド座標
			KamataEngine::Vector3 position = mapChipField_->GetBlockPositionByIndex(x, y);

			switch (mapChipType) {

				// ==============================
				// ブロック生成
				// ==============================

			case MapChipType::kBlock: {

				WorldTransform* worldTransform = new WorldTransform();

				worldTransform->Initialize();
				worldTransform->translation_ = position;

				UpdateWorldTransform(*worldTransform);

				// 公開配列との互換性のため生ポインタを保持する。終了時にGameSceneが解放する。
				worldTransformBlocks_[y][x] = worldTransform;

				break;
			}

				// ==============================
				// プレイヤー生成
				// ==============================

			case MapChipType::kPlayer: {

				// P0が複数あっても1体だけ生成
				if (isPlayerCreated) {
					break;
				}

				player_ = std::make_unique<Player>();

				player_->Initialize(playerModel_, attackModel_, &camera_, position);

				player_->SetMapChipField(mapChipField_.get());

				isPlayerCreated = true;

				break;
			}

				// ==============================
				// 敵生成
				// ==============================

			case MapChipType::kEnemy: {

				// 敵をブロック上へ配置するための補正
				// 従来の敵配置オフセットを維持し、生成位置を変更しない。
				position.y += 0.5f;

				// 敵の種類に応じて生成
				switch (subID) {

					// ==============================
					// E0：通常敵
					// ==============================

				case 0:

					enemyManager_.Spawn(EnemyManager::EnemyType::kNormal, position);

					break;

					// ==============================
					// E1：シールドエネミー
					// ==============================

				case 1:

					enemyManager_.Spawn(EnemyManager::EnemyType::kShield, position);

					break;

				default:

					// 未対応の敵サブID
					break;
				}

				break;
			}

			case MapChipType::kBlank:
			default:
				break;
			}
		}
	}
}

// ==============================
// 終了処理
// 参照先が破棄される前に管理リストを空にする。
// ==============================
GameScene::~GameScene() {
	// 参照先が生存している間に敵・弾・エフェクトを解放
	enemyManager_.Clear();
	bulletManager_.Clear();
	for (BaseEffect* effect : effects_) {
		delete effect;
	}
	effects_.clear();
	for (auto& line : worldTransformBlocks_) {
		for (auto* wt : line) {
			delete wt;
		}
	}
	worldTransformBlocks_.clear();
	// その他の所有オブジェクトは unique_ptr が自動解放する。
}

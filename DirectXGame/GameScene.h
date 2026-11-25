#pragma once

#include "BaseEffect.h"
#include "BulletManager.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "EnemyManager.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "PlayerShooting.h"
#include "Skydome.h"
#include <list>
#include <memory>
#include <vector>

class ResourceManager;
class StageManager;

/// <summary>
/// ゲーム全体の進行と各管理クラスの連携を担当する。
/// 弾・敵の生成と寿命管理は専用Managerに委譲する。
/// Initializeは新規インスタンスに対して一度だけ呼び出す。
/// </summary>
class GameScene {
public:
	GameScene() = default;
	~GameScene();
	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = delete;

	/// <summary>
	/// 共有リソースを参照し、カメラ・マップ・各管理クラスを依存順に初期化する。
	/// </summary>
	/// <param name="stageManager">ステージ管理への非所有参照。シーンより長く生存すること。</param>
	/// <param name="resourceManager">共有リソースの所有者。シーンより長く生存すること。</param>
	void Initialize(StageManager* stageManager, ResourceManager* resourceManager);
	/// <summary>
	/// 現在のフェーズに応じてゲームを更新し、最後にカメラとデバッグUIを更新する。
	/// </summary>
	void Update();
	/// <summary>
	/// 既存の描画順序でフィールド、キャラクター、演出を描画する。
	/// </summary>
	void Draw();
	/// <summary>
	/// CSVの各セルからブロック・プレイヤー・敵を生成する。初期化時に一度だけ呼ぶ。
	/// </summary>
	void GenerateFieldObjects();
	/// <summary>
	/// ゲーム内の衝突判定をまとめて呼び出す。現在は弾と敵の判定を担当する。
	/// </summary>
	void CheckAllCollisions();
	/// <summary>
	/// フェードやプレイヤーの死亡状態に応じてゲームのフェーズを遷移させる。
	/// </summary>
	void ChangePhase();
	/// <summary>
	/// 指定されたワールド座標に命中エフェクトを生成し、シーンの管理リストに登録する。
	/// </summary>
	/// <param name="position">生成位置（ワールド座標）。</param>
	void CreateHitEffect(const KamataEngine::Vector3& position);
	/// <summary>
	/// 指定されたワールド座標にガードエフェクトを生成し、シーンの管理リストに登録する。
	/// </summary>
	/// <param name="position">生成位置（ワールド座標）。</param>
	void CreateGuardEffect(const KamataEngine::Vector3& position);
	/// <summary>
	/// ゲームシーンの終了条件を満たしたか取得する。
	/// </summary>
	/// <returns>条件を満たす場合はtrue。</returns>
	bool IsFinished() const { return finished_; }
	/// <summary>
	/// マップの再読み込み要求が発生したか取得する。
	/// </summary>
	/// <returns>条件を満たす場合はtrue。</returns>
	bool IsReloadRequested() const { return reloadRequested_; }

	// ==============================
	// ゲームの進行状態
	// ==============================
	enum class Phase {
		kFadeIn, // 開始演出
		kPlay,   // 通常プレイ
		kDeath,  // プレイヤー死亡演出
		kFadeOut // 終了演出
	};
	Phase phase_ = Phase::kFadeIn;

	// 既存の外部アクセスとの互換性を維持。GameSceneが解放する。
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	std::list<BaseEffect*> effects_;

private:
	/// <summary>
	/// 配置済みブロックのワールド行列を更新する。空セルは処理しない。
	/// </summary>
	void UpdateBlocks();
	/// <summary>
	/// 管理中のエフェクトを更新する。ここではリストから削除しない。
	/// </summary>
	void UpdateEffects();
	/// <summary>
	/// 終了済みエフェクトを解放し、管理リストから取り除く。
	/// </summary>
	void RemoveDeadEffects();
	/// <summary>
	/// F1による切り替えと最終的な描画用カメラ行列の適用を行う。
	/// </summary>
	void UpdateCamera();
	/// <summary>
	/// Debug構成でシーン設定、衝突情報、敵情報を表示する。
	/// </summary>
	void DrawDebugUI();
	/// <summary>
	/// 弾の移動線分と敵のAABBを比較し、命中時にダメージと削除要求を設定する。
	/// </summary>
	void CheckBulletEnemyCollisions();

	// 共有モデル：ResourceManagerが所有する非所有ポインタ
	// ブロック描画用モデル
	KamataEngine::Model* modelBlock_ = nullptr;
	// 床描画用モデル
	KamataEngine::Model* groundModel_ = nullptr;
	// プレイヤー初期化用モデル
	KamataEngine::Model* playerModel_ = nullptr;
	// プレイヤー攻撃用モデル
	KamataEngine::Model* attackModel_ = nullptr;
	// 死亡演出の再初期化にも使用するモデル
	KamataEngine::Model* deathParticlesModel_ = nullptr;
	// 現在のステージ情報を借用する
	StageManager* stageManager_ = nullptr;
	// 共有モデルを借用する。deleteしない
	ResourceManager* resourceManager_ = nullptr;

	// 所有オブジェクト。依存先を先に宣言し、逆順で破棄する。
	// 通常カメラ。各オブジェクトが参照する
	KamataEngine::Camera camera_;
	// 床の位置・回転・拡大率とワールド行列
	KamataEngine::WorldTransform groundWorldTransform_;
	// CSVマップ情報の所有者
	std::unique_ptr<MapChipField> mapChipField_;
	// プレイヤーの所有者
	std::unique_ptr<Player> player_;
	// 自由視点カメラの所有者
	std::unique_ptr<KamataEngine::DebugCamera> debugCamera_;
	// 追従制御の所有者。Playerを借用する
	std::unique_ptr<CameraController> cameraController_;
	// プレイヤー死亡演出の所有者
	std::unique_ptr<DeathParticles> deathParticles_;
	// 背景モデル描画オブジェクトの所有者
	std::unique_ptr<Skydome> skydome_;
	// 画面遷移演出の所有者
	std::unique_ptr<Fade> fade_;

	// 弾の生成・更新・削除を担当
	BulletManager bulletManager_;
	// 敵の生成・更新・削除を担当
	EnemyManager enemyManager_;
	// 照準・射撃入力を担当。Player等を借用する
	PlayerShooting playerShooting_;

	// 自由視点の有効状態
	bool isDebugCameraActive_ = false;
	// シーン終了フラグ
	bool finished_ = false;
	// マップ再読み込み要求
	bool reloadRequested_ = false;
	// フェード時間（秒）
	static constexpr float kFadeDuration = 1.0f;

	// 衝突判定のデバッグ情報
	// 直近の衝突判定で命中した弾数
	int bulletHitCount_ = 0;
	// 累計命中数（デバッグ操作でリセット可能）
	int totalBulletHitCount_ = 0;
	// 直近の衝突判定で命中があったか
	bool isBulletHit_ = false;
	// 命中エフェクトの生成を許可するか
	bool isDrawBulletHitEffect_ = true;
};

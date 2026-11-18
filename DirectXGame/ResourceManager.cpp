
#include "ResourceManager.h"

#include <cassert>

using namespace KamataEngine;

// ==============================
// デストラクタ
// ==============================
ResourceManager::~ResourceManager() {

	// 解放忘れを防止
	Finalize();
}

// ==============================
// 初期化
// ==============================
void ResourceManager::Initialize() {

	// 二重初期化防止
	if (initialized_) {
		return;
	}

	// ==============================
	// 標準モデル
	// ==============================
	LoadDefaultModel("default");

	// ==============================
	// フィールド関連
	// ==============================
	LoadModel("block");
	LoadModel("ground");
	LoadModel("skydome");

	// ==============================
	// プレイヤー関連
	// ==============================
	LoadModel("player");
	LoadModel("hit_effect");

	// ==============================
	// 敵関連
	// ==============================
	LoadModel("enemy");
	LoadModel("shieldEnemy");

	// ==============================
	// エフェクト関連
	// ==============================
	LoadModel("deathparticle");
	LoadModel("particle");
	LoadModel("ring");

	// ==============================
	// 弾モデル
	// ==============================
	LoadDefaultModel("bullet");

	initialized_ = true;
}

// ==============================
// OBJモデルの読み込み
// ==============================
void ResourceManager::LoadModel(const std::string& name) {

	// 登録済みなら再読み込みしない
	if (models_.contains(name)) {
		return;
	}

	// モデル生成
	Model* model = Model::CreateFromOBJ(name, true);

	assert(model != nullptr);

	if (model == nullptr) {
		return;
	}

	// 所有権をResourceManagerへ移す
	models_.emplace(name, std::unique_ptr<Model>(model));
}

// ==============================
// 標準モデルの読み込み
// ==============================
void ResourceManager::LoadDefaultModel(const std::string& name) {

	if (models_.contains(name)) {
		return;
	}

	Model* model = Model::Create();

	assert(model != nullptr);

	if (model == nullptr) {
		return;
	}

	models_.emplace(name, std::unique_ptr<Model>(model));
}

// ==============================
// モデル取得
// ==============================
Model* ResourceManager::GetModel(const std::string& name) const {

	auto it = models_.find(name);

	// 未登録モデル
	if (it == models_.end()) {
		assert(false && "モデルが登録されていません");
		return nullptr;
	}

	// 所有権は渡さずポインタだけ返す
	return it->second.get();
}

// ==============================
// 全リソース解放
// ==============================
void ResourceManager::Finalize() {

	// 所有しているモデルを一括解放
	models_.clear();

	initialized_ = false;
}

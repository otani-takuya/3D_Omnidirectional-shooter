
#pragma once

#include "KamataEngine.h"

#include <memory>
#include <string>
#include <unordered_map>

// ==============================
// リソース管理クラス
// ==============================
class ResourceManager {
public:
	// モデルの事前読み込み
	void Initialize();

	// 全リソースの解放
	void Finalize();

	// 登録済みモデルの取得
	KamataEngine::Model* GetModel(const std::string& name) const;

	// コピー禁止
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	ResourceManager() = default;
	~ResourceManager();

private:
	// OBJモデルを登録
	void LoadModel(const std::string& name);

	// 標準モデルを登録
	void LoadDefaultModel(const std::string& name);

	// モデルの所有権を一元管理
	std::unordered_map<std::string, std::unique_ptr<KamataEngine::Model>> models_;

	// 初期化済みフラグ
	bool initialized_ = false;
};

#include "SceneManager.h"
#include<toolkit/Logging.h>

cherry::Scene* cherry::SceneManager::_CurrentScene = nullptr;
std::unordered_map<std::string, cherry::Scene*> cherry::SceneManager::_KnownScenes;
entt::registry cherry::SceneManager::Prefabs;

cherry::Scene* cherry::SceneManager::Current() {
	return _CurrentScene;
}
bool cherry::SceneManager::HasScene(const std::string& name) {
	auto it = _KnownScenes.find(name);
	if (it != _KnownScenes.end()) {
		return true;
	}
	return false;
}
cherry::Scene* cherry::SceneManager::Get(const std::string& name) {
	if (!HasScene(name))
		return nullptr;
	else
		return _KnownScenes[name];
}

bool cherry::SceneManager::SetCurrentScene(const std::string& name) {
	auto it = _KnownScenes.find(name);
	if (it != _KnownScenes.end()) {
		if (_CurrentScene != it->second) {
			
			// this would be null if it's the first time we're making the scene. We don't want to close null.
			if (_CurrentScene != nullptr)
				_CurrentScene->OnClose();
			_CurrentScene = it->second;
			_CurrentScene->OnOpen();
		}
		return true;
	}
	return false;
}

void cherry::SceneManager::RegisterScene(const std::string& name, Scene* scene) {
	LOG_ASSERT(!HasScene(name), "A scene with that name already exists!");
	if (scene == nullptr)
		scene = new Scene();

	// we just reference it and put it in. Everything else is done internally.
	_KnownScenes[name] = scene;
}

cherry::SceneManager::SceneIterator cherry::SceneManager::Each() { return SceneIterator(); }

// interates over every scene and deletes it.
void cherry::SceneManager::DestroyScenes() {
	for (auto& kvp : _KnownScenes) {
		delete kvp.second;
	}
	_KnownScenes.clear();
	_CurrentScene = nullptr;
}
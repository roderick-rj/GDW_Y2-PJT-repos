#include "GameplayScene.h"
#include "..\Game.h"

// constructor
cherry::GameplayScene::GameplayScene(const std::string name)
	: Scene(name)
{
}

// called when the scene is being opened.
void cherry::GameplayScene::OnOpen()
{
	Game* game = Game::GetRunningGame();
	glm::ivec2 myWindowSize = game->GetWindowSize();

	ObjectManager::CreateSceneObjectList(GetName());
	objectList = ObjectManager::GetSceneObjectListByName(GetName());

	LightManager::CreateSceneLightList(GetName());
	lightList = LightManager::GetSceneLightListByName(GetName());
}

// caled when the scene is being closed.
void cherry::GameplayScene::OnClose()
{
	ObjectManager::DestroySceneObjectListByPointer(objectList); // TODO: crashes here after 3 successful deletions.
	LightManager::DestroySceneLightListByPointer(lightList);

	// now uses shared pointers
	// for (PostLayer* layer : layers)
	// 	delete layer;

	layers.clear();

	// this is empty
	Scene::OnClose();
}

// generates a new instance of the gameplay scene.
cherry::Scene* cherry::GameplayScene::GenerateNewInstance() const
{
	return new GameplayScene(GetName());
}

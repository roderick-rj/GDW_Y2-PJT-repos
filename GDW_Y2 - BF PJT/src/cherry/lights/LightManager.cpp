// LightManager - manages all lights for the game
#include "LightManager.h"

#include "..\objects/ObjectManager.h"
#include "..\utils/Utils.h"
#include "..\post/PostLayer.h"
#include "..\Game.h"

std::vector<cherry::LightList*> cherry::LightManager::lightLists = std::vector<cherry::LightList*>();

// destructor
cherry::LightManager::~LightManager()
{
	// deletes all ight lists.
	for (LightList* ll : lightLists)
	{
		delete ll;
	}

	lightLists.clear();
}

// checks if a scene name has been taken.
bool cherry::LightManager::SceneLightListExists(const std::string sceneName)
{
	// checks if the scene name has been used.
	for (cherry::LightList * ll : lightLists)
	{
		if (ll->GetSceneName() == sceneName)
			return true;
	}

	return false;
}

// gets a light list via its index in the list.
cherry::LightList* cherry::LightManager::GetSceneLightListByIndex(unsigned int index)
{
	if (index >= lightLists.size()) // index out of bounds
		return nullptr;

	return lightLists[index];
}

// returns a light list for a scene.
cherry::LightList* cherry::LightManager::GetSceneLightListByName(std::string sceneName)
{
	// finds if a scene exists
	for (cherry::LightList* ll : lightLists)
	{
		if (ll->GetSceneName() == sceneName) // if the scene exists
		{
			return ll; // returns a pointer to the light list.
			break;
		}
	}

	return nullptr;
}

// adds a scene to the light manager.
bool cherry::LightManager::CreateSceneLightList(const std::string sceneName)
{
	// if the scene name already exists in the list.
	if (SceneLightListExists(sceneName))
	{
		return false;
	}
	else // if the scene doesn't exist in the list.
	{
		// adds a scene, and an associated list of lights.
		lightLists.push_back(new LightList(sceneName));
		return true;
	}
}

// adds a light to the scene.
bool cherry::LightManager::AddLightToSceneLightList(cherry::Light * light, bool addScene)
{
	if (light == nullptr) // light is nullptr
		return false;

	std::string sceneName = light->GetScene();
	// finds the scene
	for (cherry::LightList* ll : lightLists)
	{
		if (ll->GetSceneName() == sceneName) // if the scene exists
		{
			ll->AddLight(light); // returns a pointer to the light list.
			return true;
			break;
		}
	}

	// if the scene should be added if it doesn't already exist.
	if (addScene)
	{
		CreateSceneLightList(sceneName); // adds the scene
		GetSceneLightListByName(sceneName)->AddLight(light); // adds the light
		return true;
	}

	return false;
}

// applies lights to the objects provided.
void cherry::LightManager::ApplyLightsToObjects(cherry::ObjectList* objList)
{
	cherry::LightList* ll;
	
	if (objList == nullptr)
		return;

	ll = LightManager::GetSceneLightListByName(objList->GetSceneName());

	// light doesn't exist.
	if (ll == nullptr)
		return;


	// going through each object and applying the lights.
	cherry::Material::Sptr mat;
	for (cherry::Object* object : objList->GetObjects())
	{
		mat = object->GetMaterial();
		ll->ApplyLights(mat, MAX_LIGHTS);
	}
}

// gets all lights in the scene as a single light.
cherry::Light* cherry::LightManager::GetSceneLightsMerged(std::string sceneName)
{
	// gets the light list
	cherry::LightList* sceneLight = GetSceneLightListByName(sceneName);

	if (sceneLight != nullptr)
	{
		return sceneLight->GetLightsMerged();
	}
	else
	{
		return nullptr;
	}
}

// destroys a light list via its index
bool cherry::LightManager::DestroySceneLightListByIndex(unsigned int index)
{
	LightList* lgtList = GetSceneLightListByIndex(index);

	if (lgtList != nullptr) // if the object isn't a nullptr
	{
		util::removeFromVector(lightLists, lgtList);
		delete lgtList;

		return true;
	}
	else // if the object is a nullptr
	{
		return false;
	}
}

// destroys the light list via its pointer.
bool cherry::LightManager::DestroySceneLightListByPointer(cherry::LightList* ll)
{
	if (util::removeFromVector(lightLists, ll)) // in list
	{
		delete ll;
		return true;
	}
	else // was not in list
	{
		return false;
	}
}

// destroys a scene light list via its name
bool cherry::LightManager::DestroySceneLightListByName(std::string sceneName)
{
	LightList* ll = GetSceneLightListByName(sceneName); // destroys the light list.

	return DestroySceneLightListByPointer(ll);
}

// destroys all scene light lists
void cherry::LightManager::DestroyAllSceneLightLists()
{
	// while there are still object lists to be deleted.
	while (!lightLists.empty())
	{
		LightList* temp = lightLists[0];
		util::removeFromVector(lightLists, temp);
		delete temp;
	}
}

// LIGHT LIST
// constructor
cherry::LightList::LightList(std::string scene) : scene(scene) 
{
	// gets the window size
	glm::ivec2 windowSize = Game::GetRunningGame()->GetWindowSize();

	// shader
	shader = std::make_shared<Shader>();
	shader->Load(POST_VS, POST_LIGHTING_FS);

	// frame buffer
	frameBuffer = FrameBuffer::GenerateDefaultBuffer();

	// frameBuffer = std::make_shared<FrameBuffer>(windowSize.x, windowSize.y);
	// 
	// // buffer color
	// RenderBufferDesc bufferColor = RenderBufferDesc();
	// bufferColor.ShaderReadable = true;
	// bufferColor.Attachment = RenderTargetAttachment::Color0;
	// bufferColor.Format = RenderTargetType::Color24; // loads with RGB
	// 
	// // buffer depth
	// RenderBufferDesc bufferDepth = RenderBufferDesc();
	// bufferDepth.ShaderReadable = true;
	// bufferDepth.Attachment = RenderTargetAttachment::Depth;
	// bufferDepth.Format = RenderTargetType::Depth24;
	// 
	// // frame buffer
	// frameBuffer->AddAttachment(bufferColor);
	// frameBuffer->AddAttachment(bufferDepth);

	// makes the layer
	layer = std::make_shared<PostLayer>(shader, frameBuffer);

	// gets the shadow shader
	shadowShader = std::make_shared<Shader>();
	shadowShader->Load(POST_VS, POST_POINT_SHADOW_FS);
	
	// gets the shadow buffer
	shadowBuffer = FrameBuffer::GenerateDefaultBuffer();
	
	// creates a shadow layer.
	shadowLayer = std::make_shared<PostLayer>(shadowShader, shadowBuffer);
}

// destructor
cherry::LightList::~LightList()
{
	for (Light* light : lights)
		delete light;

	lights.clear();
}

// gets the name of the scene the light list is for.
std::string cherry::LightList::GetSceneName() const { return scene; }

// gets the light list
int cherry::LightList::GetLightCount() const { return lights.size(); }

// gets the lights as a vector
// TODO: maybe make this const
std::vector<cherry::Light*> cherry::LightList::GetLights() { return lights; }

// adds a light to the list
bool cherry::LightList::AddLight(cherry::Light* light)
{
	if(light == nullptr) // checking for nullptr
		return false;

	return util::addToVector(lights, light);

	// limiter
	// adding to the vector if the vector isn't full.
	// if (lights.size() < MAX_LIGHTS)
	// 	return util::addToVector(lights, light);
	// else
	// 	return false;
}

// gets lights averaged
cherry::Light* cherry::LightList::GetLightsMerged()
{
	cherry::Light* sceneLight = nullptr; // returns the scene light

	cherry::Vec3 lightPos; // light position
	cherry::Vec3 lightClr; // colour

	cherry::Vec3 ambiClr; // ambient colour
	float ambiPwr = 0; // ambient power

	float lightSpecPwr = 0; // specular power
	float lightShine = 0; // shininess
	float lightAtten = 0; // attenuation

	// adds together all the values for each light.
	for (int i = 0; i < lights.size(); i++)
	{
		lightPos += lights.at(i)->GetLightPosition();
		lightClr += lights.at(i)->GetLightColor();

		ambiClr += lights.at(i)->GetAmbientColor();
		ambiPwr += lights.at(i)->GetAmbientPower();

		lightSpecPwr += lights.at(i)->GetLightSpecularPower();
		lightShine += lights.at(i)->GetLightShininess();
		lightAtten += lights.at(i)->GetLightAttenuation();
	}

	// creates a single light for the whole scene by averaging the values.
	sceneLight = new Light(scene,
		lightPos / lights.size(),
		lightClr / lights.size(),
		ambiClr / lights.size(),
		ambiPwr / lights.size(),
		lightSpecPwr / lights.size(),
		lightShine / lights.size(),
		lightAtten / lights.size()
	);

	return sceneLight;
}

// generates a material
cherry::Material::Sptr cherry::LightList::GenerateMaterial(std::string vs, std::string fs) const
{
	return GenerateMaterial(vs, fs, nullptr);
}

// generates the material
cherry::Material::Sptr cherry::LightList::GenerateMaterial(std::string vs, std::string fs, const TextureSampler::Sptr& sampler) const
{
	if(lights.size() == 0) // no lights
		return cherry::Material::Sptr();

	Material::Sptr material; // the material
	Shader::Sptr phong = std::make_shared<Shader>(); // shader
	Texture2D::Sptr albedo = Texture2D::LoadFromFile("res/images/default.png"); // texture

	glm::vec3 temp; // temporary vector
	
	int lightCount = 0; // total amount of lights

	// Must align with the macro MAX_LIGHTS, which is defined in the light manager and the shaders.
	lightCount = (lights.size() > MAX_LIGHTS) ? MAX_LIGHTS : lights.size();

	// used to make the albedo // TODO: fix shaders
	phong->Load(vs.c_str(), fs.c_str()); // the shader
	material = std::make_shared<Material>(phong); // loads in the shader.
	 
	material->Set("a_EnabledLights", lightCount);

	// goes through each light, getting the values.
	for (int i = 0; i < lightCount; i++)
	{
		temp = lights.at(i)->GetAmbientColorGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].ambientColor", { temp[0], temp[1], temp[2] }); // ambient colour

		material->Set("a_Lights[" + std::to_string(i) + "].ambientPower", lights.at(i)->GetAmbientPower()); // ambient power
		material->Set("a_Lights[" + std::to_string(i) + "].specularPower", lights.at(i)->GetLightSpecularPower()); // specular power

		temp = lights.at(i)->GetLightPositionGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].position", { temp[0], temp[1], temp[2] }); // position

		temp = lights.at(i)->GetLightColorGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].color", { temp[0], temp[1], temp[2] }); // light colour

		material->Set("a_Lights[" + std::to_string(i) + "].shininess", lights.at(i)->GetLightShininess()); // shininess
		material->Set("a_Lights[" + std::to_string(i) + "].attenuation", lights.at(i)->GetLightAttenuation()); // attenuation
	}

	
	// albedo values (with sampler)
	if (sampler != nullptr)
	{
		material->Set("s_Albedos[0]", albedo, sampler);
		material->Set("s_Albedos[1]", albedo, sampler);
		material->Set("s_Albedos[2]", albedo, sampler);
	}
	else // no sampler provided
	{
		material->Set("s_Albedos[0]", albedo);
		material->Set("s_Albedos[1]", albedo);
		material->Set("s_Albedos[2]", albedo);
	}

	 
	// returns the material
	return material;
}

// gets the post layer for this light list.
cherry::PostLayer::Sptr cherry::LightList::GetPostLayer() const { return layer; }

// gets the shadow layer
cherry::PostLayer::Sptr cherry::LightList::GetShadowLayer() const { return shadowLayer; }

// applies all the lights in the list.
void cherry::LightList::ApplyLights(cherry::Material::Sptr& material) { ApplyLights(material,lights.size()); }

// applies the lighting to the material.
void cherry::LightList::ApplyLights(cherry::Material::Sptr & material, int lightCount)
{
	glm::vec3 temp; // temporary glm::vector
	int enabledLights = (abs(lightCount) < MAX_LIGHTS) ? abs(lightCount) : MAX_LIGHTS;

	// setting the light count.
	material->Set("a_EnabledLights", enabledLights);

	// goes through each light, getting the values.
	for (int i = 0; i < enabledLights; i++)
	{
		temp = lights.at(i)->GetAmbientColorGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].ambientColor", { temp[0], temp[1], temp[2] }); // ambient colour

		material->Set("a_Lights[" + std::to_string(i) + "].ambientPower", lights.at(i)->GetAmbientPower()); // ambient power
		material->Set("a_Lights[" + std::to_string(i) + "].specularPower", lights.at(i)->GetLightSpecularPower()); // specular power

		temp = lights.at(i)->GetLightPositionGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].position", { temp[0], temp[1], temp[2] }); // position

		temp = lights.at(i)->GetLightColorGLM();
		material->Set("a_Lights[" + std::to_string(i) + "].color", { temp[0], temp[1], temp[2] }); // light colour

		material->Set("a_Lights[" + std::to_string(i) + "].shininess", lights.at(i)->GetLightShininess()); // shininess
		material->Set("a_Lights[" + std::to_string(i) + "].attenuation", lights.at(i)->GetLightAttenuation()); // attenuation
	}
}

// removes a light via its index
cherry::Light* cherry::LightList::RemoveLightByIndex(unsigned int index)
{
	if (index >= lights.size()) // bounds checking
		return nullptr;

	Light * ll = lights.at(index);

	if (util::removeFromVector(lights, ll)) // success
	{
		return ll;
	}
	else // failure
	{
		return nullptr;
	}
}

// removes a light via its pointer
cherry::Light* cherry::LightList::RemoveLightByPointer(cherry::Light* light)
{
	if (light == nullptr) // nullptr passed.
		return nullptr;

	if (util::removeFromVector(lights, light)) // removed successfully
	{
		return light;
	}
	else // failure
	{
		return nullptr;
	}
}

// deletes a light via its index.
bool cherry::LightList::DeleteLightByIndex(unsigned int index)
{
	Light* ll = RemoveLightByIndex(index);

	if (ll != nullptr) // light exists
	{
		delete ll;
		return true;
	}
	else // light doesn't exist
	{
		return false;
	}
}

// deletes the light via its pointer.
bool cherry::LightList::DeleteLightByPointer(cherry::Light* light)
{
	if (light == nullptr) // nullptr passed
		return false;

	if (RemoveLightByPointer(light) != nullptr) // if the light was removed successfully
	{
		delete light;
		return true;
	}
	else // not removed successfully
	{
		return false;
	}
}

// delets all the lights.
void cherry::LightList::DeleteAllLights()
{
	for (Light* light : lights)
		delete light;

	lights.clear();
}

// gets whether or not the background is being ignored
bool cherry::LightList::IsIgnoringBackground() const { return ignoreBackground; }

// sets whether or not to ignore the background for post processed ligthing.
void cherry::LightList::SetIgnoreBackground(bool ignore)
{
	ignoreBackground = ignore;
	shader->SetUniform("a_IgnoreBackground", (int)ignoreBackground);
	shadowShader->SetUniform("a_IgnoreBackground", (int)ignoreBackground);
}

// checks to see if shadows are enabled
bool cherry::LightList::GetShadowsEnabled() const { return shadows; }

// sets whether shadows are enabled or not.
void cherry::LightList::SetShadowsEnabled(bool enable)
{
	switch (enable)
	{
	case true:
		layer->AddLayer(shadowShader, shadowBuffer);
		break;

	case false:
		layer->RemoveLayer(shadowShader, shadowBuffer);
		break;
	}
	
	shadows = enable;
}

// updates the materials of the objects
void cherry::LightList::UpdateMaterials()
{
	ObjectList* objectList = ObjectManager::GetSceneObjectListByName(scene);
	Material::Sptr tempMat; // tempory material

	if (objectList != nullptr)
	{
		// updates the objects.
		for (Object* obj : objectList->GetObjects())
		{
			tempMat = obj->GetMaterial();
			ApplyLights(tempMat);
		}
	}
}

// updates post processing layer
void cherry::LightList::UpdatePostLayer()
{
	glm::vec3 temp; // temporary glm::vector
	int enabledLights = glm::clamp((int)lights.size(), 0, MAX_LIGHTS);

	// setting the light count.
	shader->SetUniform("a_EnabledLights", enabledLights);
	shadowShader->SetUniform("a_EnabledLights", enabledLights);

	shader->SetUniform("a_IgnoreBackground", (int)ignoreBackground);
	shadowShader->SetUniform("a_IgnoreBackground", (int)ignoreBackground);

	// goes through each light, getting the values.
	for (int i = 0; i < enabledLights; i++)
	{
		// blinn-phong shader
		temp = lights.at(i)->GetAmbientColorGLM();
		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].ambientColor").c_str(), { temp[0], temp[1], temp[2] }); // ambient colour

		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].ambientPower").c_str(), lights.at(i)->GetAmbientPower()); // ambient power
		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].specularPower").c_str(), lights.at(i)->GetLightSpecularPower()); // specular power

		temp = lights.at(i)->GetLightPositionGLM();
		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].position").c_str(), { temp[0], temp[1], temp[2] }); // position

		temp = lights.at(i)->GetLightColorGLM();
		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].color").c_str(), { temp[0], temp[1], temp[2] }); // light colour

		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].shininess").c_str(), lights.at(i)->GetLightShininess()); // shininess
		shader->SetUniform(("a_Lights[" + std::to_string(i) + "].attenuation").c_str(), lights.at(i)->GetLightAttenuation()); // attenuation
	
		// shadow shader
		temp = lights.at(i)->GetAmbientColorGLM();
		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].ambientColor").c_str(), { temp[0], temp[1], temp[2] }); // ambient colour

		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].ambientPower").c_str(), lights.at(i)->GetAmbientPower()); // ambient power
		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].specularPower").c_str(), lights.at(i)->GetLightSpecularPower()); // specular power

		temp = lights.at(i)->GetLightPositionGLM();
		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].position").c_str(), { temp[0], temp[1], temp[2] }); // position

		temp = lights.at(i)->GetLightColorGLM();
		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].color").c_str(), { temp[0], temp[1], temp[2] }); // light colour

		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].shininess").c_str(), lights.at(i)->GetLightShininess()); // shininess
		shadowShader->SetUniform(("a_Lights[" + std::to_string(i) + "].attenuation").c_str(), lights.at(i)->GetLightAttenuation()); // attenuation
	}
}

// updates the lights for the objects this list is attachted to.
void cherry::LightList::Update(float deltaTime)
{
	UpdateMaterials();
	UpdatePostLayer();

}

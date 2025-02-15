#pragma once
#include <string>
#include "cherry/objects/Object.h"

namespace cnz
{
	class Obstacle : public cherry::Object {
	public:
		//// constructors
		// default (dont use)
		Obstacle() = default;

		// load in obj file and specify whether to load its associated mtl file
		Obstacle(std::string filePath, std::string scene, bool loadMtl = false);

		// same as above with option to specifiy a physics body size. This does not create the physics body!
		Obstacle(std::string filePath, std::string scene, cherry::Vec3 pbodySize, bool loadMtl = false);

		// faux copy constructor
		Obstacle(Obstacle* obj, std::string sceneName);

		// copy constructor
		Obstacle(const Obstacle&);

		// Update (probably wont need unless we have animated/moving/rotating obstacles
		void Update(float deltaTime);

		//// angle stuff. Should maybe be used in level editor. TODO: get collision working for non axis alligned boudning boxes.
		// sets object angle in degrees or radians. bool is true if degrees, false, if radians
		void SetAngle(float angle, bool isDegrees);

		// sets object angle in vec3
		void SetAngle(glm::vec3 angle);

		// set draw pbody
		bool SetDrawPBody(bool draw);

		// get draw pbody
		bool GetDrawPBody() const;

		// get pbody size
		cherry::Vec3 GetPBodySize() const;

		// set pbody size - We use this when getting the size after loading the object based on its mesh mins and maxes.
		void SetPBodySize(cherry::Vec3 newSize);

		// get pbody width
		float GetPBodyWidth() const;

		// get pbody height
		float GetPBodyHeight() const;

		// get pbody depth
		float GetPBodyDepth() const;

	private:
		// pbody size
		cherry::Vec3 pBodySize = cherry::Vec3(4, 4, 4);

		// bool for if a pbody should be drawn
		bool drawPBody = false;

		// TODO: possibly delete these variables? Object has its own rotation now.
		// object angle in screen space (degrees or radians)
		float degreeAngle = 0, radianAngle = 0;

		// object angle in world space (vec3, so 3d angle)
		glm::vec3 worldAngle;

		// a vector of physics bodies
		// std::vector<cherry::PhysicsBody*> bodies;
	};
}
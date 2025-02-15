#pragma once

#include "..\cherry\objects/Object.h"
#include "..\AnimationLoader.h"


// class for the Enemy
namespace cnz
{
	// enum for enemies
	enum enemy_t
	{
		null = 0,
		marauder = 1,
		oracle = 2,
		sentry = 3,
		bastion = 4,
		mechaspider = 5
	};

	// Enemy class
	class Enemy : public cherry::Object
	{
	public:
		//Default Contructor
		Enemy();

		// Copy Constructor
		Enemy(const Enemy&);

		// copies an enemy pointer and places the in the provided scene.
		Enemy(const Enemy* obj, std::string scene);

		// creates an Enemy, and the file path of the user.
		Enemy(std::string modelFile);

		// sets the model for the Enemy, and the position.
		Enemy(std::string modelPath, float posX, float posY, float posZ);

		// creates an Enemy, and the file path of the user, which also saves the position of the user.
		Enemy(std::string modelPath, cherry::Vec3 pos);

		// creates the Enemy, taking in the scene and material as well.
		Enemy(std::string modelPath, std::string scene, cherry::Material::Sptr material);

		// creates the Enemy, taking in the scene, material, and position as well.
		Enemy(std::string modelPath, std::string scene, cherry::Material::Sptr material, cherry::Vec3 pos);

		// loads a material if 'true' is passed.
		Enemy(std::string modelPath, std::string scene, bool loadMtl);

		// can load an mtl file and its position.
		Enemy(std::string modelPath, std::string scene, bool loadMtl, cherry::Vec3 pos);

		// loads in the Enemy using a default model. The Enemy takes this model's information.
		// Enemy(const cherry::Primitive * model);

		// the enemy's attack
		virtual void Attack(cherry::Vec3 startPos, cherry::Vec3 aimPos);

		// the type of enemy
		// TODO: move this.
		virtual std::string WhoAmI() { return description; }

		// returns the type of enemy
		virtual cnz::enemy_t GetType() const;

		// Holdovers from the Object class.
		// gets object angle in screen space in degrees
		float GetDegreeAngle() const;

		// gets object angle in screen space in radians
		float GetRadianAngle() const;

		// gets object angle in world space in vector 3
		glm::vec3 GetVec3Angle() const;

		//Update Angle given enemy position and what they should look at
		void UpdateAngle(cherry::Vec3 one, cherry::Vec3 two);

		// sets object angle in degrees or radians. bool is true if degrees, false, if radians
		void SetAngle(float angle, const bool isDegrees);

		// sets object angle in vec3
		void SetAngle(glm::vec3 angle);
		
		// gets the walking animation
		cherry::Animation* GetWalkAnimation() const;

		// gets the index of the walking animation.
		int GetWalkAnimationIndex() const;

		// gets the attack animation
		cherry::Animation* GetAttackAnimation() const;

		// gets the index of the attack animation.
		int GetAttackAnimationIndex() const;

		// gets the primary physics body for the enemy
		// if no primary body is set, then the first body in the list is returned. If there are no bodies, it returns nullptr.
		cherry::PhysicsBody* GetPrimaryPhysicsBody() const;

		// set draw pbody
		bool SetDrawPBody(bool draw);

		// get draw pbody
		bool GetDrawPBody() const;

		// get pbody size
		cherry::Vec3 GetPBodySize() const;

		// gets the width of the pbody
		float GetPBodyWidth() const;

		// gets the height of the pbody
		float GetPBodyHeight() const;

		// gets the depth of the pbody
		float GetPBodyDepth() const;

		// getter for state
		int GetState() const;

		// setting the state
		void SetState(int newState);

		// returns 'true' if the enemy uses projectiles.
		bool IsUsingProjectiles() const;

		// returns 'true' if a projectile can be fired.
		bool GetProjectileAvailable() const;

		// returns the alloted charge time for the projectile.
		float GetCurrentProjectileCharge() const;

		// returns the maximum charge time for the projectile.
		float GetMaximumProjectileChargeTime() const;

		// returns the factor applied when charging up a projectile.
		float GetProjectileChargeFactor() const;

		// called when a projectile is fired.
		// this resets the clock regardless of if the projectile was fully charged or not.
		void ProjectileFired();

		// gets the amount of points for killing the enemy.
		int GetPoints() const;

		// gets the multiplier applied to the enemy's movement.
		float GetSpeedMultiplier() const;

		// sets the speed multiplier for the enemy. It cannot be below or equal to 0.
		// set to 10.0F by default.
		void SetSpeedMultiplier(float speed);

		// the enemy's update
		virtual void Update(float dt);

		// the enemy's description.
		std::string description = "Enemy";

		// variable for attacking
		bool attacking = false;

		// no longer used.
		cherry::Object* arrow = nullptr;

		// bool for alive
		bool alive = false;

		// bool for stunned
		bool stunned = false;

		// stun timer
		float stunTimer = 0.0F;

		// I don't know what this is.
		int state = 0;

		// difficulty. TODO: the spawning system needs a major overhaul, so this will be needed for that.
		// int difficulty = 0;

	private:

		// TODO: this probably shouldn't be the same for every enemy. But you can look at this later.
		//		   z?		something is really funky here...
		cherry::Vec3 pBodySize = cherry::Vec3(1, 2, 1);
		bool drawPBody = false;

		// TODO: possibly delete these variables? Object has its own rotation now.
		// object angle in screen space (degrees or radians)
		float degreeAngle = 0, radianAngle = 0;

		// object angle in world space (vec3, so 3d angle)
		glm::vec3 worldAngle;

		// counts up to charge a projectile.
		float projChargeTimer = 0.0F;

		// maximum charge time for a projectile.
		float projChargeTimeMax = 1.0F;
		
		// used for charging up to a projectile
		float projTimerFactor = 1.0F;

		// the speed multiplier of the enemy
		float speedMult = 10.0F;

		// the points recieved for killing the enemy
		int points = 1;

	protected:

		// cnz animation
		struct CNZ_Animation
		{
			cherry::Animation* animation = nullptr;
			int index = -1;
		};
		
		// sets the type of this enemy.
		void SetType(cnz::enemy_t et);

		// loads in all animations. This is a pure virtual function since all enemies need animations.
		virtual void LoadAnimations() = 0;

		// sets whether to use projectiles or not.
		void SetUsingProjectiles(bool use);

		// set the maximum projectile charge time
		void SetMaximumProjectileChargeTime(float maxTime);

		// sets the projectile charge factor. It cannot be negative, or zero.
		void SetProjectileChargeFactor(float factor);

		// sets the amount of points the enemy provides. This cannot be negative.
		void SetPoints(int pnts);

		// becomes 'true' if the animations have been loaded.
		// TODO: add this
		// bool animsLoaded = false;

		// the enemy type (as an enum)
		cnz::enemy_t type = null;

		// set to 'true' if projectiles are used.
		bool useProjs = false;

		// walk animation
		CNZ_Animation aniWalk;

		// attack animation
		CNZ_Animation aniAttack;

		// the primary physics body.
		cherry::PhysicsBody* primaryBody = nullptr;
	};
}
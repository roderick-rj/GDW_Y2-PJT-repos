// AnimationManager - will be used to manage animations
#include "AnimationManager.h"

// animation manager constructor
cherry::AnimationManager::AnimationManager()
{
}

// copy constructor
cherry::AnimationManager::AnimationManager(const AnimationManager& anim)
{
	DeleteAllAnimations();
	
	// copying the animations
	for (Animation* a : anim.animations)
	{
		switch (a->GetId()) // id
		{
		case 1: // morph targets
			animations.push_back(new MorphAnimation(*((MorphAnimation*)a)));
			break;
		case 3: // image animation
			animations.push_back(new ImageAnimation(*((ImageAnimation*)a)));
			break;
		}
	}
}

// destructor
cherry::AnimationManager::~AnimationManager()
{
	for (Animation* anime : animations)
		delete anime;

	animations.clear();
}

// sets the object for all animations in the list.
void cherry::AnimationManager::SetObject(Object* object)
{
	if (object == nullptr)
		return;

	for (Animation* ani : animations)
		ani->SetObject(object);
}

// adds an animation, and makes it the current one if a 'true' is passed
bool cherry::AnimationManager::AddAnimation(cherry::Animation * newAni, bool current)
{
	if (newAni == nullptr) // no animation passed
		return false;

	// if 'true' is returned, then it was added to the vector.
	if (util::addToVector(animations, newAni))
	{
		if (current && animations.size() > 1)
		{
			animations[currentAnimation]->Stop(); // stops the current animation
			currentAnimation = animations.size() - 1;
		}
	}
	else // failure to add
	{
		return false;
	}
}

// removes an animation
bool cherry::AnimationManager::RemoveAnimation(unsigned int index)
{
	if (index < animations.size()) // index is valid
	{
		// returns true if the removal was successful.
		// return util::removeFromVector(animations, animations.at(index));
		animations.erase(animations.begin() + index);
		return true;
	}
	else // index is invalid
	{
		return false;
	}
}

// clears out all animations
void cherry::AnimationManager::ClearAllAnimations() { animations.clear(); }

// deletes all animations
void cherry::AnimationManager::DeleteAllAnimations()
{
	for (Animation* ani : animations)
		delete ani;

	animations.clear();
}

// replaces all animations
void cherry::AnimationManager::ReplaceAnimations(const AnimationManager& manager)
{
	DeleteAllAnimations();

	// copying the animations
	for (Animation* a : manager.animations)
	{
		switch (a->GetId()) // id
		{
		case 1: // morph targets
			animations.push_back(new MorphAnimation(*((MorphAnimation*)a)));
			break;
		case 3: // image animation
			animations.push_back(new ImageAnimation(*((ImageAnimation*)a)));
			break;
		}
	}
}

// gets the total amount of animations
unsigned int cherry::AnimationManager::GetAnimationsTotal() const { return animations.size(); }

// gets the animation at the requested index.
cherry::Animation * cherry::AnimationManager::GetAnimation(unsigned int index)
{
	if (index >= animations.size()) // out of bounds
	{
		return nullptr;
	}
	else // in bounds
	{
		return animations.at(index);
	}
}

// gets the current animation
cherry::Animation * cherry::AnimationManager::GetCurrentAnimation()
{
	if (currentAnimation < animations.size() && currentAnimation >= 0)
	{
		return animations[currentAnimation];
	}
	else
	{
		return nullptr;
	}
	
}

// sets the current animation based on the provided animation.
void cherry::AnimationManager::SetCurrentAnimation(cherry::Animation* newAni, const bool addAnimation)
{
	// TODO: check for proper animation.

	// animation provided was null.
	if (newAni == nullptr)
		return;

	// checks for the animation
	for (int i = 0; i < animations.size(); i++)
	{
		// animation has been found.
		if (animations[i] == newAni)
		{
			currentAnimation = i; // saves the index.
			return; // function finished.
		}
	}

	// if the for loop could not find the animation, then it's not in the list.
	// if that's the case, and addAnimation is true, then the animation is added to the list.
	if (addAnimation)
	{
		// saves the current animation.
		animations.push_back(newAni);
		currentAnimation = animations.size() - 1;
	}

}

// sets the current animation
void cherry::AnimationManager::SetCurrentAnimation(unsigned int index)
{
	if (index < animations.size())
	{
		currentAnimation = (signed)(index);
	}
}

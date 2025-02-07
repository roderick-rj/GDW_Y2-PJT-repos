// Morph Animation (Header) -used for morph targets
#pragma once
#include "Animation.h"
#include "..\Mesh.h"

namespace cherry
{
	// morph animation frame (forward declare)
	class MorphAnimationFrame;

	typedef class MorphAnimation : public Animation
	{
	private:
		struct Pose;

	public:
		MorphAnimation();

		// copy constructor
		MorphAnimation(const MorphAnimation&);

		// destructor
		virtual ~MorphAnimation();

		// the object being used for the morph target animation.
		MorphAnimation(Object* obj);
		
		// creates the object and changes its mesh type.
		void SetObject(cherry::Object* obj);

		// adds a frame and generates a pose.
		bool AddFrame(AnimationFrame* frame) override;

		// generates a mesh for the current pose, with position1 and normal1 being the targets.
		MorphVertex* GeneratePose() const;

		// updates the morphing animation
		void Update(float deltaTime);

		// toString
		std::string ToString() const override;

	private:
		// generates a pose based on the provided frames.
		Pose GeneratePose(MorphAnimationFrame* f0, MorphAnimationFrame* f1);

		// gets a pose from the list. One is generated if it cannot be found.
		Pose& GetPose(MorphAnimationFrame* f0, MorphAnimationFrame* f1);

		// gets the current pose
		Pose GetCurrentPose();
		
		// the value of 't'
		float t = 0;

		// the current frame
		int currFrameIndex = -1;

		// pose struct
		struct Pose
		{
			MorphVertex* pose;
			MorphAnimationFrame* f0;
			MorphAnimationFrame* f1;
		};

		// poses
		std::vector<Pose> poses;

	protected:

	};

	// class for morph target animation
	typedef class MorphAnimationFrame : public AnimationFrame
	{
	public:

		// creates a morph animation frame with the vertices and normals seperated.
		// this takes position 0.
		MorphAnimationFrame(glm::vec3 * vertices, glm::vec4 * colors, glm::vec3 * normals, glm::vec2 * uvs, unsigned int valNum, float units = 1.0F);

		// receives the verts to lerp to for this given frame, with units determining how long the lerp process takes from the last frame.
		MorphAnimationFrame(cherry::Vertex* newPose, unsigned int vertsTotal, float units = 1.0F);

		// receives the verts to lerp to for this given frame
		// units determines the length of the lerp between the previous frame; default 1 second.
		// this takes position1 and normal1 from the provided pose values
		MorphAnimationFrame(cherry::MorphVertex * newPose, unsigned int vertsTotal, float units = 1.0F);

		// the name of the file for the morph target animation frame; only takes .obj files for now.
		// units determines the length of the lerp between the previous frame; default 1 second.
		MorphAnimationFrame(std::string filePath, float units = 1.0F);

		// copy constructor
		MorphAnimationFrame(const MorphAnimationFrame&);

		// destructor
		~MorphAnimationFrame();
		
		// returns the pose
		const cherry::Vertex* const GetPose() const;

		// gets the amount of vertices
		unsigned int GetVertexCount() const;

		// gets the current time value for the morph target. The interpolation of the vertices happens in the shader.
		// this gets the current value of 't' (or 'u')
		// float GetTime() const;


		// updates the morph target
		// void Update(float deltaTime);

		// toString
		std::string ToString() const override;

	protected:
	
	private:

		// loads in the vertices from a file
		void LoadPose();

		// the pose for this frame
		Vertex* pose = nullptr; // vertices, colors, normals, and uvs

		// total amount of vertices
		unsigned int verticesTotal;

		// saves the file path once it is deteremined to be accessible.
		std::string filePath = "";
		
		// the value of 't'
		// float t = 0;

		// TODO: have LERP, SLERP, and other lerp types
		// int lerpType;


	} MorphFrame;
}
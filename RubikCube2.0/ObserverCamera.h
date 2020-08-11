#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ObserverCamera
{
public:
	ObserverCamera(const glm::vec3 & postion, const glm::vec3 & target);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);

	glm::mat4 GetViewMatrix() const { return mViewMatrix; }
	glm::vec3 GetPosition() const{ return mCurrentPostion; }
	float GetZoom() const { return mZoom; }
private:
	void UpdateCamera();

private:
   float mZoom;
   float mYaw;
   float mPitch;
   glm::vec3 mTarget;
   glm::vec3 mInitPostion;
   glm::vec3 mCurrentPostion;
   glm::mat4 mViewMatrix;
};
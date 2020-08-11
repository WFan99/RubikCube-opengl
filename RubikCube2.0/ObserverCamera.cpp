#include "ObserverCamera.h"

namespace {
	constexpr float ZOOM = 45.0f;
	constexpr float MouseSensitivity = 0.25;
	constexpr float MaxPitch = 40;
	constexpr float MinPitch = -40;
	constexpr float MaxZoom = 45;
	constexpr float MinZoom = 1;
}

ObserverCamera::ObserverCamera(const glm::vec3 & postion, const glm::vec3 & target) :
	mZoom(ZOOM),
	mYaw(0),
	mPitch(0),
	mTarget(target),
	mInitPostion(postion),
	mCurrentPostion(postion)
{
	UpdateCamera();
}

void ObserverCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
	mPitch += yoffset * MouseSensitivity;
	mYaw -= xoffset * MouseSensitivity;
	if (mPitch > MaxPitch)
		mPitch = MaxPitch;
	else if (mPitch < MinPitch)
		mPitch = MinPitch;
	UpdateCamera();
}

void ObserverCamera::UpdateCamera()
{
	glm::mat4 rotate(1.0f);
	glm::vec3 pos(mInitPostion);
	pos = glm::mat3(glm::rotate(rotate, glm::radians(mPitch), glm::vec3(1, 0, 0))) * pos;
	pos = glm::mat3(glm::rotate(rotate, glm::radians(mYaw), glm::vec3(0, 1, 0))) * pos;
	mCurrentPostion = pos;

	glm::vec3 front = glm::normalize(mTarget - pos);
	glm::vec3 right = glm::cross(front, glm::vec3(0,1,0));
	glm::vec3 up = glm::cross(right, front);
	mViewMatrix = glm::lookAt(pos, pos+front, up);

}

void ObserverCamera::ProcessMouseScroll(float yoffset)
{
	mZoom -= (float)yoffset;
	if (mZoom < MinZoom)
		mZoom = MinZoom;
	if (mZoom > MaxZoom)
		mZoom = MaxZoom;
}
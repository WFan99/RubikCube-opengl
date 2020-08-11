#pragma once
#include "Object3D.h"
#include <vector>
#include <memory>

class Plane :public Object3D
{
public:
	Plane(std::shared_ptr<Shader> shader, unsigned int attribute);
	~Plane();
	void Draw();
	void DrawShadowMap();
	void SetTex(unsigned int slot, unsigned int tex);
	void SetShadowShader(std::shared_ptr<Shader> shadowShader) { mShadowShader = shadowShader; }
private:
	struct PlaneTex
	{
		unsigned int slot;
		unsigned int texture;
	};

private:
	std::vector<PlaneTex> mTexVec;
	std::shared_ptr<Shader> mShadowShader;
	unsigned int mVBO;
	unsigned int mVAO;
	unsigned int mEBO;
};
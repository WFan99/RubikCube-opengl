#pragma once
#include "Object3D.h"
#include <vector>
class Cube :public Object3D
{
public:
	enum Face{ Back,Front,Left,Right,Bottom,Top};

	Cube(std::shared_ptr<Shader> shader,unsigned int attribute);
	~Cube();
	void Draw();
	void DrawShadowMap();
	void SetShadowMapShader(std::shared_ptr<Shader> shadowMapShader) { mShadowMapShader = shadowMapShader; }
	void SetTex(unsigned int slot, unsigned int tex);
	void SetTex(unsigned int slot, Face face, unsigned int tex);
	void SetTex(unsigned int slot, const std::vector<unsigned int>& texVec);
private:
	struct CubeTex
	{
		unsigned int slot;
		std::vector<unsigned int> tex;
	};

private:
	std::vector<CubeTex> mTexVec;
	std::shared_ptr<Shader> mShadowMapShader;
	unsigned int mVBO;
	unsigned int mVAO;
	unsigned int mEBO;
};
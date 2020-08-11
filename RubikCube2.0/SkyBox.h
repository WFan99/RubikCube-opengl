#pragma once
#include "Object3D.h"
#include <vector>
class SkyBox :public Object3D
{
public:
	SkyBox(std::shared_ptr<Shader> shader);
	~SkyBox();
	void Draw();
	void SetCubmapTex(unsigned int cubemapTex) { mCubmapTexture = cubemapTex; }
	static unsigned int LoadCubemapTexture(const char** path, bool flip);

private:
	unsigned int mCubmapTexture;
	unsigned int mVBO;
	unsigned int mVAO;
	unsigned int mEBO;
	
};
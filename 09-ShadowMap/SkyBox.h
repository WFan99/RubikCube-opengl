#pragma once
#include "Object3D.h"
#include <vector>
class SkyBox :public Object3D
{
public:
	SkyBox(std::shared_ptr<Shader> shader);
	~SkyBox();
	void Draw();
	void SetTex(const std::vector<unsigned int>& tex) { mTex = tex; };

private:
	std::vector<unsigned int> mTex;
	unsigned int mVBO;
	unsigned int mVAO;
	unsigned int mEBO;
	
};
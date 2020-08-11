#pragma once
#include "Cube.h"
#include <map>
class RubikCube
{
public:
	RubikCube(std::shared_ptr<Shader> shader);
	void Draw();
	void DrawShadowMap();
private:
	enum RubikFace { Back, Front, Left, Right, Bottom, Top };
	enum Color {Green, Blue, White, Yellow, Red, Orange, None};

private:
	Cube mCube;
	std::vector<unsigned int> mCubeTex;
	std::shared_ptr<Shader> mShader;
	std::map<RubikFace, std::vector<Color>> mColors;
	
};
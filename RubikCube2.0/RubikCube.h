#pragma once
#include "Cube.h"
#include <map>
class RubikCube
{
public:
	RubikCube(std::shared_ptr<Shader> shader);
	void Draw();
	void DrawShadowMap();
	void DrawSelectionMap();
	void SetShadowMapShader(std::shared_ptr<Shader> shadowMapShader) { 
		mShadowMapShader = shadowMapShader; 
		mCube.SetShadowMapShader(shadowMapShader);
	}
	void SetSelectionShader(std::shared_ptr<Shader> selectionShader) { 
		mSelectionShader = selectionShader;
		mCube.SetSelectionShader(selectionShader);}
	void ProcessClickEvent(double x, double y, bool leftBtn);
private:
	enum RubikFace { Back, Front, Left, Right, Bottom, Top };
	enum Color {Green, Blue, White, Yellow, Red, Orange, None};

private:
	Cube mCube;
	std::vector<unsigned int> mCubeTex;
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Shader> mShadowMapShader;
	std::shared_ptr<Shader> mSelectionShader;
	std::map<RubikFace, std::vector<Color>> mColors;
	
};
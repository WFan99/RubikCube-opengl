#pragma once
#include "Cube.h"
#include <map>
class RubikCube
{
public:
	enum Move {MoveUp, MoveDown, MoveLeft, MoveRight};

public:
	RubikCube(std::shared_ptr<Shader> cubeShader, std::shared_ptr<Shader> edgeShader);
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
	void ProcessKeyboardEvent(Move event);

private:
	enum RubikFace { Back, Front, Left, Right, Bottom, Top };
	enum Color {Green, Blue, White, Yellow, Red, Orange, NoneColor};
	enum Layer { NoneLayer, HorizonTop, HorizonMid, HorizonDown, VerticalLeft, VerticalMid, VerticalRight };
private:
	bool JudgeClicked(int x, int y, int z);
	bool NoneVeriticalRotate();
	bool NoneHorizontalRotate();
	void RotateVerticalTexDown();
	void RotateHorizontalTexRight();
	void RotateVerticalTexUp();
	void RotateHorizontalTexLeft();

private:
	Cube mCube;
	Cube mEdge;
	Layer mClickedLayer;
	unsigned int mEdgeTex;
	bool mRotateVertical;
	std::vector<unsigned int> mCubeTex;
	std::shared_ptr<Shader> mShader;
	std::shared_ptr<Shader> mEdgeShader;
	std::shared_ptr<Shader> mShadowMapShader;
	std::shared_ptr<Shader> mSelectionShader;
	std::map<RubikFace, std::vector<Color>> mColors;
	std::map<Layer, float> mRotateAngle;
	glm::vec3 xAxis;
	glm::vec3 yAxis;
	
};
#include "RubikCube.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;
namespace {
	const string texPathPre = "res/textures/rubikCube/";
	const string texName[] = { "green.png","blue.png","white.png",
							  "yellow.png","red.png","orange.png" };
	const string edgeName = "cube_edge.png";

	constexpr float CUBE_SIZE = 0.3f;
	constexpr float EDGE_SCALE = 1.15f;
	constexpr float ZERO_ANGLE = 6.8f;

	bool FloatZero(float f) { return abs(f) < ZERO_ANGLE; }
	
}


RubikCube::RubikCube(std::shared_ptr<Shader> cubeShader, std::shared_ptr<Shader> edgeShader) :
	mCube(cubeShader, Object3D::Attribute::Position | Object3D::Attribute::Normal | Object3D::Attribute::Texture),
	mEdge(edgeShader, Object3D::Attribute::Position | Object3D::Attribute::Texture),
	mRotateVertical(true),
	mCubeTex(6),
	mShader(cubeShader),
	mEdgeShader(edgeShader),
	xAxis(1,0,0),
	yAxis(0,1,0)
{
	mColors[Back] = vector<Color>(9, Green);
	mColors[Front] = vector<Color>(9, Blue);
	mColors[Left] = vector<Color>(9, White);
	mColors[Right] = vector<Color>(9, Yellow);
	mColors[Bottom] = vector<Color>(9, Red);
	mColors[Top] = vector<Color>(9, Orange);

	//HorizonTop, HorizonMid, HorizonDown, VerticalLeft, VerticalMid, VerticalRight
	mRotateAngle[HorizonTop] = mRotateAngle[HorizonMid] = mRotateAngle[HorizonDown] =
		mRotateAngle[VerticalLeft] = mRotateAngle[VerticalMid] = mRotateAngle[VerticalRight] = 0;

	
	for (int i = 0; i < 6; i++)
	{
		string path = texPathPre + texName[i];
		mCubeTex[i] = Object3D::LoadTexture(path.c_str(),true);
	}
	string edgePath = texPathPre + edgeName;
	mEdgeTex = Object3D::LoadTexture(edgePath.c_str(), true);
	mEdge.SetTex(GL_TEXTURE0, mEdgeTex);
}

bool RubikCube::JudgeClicked(int x, int y, int z)
{
	bool clicked = false;
	//点击事件
	switch (mClickedLayer)
	{
	case Layer::HorizonTop:
		clicked = y == 1;
		break;
	case Layer::HorizonMid:
		clicked = y == 0;
		break;
	case Layer::HorizonDown:
		clicked = y == -1;
		break;
	case Layer::VerticalLeft:
		clicked = x == -1;
		break;
	case Layer::VerticalMid:
		clicked = x == 0;
		break;
	case Layer::VerticalRight:
		clicked = x == 1;
		break;
	default:
		clicked = false;
		break;
	}
	return clicked;
}


void RubikCube::Draw()
{
	
	for (int y = 1; y >= -1; y--)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int x = -1; x <= 1; x++)
			{
				bool clicked = JudgeClicked(x, y, z);
				mShader->use();
				glm::mat4 model(1.0f);

				glm::mat4 cubeModel = glm::scale(model, glm::vec3(CUBE_SIZE));
				if (mRotateVertical)
				{
					if (x == -1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalLeft]), xAxis);
					else if(x == 0)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalMid]), xAxis);
					else if( x == 1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalRight]), xAxis);
				}
				else
				{
					if (y == -1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonDown]), yAxis);
					else if(y == 0)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonMid]), yAxis);
					else if(y == 1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonTop]), yAxis);
				}
				cubeModel = glm::translate(cubeModel, glm::vec3(x, y, z));		
				mShader->setMat4("model", cubeModel);
				//纹理颜色
				vector<Color> colors(6, NoneColor);
				if (x == -1)
					colors[Left] = mColors[Left][(z+1)*3 + (y+1)];
				else if (x == 1)
					colors[Right] = mColors[Right][-(z-1)*3 + (y+1)];

				if (y == -1)
					colors[Bottom] = mColors[Bottom][(x + 1)*3 +(z + 1)];
				else if (y == 1)
					colors[Top] = mColors[Top][(x + 1)*3 -(z - 1)];
				if (z == -1)
					colors[Back] = mColors[Back][(x + 1)*3 - (y - 1)];
				else if (z == 1)
					colors[Front] = mColors[Front][(x + 1)*3 + (y + 1)];

				vector<unsigned int> texes(6,0);
				for (int i= 0; i < 6; i++)
				{
					if (colors[i] != NoneColor)
						texes[i] = mCubeTex[colors[i]];
				}
				mCube.SetTex(GL_TEXTURE0, move(texes));
				;
				if (clicked)
				{
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
					glStencilFunc(GL_ALWAYS,1, 0xff);
					glStencilMask(0xff);
					mCube.Draw();
					glm::mat4 edgeModel = glm::scale(cubeModel, glm::vec3(EDGE_SCALE));
					mEdgeShader->use();
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilMask(0x00);
					mEdgeShader->setMat4("model", edgeModel);
					mEdge.Draw();
				}
				else
				{
					glStencilMask(0xFF);
					glStencilFunc(GL_ALWAYS, 0, 0xff);
					mCube.Draw();
				}				
			}
		}
	}
}


void RubikCube::DrawShadowMap()
{
	mShadowMapShader->use();

	for (int y = 1; y >= -1; y--)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int x = -1; x <= 1; x++)
			{
				bool clicked = JudgeClicked(x, y, z);
				glm::mat4 model(1.0f);

				glm::mat4 cubeModel = glm::scale(model, glm::vec3(CUBE_SIZE));
				if (mRotateVertical)
				{
					if (x == -1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalLeft]), xAxis);
					else if (x == 0)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalMid]), xAxis);
					else if (x == 1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[VerticalRight]), xAxis);
				}
				else
				{
					if (y == -1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonDown]), yAxis);
					else if (y == 0)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonMid]), yAxis);
					else if (y == 1)
						cubeModel = glm::rotate(cubeModel, glm::radians(mRotateAngle[HorizonTop]), yAxis);
				}
				cubeModel = glm::translate(cubeModel, glm::vec3(x, y, z));
				mShadowMapShader->setMat4("model", cubeModel);
				mCube.DrawShadowMap();
			}
		}
	}

}

void RubikCube::DrawSelectionMap()
{
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mSelectionShader->use();
	int id = 0;
	for (int y = 1; y >= -1; y--)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int x = -1; x <= 1; x++)
			{
				glm::mat4 model(1.0f);
				//BACK,FRONT,LEFT,RIGHT,UP,DOWN
				model = glm::scale(model, glm::vec3(CUBE_SIZE));
				model = glm::translate(model, glm::vec3(x, y, z));
				mSelectionShader->setMat4("model", model);
				mSelectionShader->setInt("code", ++id);
				mCube.DrawSelectionMap();
			}
		}
	}

}

bool RubikCube::NoneVeriticalRotate()
{
	return FloatZero(mRotateAngle[VerticalLeft]) &&
		FloatZero(mRotateAngle[VerticalMid]) &&
		FloatZero(mRotateAngle[VerticalRight]);
}

bool RubikCube::NoneHorizontalRotate()
{
	return FloatZero(mRotateAngle[HorizonMid]) &&
		FloatZero(mRotateAngle[HorizonTop]) &&
		FloatZero(mRotateAngle[HorizonDown]);
}


void RubikCube::ProcessClickEvent(double x, double y, bool leftBtn)
{
	DrawSelectionMap();
	unsigned char res[4];
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
	if (res[0] == 0)
	{
		mClickedLayer = Layer::NoneLayer;
		return;
	}
	if (leftBtn && NoneVeriticalRotate())
	{
		mRotateVertical = false;
		switch ((res[0] - 1) / 9)
		{
		case 0:
			mClickedLayer = Layer::HorizonTop;
			break;
		case 1:
			mClickedLayer = Layer::HorizonMid;
			break;
		case 2:
			mClickedLayer = Layer::HorizonDown;
			break;
		default:
			break;
		}
	}
	else if(!leftBtn && NoneHorizontalRotate())
	{
		mRotateVertical = true;
		switch (((res[0] - 1) % 9) % 3)
		{
		case 0:
			mClickedLayer = Layer::VerticalLeft;
			break;
		case 1:
			mClickedLayer = Layer::VerticalMid;
			break;
		case 2:
			mClickedLayer = Layer::VerticalRight;
			break;
		default:
			break;

		}
	}

}
void RubikCube::RotateVerticalTexDown()
{
	int index;
	if (mClickedLayer == VerticalLeft)
	{
		auto colorLeft = mColors[Left];
		int changed[] = {6,3,0,7,4,1,8,5,2};
		for (int i = 0; i < 9; i++)
			mColors[Left][i] = colorLeft[changed[i]];
		index = 0;
	}
	else if (mClickedLayer == VerticalMid)
		index = 3;
	else if (mClickedLayer == VerticalRight)
	{
		auto colorRight = mColors[Right];
		int changed[]  = {2,5,8,1,4,7,0,3,6};
		for (int i = 0; i < 9; i++)
			mColors[Right][i] = colorRight[changed[i]];
		index = 6;
	}

	auto tempFront = mColors[Front];
	for (int i = index; i < 3 + index; i++)
	{
		mColors[Front][i] = mColors[Top][i];
		mColors[Top][i] = mColors[Back][i];
		mColors[Back][i] = mColors[Bottom][i];
		mColors[Bottom][i] = tempFront[i];

	}
}
void RubikCube::RotateHorizontalTexRight()
{
	int index;
	if (mClickedLayer == HorizonDown)
	{
		auto colorDown = mColors[Bottom];
		int changed[] = { 6,3,0,7,4,1,8,5,2 };
		for (int i = 0; i < 9; i++)
			mColors[Bottom][i] = colorDown[changed[i]];

		index = 0;
	}
	else if (mClickedLayer == HorizonMid)
		index = 1;
	else if (mClickedLayer == HorizonTop)
	{
		auto colorTop = mColors[Top];
		int changed[] = { 2,5,8,1,4,7,0,3,6 };
		for (int i = 0; i < 9; i++)
			mColors[Top][i] = colorTop[changed[i]]; 
		index = 2;
	}

	auto tempFront = mColors[Front];
	for (int i = index; i < 7 + index; i+=3)
	{
		mColors[Front][i] = mColors[Left][i];
		mColors[Left][i] = mColors[Back][-i+8];
		mColors[Back][-i +8] = mColors[Right][i];
		mColors[Right][i] = tempFront[i];
	}
}
void RubikCube::RotateVerticalTexUp()
{
	int index;
	if (mClickedLayer == VerticalLeft)
	{
		auto colorLeft = mColors[Left];
		int changed[] = { 2,5,8,1,4,7,0,3,6 };
		for (int i = 0; i < 9; i++)
			mColors[Left][i] = colorLeft[changed[i]];
		index = 0;
	}
	else if (mClickedLayer == VerticalMid)
		index = 3;
	else if (mClickedLayer == VerticalRight)
	{
		auto colorRight = mColors[Right];
		int changed[] = { 6,3,0,7,4,1,8,5,2 };
		for (int i = 0; i < 9; i++)
			mColors[Right][i] = colorRight[changed[i]];
		index = 6;
	}
	
	auto tempFront = mColors[Front];
	for (int i = index; i < 3+index; i++)
	{
		mColors[Front][i] = mColors[Bottom][i];
		mColors[Bottom][i] = mColors[Back][i];
		mColors[Back][i] = mColors[Top][i];
		mColors[Top][i] = tempFront[i];
		
	}
}
void RubikCube::RotateHorizontalTexLeft()
{
	int index;
	if (mClickedLayer == VerticalLeft)
		index = 0;
	else if (mClickedLayer == VerticalMid)
		index = 3;
	else if (mClickedLayer == VerticalRight)
		index = 6;

	auto tempFront = mColors[Front];
}

void RubikCube::ProcessKeyboardEvent(Move event)
{
	switch (event)
	{
	case MoveDown:
		if (mClickedLayer == HorizonTop || mClickedLayer == HorizonMid || mClickedLayer == HorizonDown)
			break;
		mRotateAngle[mClickedLayer] += 0.1;
		if (mRotateAngle[mClickedLayer] >= 90.0f)
		{
			mRotateAngle[mClickedLayer] = 0;
			RotateVerticalTexDown();
		}
		break;
	case MoveRight:
		if (mClickedLayer == VerticalLeft || mClickedLayer == VerticalMid || mClickedLayer == VerticalRight)
			break;
		mRotateAngle[mClickedLayer] += 0.1;
		if (mRotateAngle[mClickedLayer]>=90.0f)
		{
			mRotateAngle[mClickedLayer] = 0;
			RotateHorizontalTexRight();
		}
		break;
	case MoveUp:
		if (mClickedLayer == HorizonTop || mClickedLayer == HorizonMid || mClickedLayer == HorizonDown)
			break;
		mRotateAngle[mClickedLayer] -= 0.1;
		if (mRotateAngle[mClickedLayer] <= -90.0f)
		{
			mRotateAngle[mClickedLayer] = 0;
			RotateVerticalTexUp();
		}
		break;
	case MoveLeft:
		if (mClickedLayer == VerticalLeft || mClickedLayer == VerticalMid || mClickedLayer == VerticalRight)
			break;
		mRotateAngle[mClickedLayer] -= 0.1;
		if (mRotateAngle[mClickedLayer] <= -90.0f)
		{
			mRotateAngle[mClickedLayer] = 0;
			RotateHorizontalTexLeft();
		}
		break;
	default:
		break;
	}
}
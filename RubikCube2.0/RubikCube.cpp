#include "RubikCube.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
namespace {
	const string texPathPre = "res/textures/rubikCube/";
	const string texName[] = { "green.png","blue.png","white.png",
							  "yellow.png","red.png","orange.png" };

	const glm::vec3 TRANSLATE = glm::vec3(0.0f, 1.0f, 0.0f);
	constexpr float CUBE_SIZE = 0.3f;
	
}


RubikCube::RubikCube(std::shared_ptr<Shader> shader) :
	mCube(shader, Object3D::Attribute::Position | Object3D::Attribute::Normal | Object3D::Attribute::Texture),
	mCubeTex(6),
	mShader(shader)
{
	mColors[Back] = vector<Color>(9, Green);
	mColors[Front] = vector<Color>(9, Blue);
	mColors[Left] = vector<Color>(9, White);
	mColors[Right] = vector<Color>(9, Yellow);
	mColors[Bottom] = vector<Color>(9, Red);
	mColors[Top] = vector<Color>(9, Orange);

	
	for (int i = 0; i < 6; i++)
	{
		string path = texPathPre + texName[i];
		mCubeTex[i] = Object3D::LoadTexture(path.c_str());
	}
}

void RubikCube::Draw()
{

	for (int y = 1; y >= -1; y--)
	{
		for (int z = -1; z <= 1; z++)
		{
			for (int x = -1; x <= 1; x++)
			{
				glm::mat4 model(1.0f);
				model = glm::scale(model, glm::vec3(CUBE_SIZE));
				model = glm::translate(model, glm::vec3(x, y, z)+TRANSLATE);
				mShader->use();
				mShader->setMat4("model", model);
				vector<Color> colors(6, None);
				if (x == -1)
					colors[Left] = mColors[Left][y + z + 2];
				else if (x == 1)
					colors[Right] = mColors[Right][y + z + 2];

				if (y == -1)
					colors[Bottom] = mColors[Bottom][x + z + 2];
				else if (y == 1)
					colors[Top] = mColors[Top][x + z + 2];
				if (z == -1)
					colors[Back] = mColors[Back][x + y + 2];
				else if (z == 1)
					colors[Front] = mColors[Front][x + y + 2];

				vector<unsigned int> texes(6,0);
				for (int i= 0; i < 6; i++)
				{
					if (colors[i] != None)
						texes[i] = mCubeTex[colors[i]];
				}
				mCube.SetTex(GL_TEXTURE0, move(texes));
				mCube.Draw();
			}
		}
	}
}
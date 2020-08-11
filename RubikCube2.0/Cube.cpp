#include "Cube.h"
#include <vector>
#include <iostream>
using namespace std;

Cube::Cube(std::shared_ptr<Shader> shader, unsigned int attribute) :Object3D(shader)
{
	unsigned int slot = 0;
	unsigned int arrayBufferSize = 0;

	vector<float> vertices = {
		//Back
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		//Front
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		//Left
		-0.5f,  0.5f,  -0.5f,
		-0.5f, -0.5f,  -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 //Right
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 //Bottom
		 -0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, 0.5f,
		 //Top	
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,

	};

	vector<unsigned int> indices = {
		0, 1, 2, 2, 3 ,0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	vector<float> normal = {
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,

		0.0f,0.0f, 1.0f,
		0.0f,0.0f, 1.0f,
		0.0f,0.0f, 1.0f,
		0.0f,0.0f, 1.0f,

		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,

		 1.0f,0.0f,0.0f,
		 1.0f,0.0f,0.0f,
		 1.0f,0.0f,0.0f,
		 1.0f,0.0f,0.0f,

		 0.0f,-1.0f,0.0f,
		 0.0f,-1.0f,0.0f,
		 0.0f,-1.0f,0.0f,
		 0.0f,-1.0f,0.0f,

		 0.0f,1.0f,0.0f,
		 0.0f,1.0f,0.0f,
		 0.0f,1.0f,0.0f,
		 0.0f,1.0f,0.0f,
	};

	vector<float> texcoords = {
		//Back
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		//Front
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		
		//Left
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		//Right
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		//Bottom
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		//Top
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	unsigned int bufferSize = 0;
	if (attribute & Attribute::Position)
		bufferSize += vertices.size()*sizeof(float);
	if (attribute & Attribute::Normal)
		bufferSize += normal.size() * sizeof(float);
	if (attribute & Attribute::Texture)
		bufferSize += texcoords.size() * sizeof(float);

	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

	unsigned int startPos = 0;
	unsigned int slotPos = 0;
	if (attribute & Attribute::Position)
	{
		glBufferSubData(GL_ARRAY_BUFFER, startPos, sizeof(float)*(vertices.size()), vertices.data());
		glVertexAttribPointer(slotPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(startPos));
		glEnableVertexAttribArray(slotPos);
		startPos += sizeof(float)*(vertices.size());
		slotPos++;
	}
	if (attribute & Attribute::Normal)
	{
		glBufferSubData(GL_ARRAY_BUFFER, startPos, sizeof(float)*(normal.size()), normal.data());
		glVertexAttribPointer(slotPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(startPos));
		glEnableVertexAttribArray(slotPos);
		startPos += sizeof(float)*(normal.size());
		slotPos++;
	}

	if (attribute & Attribute::Texture)
	{
		glBufferSubData(GL_ARRAY_BUFFER,startPos,sizeof(float)*(texcoords.size()), texcoords.data());
		glVertexAttribPointer(slotPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(startPos));
		glEnableVertexAttribArray(slotPos);
		startPos += sizeof(float)*(texcoords.size());
		slotPos++;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), indices.data(), GL_STATIC_DRAW);

}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void Cube::SetTex(unsigned int slot, unsigned int tex)
{
	CubeTex cubeTex;
	cubeTex.slot = slot;
	cubeTex.tex = vector<unsigned int>(6, tex);
	for (int i = 0; i < mTexVec.size(); i++)
	{
		if (mTexVec[i].slot == slot)
		{
			mTexVec[i] = cubeTex;
			return;
		}
	}
	mTexVec.push_back(cubeTex);
}

void Cube::SetTex(unsigned int slot, Face face, unsigned int tex)
{
	for (int i = 0; i < mTexVec.size(); i++)
	{
		if (mTexVec[i].slot == slot)
		{
			mTexVec[i].tex[face] = tex;
			return;
		}
	}
	CubeTex cubeTex;
	cubeTex.slot = slot;
	cubeTex.tex = vector<unsigned int>(6, 0);
	cubeTex.tex[face] = tex;
	mTexVec.push_back(cubeTex);
}
void Cube::SetTex(unsigned int slot, const std::vector<unsigned int>& texVec)
{
	for (int i = 0; i < mTexVec.size(); i++)
	{
		if (mTexVec[i].slot == slot)
		{
			mTexVec[i].tex = texVec;
			return;
		}
	}
	CubeTex cubeTex;
	cubeTex.slot = slot;
	cubeTex.tex = texVec;
	mTexVec.push_back(cubeTex);
}


void Cube::Draw()
{
	mShader->use();
	glBindVertexArray(mVAO);

	float * ptr = 0;
	for (int i = 0; i < 6; i++)
	{
		for (CubeTex& cubeTex : mTexVec)
		{
			glActiveTexture(cubeTex.slot);
			glBindTexture(GL_TEXTURE_2D, cubeTex.tex[i]);
		}
		glDrawRangeElements(GL_TRIANGLES, i * 6, i * 6 + 5, 6, GL_UNSIGNED_INT, ptr + i * 6);
	}
}
void Cube::DrawShadowMap()
{
	mShadowMapShader->use();
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void Cube::DrawSelectionMap()
{
	mSelectionShader->use();
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
#include "Plane.h"
#include <vector>
using namespace std;

Plane::Plane(std::shared_ptr<Shader> shader, unsigned int attribute) :Object3D(shader)
{
	vector<float> vertices = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f,  0.0f,
		1.0f, 1.0f,  0.0f,
	};

	vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};

	vector<float> normal = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
	};

	vector<float> texcoords = {
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
		bufferSize += vertices.size() * sizeof(float);
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
		glBufferSubData(GL_ARRAY_BUFFER, startPos, sizeof(float)*(texcoords.size()), texcoords.data());
		glVertexAttribPointer(slotPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(startPos));
		glEnableVertexAttribArray(slotPos);
		startPos += sizeof(float)*(texcoords.size());
		slotPos++;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

Plane::~Plane()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void Plane::SetTex(unsigned int slot, unsigned int tex)
{
	
	for (int i = 0; i < mTexVec.size(); i++)
	{
		if (mTexVec[i].slot == slot)
		{
			mTexVec[i].texture = tex;
			return;
		}
	}
	PlaneTex planeTex;
	planeTex.slot = slot;
	planeTex.texture = tex;
	mTexVec.push_back(planeTex);
}

void Plane::Draw()
{
	mShader->use();
	for (const PlaneTex& planeTex : mTexVec)
	{
		glActiveTexture(planeTex.slot);
		glBindTexture(GL_TEXTURE_2D, planeTex.texture);
	}

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
}

void Plane::DrawShadowMap()
{
	mShadowShader->use();
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
}
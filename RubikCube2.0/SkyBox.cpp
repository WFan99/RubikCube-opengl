#include "SkyBox.h"
#include <stb_image.h>
using namespace std;

unsigned int SkyBox::LoadCubemapTexture(const char** path, bool flip)
{
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	stbi_set_flip_vertically_on_load(flip);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char *data = stbi_load(path[i], &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << path[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return cubemapTexture;
}

SkyBox::SkyBox(std::shared_ptr<Shader> shader) :Object3D(shader)
{
#if 0
	 vector<float> vertices = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f
	};

	 vector<unsigned int> indices = {
		 0,1,2,
		 2,3,1,

		 0,1,2,
		 2,3,1,
		 
		 0,1,2,
		 2,3,1,

		 0,1,2,
		 2,3,1,

		 0,1,2,
		 2,3,1,

		 0,1,2,
		 2,1,3,
	 };

	 glGenVertexArrays(1, &mVAO);
	 glGenBuffers(1, &mVBO);
	 glGenBuffers(1, &mEBO);

	 glBindVertexArray(mVAO);
	 glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

	 glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

	 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	 glEnableVertexAttribArray(0);
#endif

	 vector<float> vertices_ = {
		 // positions          
		 -1.0f,  1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,

		 -1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,

		  1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,

		 -1.0f, -1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,

		 -1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f, -1.0f,

		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f
	 };

	 glGenVertexArrays(1, &mVAO);
	 glGenBuffers(1, &mVBO);
	 glGenTextures(1, &mCubmapTexture);
	 glBindVertexArray(mVAO);
	 glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
	 glEnableVertexAttribArray(0);
	 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

SkyBox::~SkyBox()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void SkyBox::Draw()
{

	// draw skybox as last
	mShader->use();
	glDepthFunc(GL_EQUAL);
	// skybox cube
	glBindVertexArray(mVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mCubmapTexture);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}
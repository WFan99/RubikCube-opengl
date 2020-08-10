#pragma once
#include <learnopengl/shader_m.h>
#include <memory>
class Object3D
{
public:
	Object3D(std::shared_ptr<Shader> shader) :mShader(shader) {};
	virtual ~Object3D() {};
	virtual void Draw() = 0;

	static int LoadTexture(const char* path);

public:
	enum Attribute{Position = 1, Normal=2, Texture=4};

protected:
	std::shared_ptr<Shader> mShader;
};
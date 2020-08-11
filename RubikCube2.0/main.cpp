#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>

#include <iostream>

#include "Plane.h"
#include "Cube.h"
#include "Light.h"
#include "RubikCube.h"

unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 1200;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

Camera camera(glm::vec3(0.0f, 0.0F, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 2.0f, 2.0f);


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShadowMap", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// depthMapFBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	std::shared_ptr<Shader> objectShader =
		std::make_shared<Shader>("Object.vs", "Object.fs");
	std::shared_ptr<Shader> skyBoxShader =
		std::make_shared<Shader>("Skybox.vs", "Skybox.fs");
	std::shared_ptr<Shader> lightShader =
		std::make_shared<Shader>("Light.vs", "Light.fs");
	std::shared_ptr<Shader> ShadowQuadShader =
		std::make_shared<Shader>("ShadowQuad.vs", "ShadowQuad.fs");
	std::shared_ptr<Shader> shadowMapShader =
		std::make_shared<Shader>("ShadowMap.vs", "ShadowMap.fs");


	unsigned int planeTex = Object3D::LoadTexture("res/textures/wood.png");
	unsigned int cubeTex = Object3D::LoadTexture("res/textures/container.png");
	std::vector<unsigned int> skyBoxTex(6);

	skyBoxTex[Cube::Back] = Object3D::LoadTexture("res/skybox/back.jpg");
	skyBoxTex[Cube::Bottom] = Object3D::LoadTexture("res/skybox/bottom.jpg");
	skyBoxTex[Cube::Front] = Object3D::LoadTexture("res/skybox/front.jpg");
	skyBoxTex[Cube::Left] = Object3D::LoadTexture("res/skybox/left.jpg");
	skyBoxTex[Cube::Right] = Object3D::LoadTexture("res/skybox/right.jpg");
	skyBoxTex[Cube::Top] = Object3D::LoadTexture("res/skybox/top.jpg");

	unsigned int planeAttri = Object3D::Attribute::Position | Object3D::Attribute::Normal | Object3D::Attribute::Texture;
	Plane plane(objectShader, planeAttri);
	plane.SetTex(GL_TEXTURE0, planeTex);
	plane.SetTex(GL_TEXTURE1, depthMap);
	plane.SetShadowShader(shadowMapShader);

	unsigned int ShadowQuadTexureAttri = Object3D::Attribute::Position | Object3D::Attribute::Texture;
	Plane ShadowQuadTexture(ShadowQuadShader, ShadowQuadTexureAttri);
	ShadowQuadTexture.SetTex(GL_TEXTURE0, depthMap);

	unsigned int cubeAttri = Object3D::Attribute::Position | Object3D::Attribute::Normal |  Object3D::Attribute::Texture;
	Cube cube(objectShader, cubeAttri);
	cube.SetTex(GL_TEXTURE0, cubeTex);
	cube.SetTex(GL_TEXTURE1, depthMap);
	cube.SetShadowMapShader(shadowMapShader);

	RubikCube rubickCube(objectShader);

	
	unsigned int skyBoxAttri = Object3D::Attribute::Position | Object3D::Attribute::Texture;
	Cube skyBox(skyBoxShader, skyBoxAttri);
	skyBox.SetTex(GL_TEXTURE0,skyBoxTex);

	unsigned int lightAttri = Object3D::Attribute::Position;
	Cube lightCube(lightShader, lightAttri);
	
// 点光
	PointLight pointLight;
	pointLight.ambient = glm::vec3(0.2f);
	pointLight.diffuse = glm::vec3(0.5f);
	pointLight.specular = glm::vec3(1.0f);
	pointLight.position = lightPos;

	objectShader->use();
	objectShader->setFloat("material.shininess", 16.0f);
	objectShader->setInt("material.diffuse", 0);
	objectShader->setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
	objectShader->setInt("shadowMap", 1);


// 点光
	objectShader->setVec3("pointLight.ambient", pointLight.ambient);
	objectShader->setVec3("pointLight.diffuse", pointLight.diffuse);
	objectShader->setVec3("pointLight.position", pointLight.position);
	objectShader->setVec3("pointLight.specular", pointLight.specular);


	
	skyBoxShader->use();
	skyBoxShader->setInt("skybox", 0);

	lightShader->use();
	lightShader->setVec4("lightColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));



	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "fuck" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection* lightView;
#if 0
		glm::mat4 cubeModel = glm::mat4(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0, 0.5, 0));
		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shadowMapShader->setMat4("model", cubeModel);
		cube.DrawShadowMap();
#endif

		glm::mat4 planeModel = glm::mat4(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0, -0.5, 0));
		planeModel = glm::scale(planeModel, glm::vec3(5.0f));
		planeModel = glm::rotate(planeModel, glm::radians(-90.0f), glm::vec3(1.0f, 0, 0));
		shadowMapShader->setMat4("model", planeModel);
		plane.DrawShadowMap();

#if 0
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShadowQuadTexture.Draw();
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
			(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f );
		glm::mat4 view = camera.GetViewMatrix();
		objectShader->use();
		objectShader->setMat4("projection", projection);
		objectShader->setMat4("view", view);
		objectShader->setVec3("viewPos", camera.Position);
		objectShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		objectShader->setMat4("model", planeModel);
		plane.Draw();
		rubickCube.Draw();

		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));

		lightShader->use();
		lightShader->setMat4("model", lightModel);
		lightShader->setMat4("projection", projection);
		lightShader->setMat4("view", view);
		lightCube.Draw();

		glCullFace(GL_FRONT);
		glDepthFunc(GL_EQUAL);
		glm::mat4 skyBoxModel = glm::mat4(1.0f);
		skyBoxModel = glm::translate(skyBoxModel, glm::vec3(3, 1, 0));
		skyBoxShader->use();
		skyBoxShader->setMat4("model", skyBoxModel);
		skyBoxShader->setMat4("projection", projection);
		skyBoxShader->setMat4("view", glm::mat4(glm::mat3(view)));
		skyBox.Draw();
		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

 	 glDeleteFramebuffers(1, &depthMapFBO);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
	glViewport(0,0,width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
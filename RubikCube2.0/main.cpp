#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>

#include <iostream>

#include "Plane.h"
#include "Cube.h"
#include "Light.h"
#include "RubikCube.h"
#include "SkyBox.h"
#include "ObserverCamera.h"

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

RubikCube* pRubickCube = NULL;
ObserverCamera observerCamera(glm::vec3(0.0f,0.0f,3.0f),glm::vec3(0.0f));
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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
		std::make_shared<Shader>("shader/Object.vs", "shader/Object.fs");
	std::shared_ptr<Shader> skyBoxShader =
		std::make_shared<Shader>("shader/Skybox.vs", "shader/Skybox.fs");
	std::shared_ptr<Shader> lightShader =
		std::make_shared<Shader>("shader/Light.vs", "shader/Light.fs");
	std::shared_ptr<Shader> ShadowQuadShader =
		std::make_shared<Shader>("shader/ShadowQuad.vs", "shader/ShadowQuad.fs");
	std::shared_ptr<Shader> shadowMapShader =
		std::make_shared<Shader>("shader/ShadowMap.vs", "shader/ShadowMap.fs");
	std::shared_ptr<Shader> selectionMapShader =
		std::make_shared<Shader>("shader/SelectionMap.vs", "shader/SelectionMap.fs");
	std::shared_ptr<Shader> cubeEdgeShader =
		std::make_shared<Shader>("shader/CubeEdge.vs", "shader/CubeEdge.fs");


	unsigned int planeTex = Object3D::LoadTexture("res/textures/wood.png", true);

	std::vector<const char*> path = {
		"res/skybox/right.jpg",
		"res/skybox/left.jpg",
		"res/skybox/top.jpg",
		"res/skybox/bottom.jpg",
		"res/skybox/front.jpg",
		"res/skybox/back.jpg",
	};

	unsigned int skyBoxTex = SkyBox::LoadCubemapTexture(path.data(),false);


	unsigned int planeAttri = Object3D::Attribute::Position | Object3D::Attribute::Normal | Object3D::Attribute::Texture;
	Plane plane(objectShader, planeAttri);
	plane.SetTex(GL_TEXTURE0, planeTex);
	plane.SetTex(GL_TEXTURE1, depthMap);
	plane.SetShadowShader(shadowMapShader);

	unsigned int ShadowQuadTexureAttri = Object3D::Attribute::Position | Object3D::Attribute::Texture;
	Plane ShadowQuadTexture(ShadowQuadShader, ShadowQuadTexureAttri);
	ShadowQuadTexture.SetTex(GL_TEXTURE0, depthMap);


	RubikCube rubickCube(objectShader,cubeEdgeShader);
	rubickCube.SetShadowMapShader(shadowMapShader);
	rubickCube.SetSelectionShader(selectionMapShader);
	pRubickCube = &rubickCube;

	SkyBox skyBox(skyBoxShader);
	skyBox.SetCubmapTex(skyBoxTex);


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
	objectShader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
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
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

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
		float near_plane = 1.0f, far_plane = 15.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection* lightView;


		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		rubickCube.DrawShadowMap();

		glm::mat4 planeModel = glm::mat4(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0, -1.0, 0));
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		
		glm::mat4 projection = glm::perspective(glm::radians(observerCamera.GetZoom()),
			(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f );
		glm::mat4 view = observerCamera.GetViewMatrix();
		selectionMapShader->use();
		selectionMapShader->setMat4("projection", projection);
		selectionMapShader->setMat4("view", view);

		objectShader->use();
		objectShader->setMat4("projection", projection);
		objectShader->setMat4("view", view);
		objectShader->setVec3("viewPos", observerCamera.GetPosition());
		objectShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		objectShader->setMat4("model", planeModel);
		plane.Draw();
		glEnable(GL_STENCIL_TEST);
		cubeEdgeShader->use();
		cubeEdgeShader->setMat4("projection", projection);
		cubeEdgeShader->setMat4("view", view);
		rubickCube.Draw();
		glDisable(GL_STENCIL_TEST);

		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));

		lightShader->use();
		lightShader->setMat4("model", lightModel);
		lightShader->setMat4("projection", projection);
		lightShader->setMat4("view", view);
		lightCube.Draw();

		
		glm::mat4 skyBoxModel = glm::mat4(1.0f);
		skyBoxModel = glm::translate(skyBoxModel, glm::vec3(3, 1, 0));
		skyBoxShader->use();
		skyBoxShader->setMat4("projection", projection);
		skyBoxShader->setMat4("view", glm::mat4(glm::mat3(view)));
		skyBox.Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

 	 glDeleteFramebuffers(1, &depthMapFBO);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pRubickCube->ProcessKeyboardEvent(RubikCube::MoveLeft);
	else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pRubickCube->ProcessKeyboardEvent(RubikCube::MoveRight);
	else if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pRubickCube->ProcessKeyboardEvent(RubikCube::MoveUp);
	else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pRubickCube->ProcessKeyboardEvent(RubikCube::MoveDown);
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		observerCamera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		pRubickCube->ProcessClickEvent(xpos, ypos, true);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		pRubickCube->ProcessClickEvent(xpos, ypos, false);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	observerCamera.ProcessMouseScroll(yoffset);
}
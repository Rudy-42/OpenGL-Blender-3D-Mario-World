//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;
int i1 = 0;
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::vec3 campos;
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.07f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;
GLfloat ghostAngle;
GLfloat flowerpos = 0;
GLfloat flowerposa = 0.1f;
GLfloat flowerposb = 0.01f;
GLfloat bulletpos = 40.0f;

int i2, i3, i4 = 0;
bool tour = false;

gps::Model3D myModel;
gps::Model3D myPipe;
gps::Model3D myBullet;
gps::Model3D myGhost;
gps::Model3D myFlower;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;
bool okhere = false;
GLuint shadowMapFBO;
GLuint depthMapTexture;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	myCamera.mouse_callback(xpos, ypos);
}

void processMovement()
{

	/*if (pressedKeys[GLFW_KEY_Q]) {
		angle += 1.0f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 1.0f;
		if (angle < 0.0f)
			angle += 360.0f;
	}*/
	if (glfwGetKey(glWindow, GLFW_KEY_Z)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FRONT_AND_BACK);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_C)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_Q)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_E)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_W]) {
		
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_T]) {

		myCamera.tour0();
		tour = true;
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_ENTER]) {
		campos = myCamera.getCameraPosition();
		if (campos.x > -8.3 && campos.x < -6.3 && campos.y>2 && campos.y < 4 && campos.z>6.7 && campos.z < 8.2)
		{
			okhere = true;
			i3 = 1;
		}else
		if (campos.x > 7.4 && campos.x < 9.5 && campos.y>2 && campos.y < 4 && campos.z>-10.8 && campos.z < -9.1)
		{
			okhere = true;
			i3 = 0;
		}
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f; 
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}	
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	myModel = gps::Model3D("objects/scene/scene1.obj", "objects/scene/");
	ground = gps::Model3D("objects/ground/ground1.obj", "objects/ground/");
	myGhost = gps::Model3D("objects/scene/ghost.obj", "objects/scene/");
	myPipe = gps::Model3D("objects/scene/pipe.obj", "objects/scene/");
	myFlower = gps::Model3D("objects/scene/flower.obj", "objects/scene/");
	myBullet = gps::Model3D("objects/scene/bullet.obj", "objects/scene/");
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	processMovement();	

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
		
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	if (okhere)
	{
		if (i1++ > 300)
		{
			i1 = 0;
			i2++;
			
			if (i2 == 2)
			{
				okhere = false;
				i2 = 0;
			}
			else
				myCamera.movep(true, i2, i3);
		}
		else {
			myCamera.movep(false,i2,i3);
			
		}
	}

	if (tour)
	{
		if (i4++>500)
		{
			i4 = 0;
			tour = false;
		}
		else {
			myCamera.tour();
			if (i4 < 250)
			{
				myCamera.move(gps::MOVE_RIGHT, 0.03f);
				myCamera.move(gps::MOVE_UP, 0.02f);
			}
			else {
				myCamera.move(gps::MOVE_RIGHT, 0.03f);
				myCamera.move(gps::MOVE_DOWN, 0.02f);
			}
		}
	}

	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));

	myModel.Draw(depthMapShader);
	myPipe.Draw(depthMapShader);
	if (bulletpos < -250)
		bulletpos = 40;
	bulletpos -= 1;
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(bulletpos, -20.0f, -55.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	myBullet.Draw(depthMapShader);

	ghostAngle += 0.3f;
	//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	myGhost.Draw(depthMapShader);

	if (flowerpos > 0.0f || flowerpos < -25.0f)
	{
		flowerposa *= -1;
	}
	if (flowerpos > -1.0f && flowerpos < -0.5f)
		flowerpos += (flowerposa / 10);
	else
		flowerpos += flowerposa;
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(0.0f, flowerpos, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	myFlower.Draw(depthMapShader);
	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 
						1, 
						GL_FALSE, 
						glm::value_ptr(model));

	ground.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));	

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	
	//create model matrix for nanosuit
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::translate(model, glm::vec3(2, 0, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	myModel.Draw(myCustomShader);
	myPipe.Draw(myCustomShader);
	
	//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(bulletpos, -20.0f, -55.0f));
	//model = glm::translate(model, glm::vec3(0.0f, -50.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	myBullet.Draw(myCustomShader);

	ghostAngle += 0.3f;
	//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(ghostAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::translate(model, glm::vec3(0.0f, -50.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	myGhost.Draw(myCustomShader);

	
	//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(0.0f, flowerpos, 0.0f));
	//model = glm::translate(model, glm::vec3(0.0f, -50.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	myFlower.Draw(myCustomShader);
	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ground.Draw(myCustomShader);

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, 1.0f * lightDir);
	model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	glCheckError();
	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}

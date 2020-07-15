#ifndef OPENGLPIPELINESERVICE_H
#define OPENGLPIPELINESERVICE_H

#include <glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include <learnopengl/shader_m.h>
//#include <learnopengl/camera.h>

#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <memory>
#include <stddef.h>

#include <json/json.h>

#include "Vertex.h"
#include "FileLoadingService.h"
#include "ShaderProgram.h"
#include "CameraBufferObject.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

class OpenGLPipelineService {
private:
	const uint32_t _width = 3120 / 2; //TODO: Fix this for my actual screen
	const uint32_t _height = 1440 / 2;
	CameraBufferObject _ubo{ glm::lookAt(glm::vec3(40.0f, 40.0f, 56.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::perspective(glm::radians(90.0f), (float)_width / (float)_height, 0.1f, 65565.0f) };

	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> _window;


	bool _cameraUboGenerated = false;
	bool _lightPosUboGenerated = false;
	bool _transformDataUboGenerated = false;
	GLuint _cameraUboHandle = 0;
	GLuint _lightPosUboHandle = 0;
	GLuint _transformDataUboHandle = 0;
	GLuint _indicesHandle = 0;
	GLuint _instancesHandle = 0;

	GLuint _textureHandle = 0;
	GLuint _instanceVBO = 0;
	GLuint _cubeVAO = 0;
	GLuint _vboDataHandle = 0;

	ShaderProgram _phongProgram;

	const glm::vec3 v[8] = {
		{ +0.5f, +0.5f, -0.5f },
		{ +0.5f, -0.5f, -0.5f },
		{ +0.5f, +0.5f, +0.5f },
		{ +0.5f, -0.5f, +0.5f },
		{ -0.5f, +0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, +0.5f, +0.5f },
		{ -0.5f, -0.5f, +0.5f },
	};

	const glm::vec2 vTexture[20] = {
		{ +1.0f, +1.0f },
		{ +0.0f, +0.0f },
		{ +0.0f, +1.0f },
		{ +1.0f, +0.0f },
	};

	const glm::vec3 vNormal[6] = {
		{ +0.0f, +1.0f, +0.0f },
		{ +0.0f, +0.0f, +1.0f },
		{ -1.0f, +0.0f, +0.0f },
		{ +0.0f, -1.0f, +0.0f },
		{ +1.0f, +0.0f, +0.0f },
		{ +0.0f, +0.0f, -1.0f },
	};

	const std::vector<Vertex> _vertices = {
		{ v[4], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[0] },
		{ v[2], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[0] },
		{ v[0], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[0] },
		{ v[6], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[0] }, //3

		{ v[2], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[1] },
		{ v[7], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[1] },
		{ v[3], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[1] },
		{ v[6], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[1] }, //7

		{ v[6], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[2] },
		{ v[5], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[2] },
		{ v[7], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[2] },
		{ v[4], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[2] }, //11

		{ v[1], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[3] },
		{ v[7], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[3] },
		{ v[5], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[3] },
		{ v[3], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[3] }, //15

		{ v[0], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[4] },
		{ v[3], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[4] },
		{ v[1], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[4] },
		{ v[2], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[4] }, //19

		{ v[4], { +0.0f, +0.0f, +0.0f }, vTexture[0], vNormal[5] },
		{ v[1], { +0.0f, +0.0f, +0.0f }, vTexture[1], vNormal[5] },
		{ v[5], { +0.0f, +0.0f, +0.0f }, vTexture[2], vNormal[5] },
		{ v[0], { +0.0f, +0.0f, +0.0f }, vTexture[3], vNormal[5] },
	};

	const std::vector<GLushort> _indices = {
		0, 1, 2, 0, 3, 1,
		4, 5, 6, 4, 7, 5,
		8, 9, 10, 8, 11, 9,
		12, 13, 14, 12, 15, 13,
		16, 17, 18, 16, 19, 17,
		20, 21, 22, 20, 23, 21
	};

	size_t _voxelInstanceCount = 0;
	std::vector<glm::mat4> _transformData;

	void initVoxelData();

#if defined(_WIN32) || defined(_WIN64)

	HMODULE _optimus;
	void checkForOptimus(const char* library);
#endif
	void initWindow();
	
	GLuint getCameraUboHandle();
	GLuint getLightPosDataUboHandle();
	GLuint getTransformBufferDataUboHandle();
	void bindCameraUboForProgram(GLuint shaderProgramId);
	void bindTransformDataUboForProgram(GLuint shaderProgramId);
	void bindLightPosDataUboForProgram(GLuint shaderProgramId);
	ShaderProgram loadShaders(const std::string& vertexFileName, const std::string& fragmentFileName);
	void loadTextures();
	void setUpCubeInstancing();
	void bufferVertexData();
	void initOpenGLES();


	void handleCameraBufferObject();
	void handleLightPosData();
	void handleTransformDataBufferObject();
	void handleTexture();
	void handleVAODraw();
	void drawFrame();
	void mainLoop();

	void cleanup();

public:
	OpenGLPipelineService() noexcept;
	void launch();
};


#endif //!OPENGLPIPELINESERVICE_H
#ifndef TESTOPENGLLOGIC_H
#define TESTOPENGLLOGIC_H

#include <glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/shader_m.h>
//#include <learnopengl/camera.h>

#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

class TestOpenGLLogic {
private:
	const char* _vertexShaderSource = "#version 300 es\n"
		"precision mediump float;\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* _fragmentShaderSource = "#version 300 es\n"
		"precision mediump float;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

	const uint32_t _width = 3120 / 2; //TODO: Fix this for my actual screen
	const uint32_t _height = 1440 / 2;
	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> _window;
#if defined(_WIN32) || defined(_WIN64)

	HMODULE _optimus;
	void checkForOptimus(const char* library);
#endif
	void initWindow();

public:
	void doEverything();
};

#endif // !TESTOPENGLLOGIC_H

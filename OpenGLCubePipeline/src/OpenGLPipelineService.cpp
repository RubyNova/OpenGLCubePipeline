#include "../include/OpenGLPipelineService.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void OpenGLPipelineService::initVoxelData() {
    Json::Value root;

    auto path = std::filesystem::path("Resources") / "VoxelModels" / "Test.json";

    std::ifstream streamReader;
    streamReader.open(path);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    Json::String errs;

    if (!parseFromStream(builder, streamReader, &root, &errs)) {
        throw std::runtime_error(errs);
    }

    Json::Value array = root["vectorValues"];

    _voxelInstanceCount = array.size();

    for (size_t i = 0; i < _voxelInstanceCount; i++) {
        _transformData.emplace_back(glm::translate(glm::identity<glm::mat4>(), glm::vec3(array[static_cast<int>(i)][0].asFloat(), array[static_cast<int>(i)][1].asFloat(), array[static_cast<int>(i)][2].asFloat())));
    }
}

#if defined(_WIN64) || defined(_WIN32)
void OpenGLPipelineService::checkForOptimus(const char* library) {
    _optimus = LoadLibrary(reinterpret_cast<LPCSTR>(library));
}
#endif

void OpenGLPipelineService::initWindow() {
    //Check is only for Windows - stays inside definition checks
#if defined(_WIN64)
    static const char* OptimusLibraryName = "nvapi64.dll";
    checkForOptimus(OptimusLibraryName);
#elif defined(_WIN32)
    static const char* OptimusLibraryName = "nvapi.dll";
    checkForOptimus(OptimusLibraryName);
#endif
    glfwInit();

    std::cout << "Attempting standard ES 3.0 launch..." << std::endl;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    auto window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Attempting ES 3.0 launch with native API..." << std::endl;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

        window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);

        if (window == nullptr)
        {
            std::cout << "Attempting Core 4.3 launch with native API..." << std::endl;
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

#ifndef NDEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

            window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);

            if(window == nullptr) {
                throw std::runtime_error("Failed to create OpenGL context!");
            }
        }
    }

    _window = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(window, glfwDestroyWindow);
}

void OpenGLPipelineService::bindCameraUboForProgram(GLuint shaderProgramId) {
    GLuint uboIndex = glGetUniformBlockIndex(shaderProgramId, "CameraBufferObject");
    glUniformBlockBinding(shaderProgramId, uboIndex, 0);
}

void OpenGLPipelineService::bindTransformDataUboForProgram(GLuint shaderProgramId) {
    GLuint uboIndex = glGetUniformBlockIndex(shaderProgramId, "TransformBufferObject");
    glUniformBlockBinding(shaderProgramId, uboIndex, 1);
}

void OpenGLPipelineService::bindLightPosDataUboForProgram(GLuint shaderProgramId) {
    GLuint uboIndex = glGetUniformBlockIndex(shaderProgramId, "lightPosData");
    glUniformBlockBinding(shaderProgramId, uboIndex, 2);
}

GLuint OpenGLPipelineService::getCameraUboHandle() {
    if (!_cameraUboGenerated) {
        glGenBuffers(1, &_cameraUboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, _cameraUboHandle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, _cameraUboHandle, 0, sizeof(glm::mat4) * 2);
        _cameraUboGenerated = true;
    }

    return _cameraUboHandle;
}

GLuint OpenGLPipelineService::getLightPosDataUboHandle() {
    if (!_lightPosUboGenerated) {
        glGenBuffers(1, &_lightPosUboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightPosUboHandle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 2, _lightPosUboHandle, 0, sizeof(glm::vec3));
        _lightPosUboGenerated = true;
    }

    return _lightPosUboHandle;
}

GLuint OpenGLPipelineService::getTransformBufferDataUboHandle() {
    if (!_transformDataUboGenerated) {
        glGenBuffers(1, &_transformDataUboHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, _transformDataUboHandle);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 1024, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, _transformDataUboHandle, 0, sizeof(glm::mat4) * 1024);
        _transformDataUboGenerated = true;
    }

    return _transformDataUboHandle;
}

ShaderProgram OpenGLPipelineService::loadShaders(const std::string& vertexFileName, const std::string& fragmentFileName) {

    // Create the shaders
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    std::filesystem::path shadersDirPath = std::filesystem::path("Resources") / "Shaders";

    // Read the Vertex Shader code from the file
    std::string vertexShaderCode;

    std::ifstream VertexShaderStream(shadersDirPath / vertexFileName, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        vertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        throw std::runtime_error("Unable to compile vertex shader! Aborting...");
    }

    // Read the Fragment Shader code from the file
    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(shadersDirPath / fragmentFileName, std::ios::in);
    if (fragmentShaderStream.is_open()) {
        std::stringstream stringStream;
        stringStream << fragmentShaderStream.rdbuf();
        fragmentShaderCode = stringStream.str();
        fragmentShaderStream.close();
    }
    else {
        throw std::runtime_error("Unable to compile fragment shader! Aborting...");
    }

    GLint Result = GL_FALSE;
    int infoLogLength;

    // Compile Vertex Shader
    char const* vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderId, 1, &vertexSourcePointer, nullptr);
    glCompileShader(vertexShaderId);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(static_cast<size_t>(infoLogLength) + 1);
        glGetShaderInfoLog(vertexShaderId, infoLogLength, nullptr, &vertexShaderErrorMessage[0]);
        std::cout << std::string(&vertexShaderErrorMessage[0]) << std::endl;
        throw std::runtime_error("Unable to continue! Please fix the compile time error in the specified shader.");
    }

    // Compile Fragment Shader
    const char* FragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderId, 1, &FragmentSourcePointer, nullptr);
    glCompileShader(fragmentShaderId);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(static_cast<size_t>(infoLogLength) + 1);
        glGetShaderInfoLog(fragmentShaderId, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]);
        std::cout << std::string(&fragmentShaderErrorMessage[0]) << std::endl;
        throw std::runtime_error("Unable to continue! Please fix the compile time error in the specified shader.");
    }

    // Link the program
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    // Check the program
    glGetProgramiv(programId, GL_LINK_STATUS, &Result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(static_cast<size_t>(infoLogLength) + 1);
        glGetProgramInfoLog(programId, infoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cout << std::string(&ProgramErrorMessage[0]) << std::endl;
        throw std::runtime_error("Unable to continue! Please fix the specified error in the shader program.");
    }

    glDetachShader(programId, vertexShaderId);
    glDetachShader(programId, fragmentShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    ShaderProgram returnProg;
    returnProg.shaderProgramId = programId;
    returnProg.finalViewMatrixBufferUboId = getCameraUboHandle();
    returnProg.lightPosDataUboId = getLightPosDataUboHandle();
    returnProg.transformBufferDataUboId = getTransformBufferDataUboHandle();
    bindCameraUboForProgram(programId);
    bindTransformDataUboForProgram(programId);
    bindLightPosDataUboForProgram(programId);

    return returnProg;
}

void OpenGLPipelineService::loadTextures() {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("Resources/Textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    
    glGenTextures(1, &_textureHandle);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);

    int mode = GL_RGBA;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, texWidth, texHeight, 0, mode, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
}

void OpenGLPipelineService::setUpCubeInstancing() {
    glGenVertexArrays(1, &_cubeVAO);
    glGenBuffers(1, &_vboDataHandle);
    glGenBuffers(1, &_indicesHandle);
    bufferVertexData();
}

void OpenGLPipelineService::bufferVertexData() {
    glBindBuffer(GL_ARRAY_BUFFER, _vboDataHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * _indices.size(), _indices.data(), GL_STATIC_DRAW);
}

void OpenGLPipelineService::initOpenGLES() {
    glfwMakeContextCurrent(_window.get());
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Unable to initialise glad!");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loadTextures();
    setUpCubeInstancing();
    _phongProgram = loadShaders("Shader.vert", "Shader.frag");

}

void OpenGLPipelineService::handleCameraBufferObject() {
    glBindBuffer(GL_UNIFORM_BUFFER, _phongProgram.finalViewMatrixBufferUboId);
    CameraBufferObject ubo{};
    //should be 4, 4, 4
    ubo.view = glm::lookAt(glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(90.0f), _width / (float)_height, 0.1f, 10.0f);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraBufferObject), &ubo, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLPipelineService::handleLightPosData() {
    glBindBuffer(GL_UNIFORM_BUFFER, _phongProgram.lightPosDataUboId);
    glm::vec3 lightPos(4.0f, 4.0f, 4.0f);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), &lightPos, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLPipelineService::handleTransformDataBufferObject() {
    glBindBuffer(GL_UNIFORM_BUFFER, _phongProgram.transformBufferDataUboId);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * _transformData.size(), _transformData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLPipelineService::handleTexture() {
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
}

void OpenGLPipelineService::handleVAODraw() {
    glBindVertexArray(_cubeVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vboDataHandle);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, pos)
        );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _vboDataHandle);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)
        );

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _vboDataHandle);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, texCoord)
        );

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, _vboDataHandle);
    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
        );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesHandle);
    glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, 0, _transformData.size()); //infinite recursion followed by segfault occurs here. Stacktrace is useless.
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void OpenGLPipelineService::drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glUseProgram(_phongProgram.shaderProgramId);

    handleCameraBufferObject();
    handleTransformDataBufferObject();
    handleLightPosData();
    bufferVertexData();
    handleTexture();
    handleVAODraw();

    glfwSwapBuffers(_window.get());
}

void OpenGLPipelineService::mainLoop() {
    while (!glfwWindowShouldClose(_window.get())) {
        glfwPollEvents();
        drawFrame();
    }
}

void OpenGLPipelineService::cleanup() {
    glDeleteVertexArrays(1, &_cubeVAO);
    _window.reset();
    glfwTerminate();
}

OpenGLPipelineService::OpenGLPipelineService() noexcept : _window(nullptr, nullptr) {
}

void OpenGLPipelineService::launch() {
    initVoxelData();
    initWindow();
    initOpenGLES();
    mainLoop();
    cleanup();
}

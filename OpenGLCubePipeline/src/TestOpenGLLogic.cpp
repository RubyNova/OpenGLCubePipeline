//#include "..\include\TestOpenGLLogic.h"
//
//#if defined(_WIN64) || defined(_WIN32)
//void TestOpenGLLogic::checkForOptimus(const char* library) {
//    _optimus = LoadLibrary(reinterpret_cast<LPCSTR>(library));
//}
//#endif
//
//void TestOpenGLLogic::initWindow() {
//    //Check is only for Windows - stays inside definition checks
//#if defined(_WIN64)
//    static const char* OptimusLibraryName = "nvapi64.dll";
//    checkForOptimus(OptimusLibraryName);
//#elif defined(_WIN32)
//    static const char* OptimusLibraryName = "nvapi.dll";
//    checkForOptimus(OptimusLibraryName);
//#endif
//    glfwInit();
//
//    std::cout << "Attempting standard ES 3.0 launch..." << std::endl;
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
//
//    auto window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);
//
//    if (window == nullptr)
//    {
//        std::cout << "Attempting ES 3.0 launch with native API..." << std::endl;
//        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
//
//        window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);
//
//        if (window == nullptr)
//        {
//            std::cout << "Attempting Core 4.3 launch with native API..." << std::endl;
//            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
//            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
//            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//            glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
//
//#ifndef NDEBUG
//            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
//#endif
//
//            window = glfwCreateWindow(_width, _height, "OpenGL Cube Window", nullptr, nullptr);
//
//            if (window == nullptr) {
//                throw std::runtime_error("Failed to create OpenGL context!");
//            }
//        }
//    }
//
//    _window = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(window, glfwDestroyWindow);
//}
//
//void TestOpenGLLogic::doEverything() {
//    glfwMakeContextCurrent(_window.get());
//    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
//        throw std::runtime_error("Unable to initialise glad!");
//    }
//
//    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertShader, 1, &_vertexShaderSource, NULL);
//    glCompileShader(vertShader);
//    int  success;
//    char infoLog[512];
//    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
//
//    if (!success)
//    {
//        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//
//    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragShader, 1, &_fragmentShaderSource, NULL);
//    glCompileShader(fragShader);
//    int  success;
//    char infoLog[512];
//    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
//
//    if (!success)
//    {
//        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//
//    GLuint shaderProgram = glCreateProgram();
//
//    glAttachShader(shaderProgram, vertShader);
//    glAttachShader(shaderProgram, fragShader);
//    glLinkProgram(shaderProgram);
//
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//
//    if (!success)
//    {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//    
//    glDeleteShader(vertShader);
//    glDeleteShader(fragShader);
//
//    GLuint VBO;
//    glGenBuffers(1, &VBO);
//
//    float vertices[] = {
//    -0.5f, -0.5f, 0.0f,
//     0.5f, -0.5f, 0.0f,
//     0.0f,  0.5f, 0.0f
//    };
//
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    GLuint VAO;
//    glGenVertexArrays(1, &VAO);
//
//
//
//    while (!glfwWindowShouldClose(_window.get())) {
//        glfwPollEvents();
//        glUseProgram(shaderProgram);
//        glBindVertexArray(VAO);
//        // 0. copy our vertices array in a buffer for OpenGL to use
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//        // 1. then set the vertex attributes pointers
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        // 2. use our shader program when we want to render an object
//
//
//
//        glBindVertexArray(0);
//
//    }
//
//
//    _window.reset();
//}

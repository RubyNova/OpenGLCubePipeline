# CMakeList.txt : CMake project for OpenGLCubePipeline, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

find_path(STB_INCLUDES stb_image.h include)



# Add source to this project's executable.
add_executable (OpenGLCubePipeline "OpenGLCubePipeline.cpp" "OpenGLCubePipeline.h" "src/OpenGLPipelineService.cpp" "include/OpenGLPipelineService.h" "include/FileLoadingService.h" "include/Vertex.h" "include/CameraBufferObject.h" "include/ShaderProgram.h")
target_include_directories(OpenGLCubePipeline PRIVATE ${STB_INCLUDES})

target_link_libraries(OpenGLCubePipeline glfw glad::glad glm jsoncpp_lib)

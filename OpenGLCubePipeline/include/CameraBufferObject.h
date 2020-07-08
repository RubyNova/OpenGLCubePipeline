#ifndef CAMERABUFFEROBJECT_H
#define CAMERABUFFEROBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
};

#endif //!CAMERABUFFEROBJECT_H
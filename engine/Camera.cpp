#include "Camera.h"

LIB_API Camera::Camera() {

}

LIB_API Camera::~Camera() {

}

glm::mat4 LIB_API Camera::inverseCamera() const{
    return m_camera_inverse;
}

void LIB_API Camera::setTransformation(const glm::mat4& transformation)
{
    m_camera = transformation;
    m_camera_inverse = glm::inverse(m_camera);
}

void LIB_API Camera::render(std::shared_ptr<Object> camera){

}

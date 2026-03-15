#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::normalize(cameraUp);
        
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 front = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 right = glm::normalize(glm::cross(front, cameraUpDirection));

        glm::vec3 delta(0.0f);

        switch (direction) {
        case MOVE_FORWARD:  delta = front * speed; break;
        case MOVE_BACKWARD: delta = -front * speed; break;
        case MOVE_RIGHT:    delta = right * speed; break;
        case MOVE_LEFT:     delta = -right * speed; break;
        }

        cameraPosition += delta;
        cameraTarget += delta;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 front = glm::normalize(cameraTarget - cameraPosition);

        // yaw around up
        front = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUpDirection)) * front;

        // recompute right after yaw
        glm::vec3 right = glm::normalize(glm::cross(front, cameraUpDirection));

        // pitch around right
        front = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right)) * front;

        cameraTarget = cameraPosition + glm::normalize(front);
    }
}

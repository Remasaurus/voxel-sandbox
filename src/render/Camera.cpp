#include "Camera.h"
#include <algorithm>

glm::mat4 Camera::view() const {
    return glm::lookAt(position, position + front, UP);
}

glm::mat4 Camera::projection() const {
    return glm::perspective(
        glm::radians(fov), static_cast<float>(size.x) / size.y, NEAR_CLIP_PLANE,
        FAR_CLIP_PLANE
    );
}

void Camera::rotate(const glm::vec2 &rotation_offset) {
    rotation += rotation_offset;
    rotation.y = std::clamp(rotation.y, -PITCH_LIMIT, PITCH_LIMIT);
    update_vectors();
}

void Camera::update_vectors() {
    front = glm::normalize(
        glm::vec3(
            std::cos(rotation.x) * std::cos(rotation.y), std::sin(rotation.y),
            std::sin(rotation.x) * std::cos(rotation.y)
        )
    );

    right = glm::normalize(glm::cross(front, UP));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::move(const glm::vec3 direction, const float deltatime) {
    glm::vec3 move_distance = direction * deltatime;

    // move in the move direction from the camera's point of view
    position += move_distance.x * right + move_distance.y * front +
                move_distance.z * UP;
}

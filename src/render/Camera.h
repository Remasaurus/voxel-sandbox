#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera() = default;

    // Member variables
    glm::vec3 front = {1, 0, 0};
    glm::vec3 right = {0, 0, 1};
    glm::vec3 up = UP;
    glm::vec3 position{};

    glm::vec2 rotation = glm::vec2(-90.0f, 0.0f);
    glm::vec2 size{};

    glm::mat4 view() const;
    glm::mat4 projection() const;

    void rotate(const glm::vec2 &rotation_offset);

    float fov = 45.0f;

    static constexpr float NEAR_CLIP_PLANE = 0.1f;
    static constexpr float FAR_CLIP_PLANE = 100.0f;
    static constexpr float PITCH_LIMIT = glm::radians(89.0f);

    static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

private:
    void update_vectors();
};

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(const glm::vec3 &cam_pos, const glm::vec3 &cam_target,
         const glm::vec2 &render_dimensions);

  void set_fov(float a_fov);
  void update_fov(float offset);
  void set_render_dimensions(size_t width, size_t height);

  glm::vec3 get_position() const;
  glm::vec3 get_look_direction() const;
  glm::mat4 get_view_matrix() const;
  glm::mat4 get_projection_matrix() const;

  void handle_movement(const glm::vec3 &move_input, float deltatime);
  void handle_rotation(const glm::vec2 &rotation_offset);

private:

  // Constants 
  static constexpr float FOV_MAX = 45.0f;
  static constexpr float FOV_MIN = 1.0f;

  static constexpr float NEAR_CLIP_PLANE = 0.1f;
  static constexpr float FAR_CLIP_PLANE = 100.0f;

  static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

  // Member variables
  glm::vec3 position;
  glm::vec3 cam_front;
  glm::vec3 cam_right;
  glm::vec3 cam_up;

  glm::vec2 rotation = glm::vec2(-90.0f, 0.0f);

  float fov = 45.0f;
  float sensitivity = glm::radians(0.25f);
  float speed = 3.0f;
  size_t render_width, render_height;
};
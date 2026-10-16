#include "Camera.h"

constexpr float PITCH_LIMIT = glm::radians(89.0f);

Camera::Camera(const glm::vec3 &cam_pos, const glm::vec3 &cam_target,
               const glm::vec2 &render_dimensions)
    : position(cam_pos) {
  cam_front = glm::normalize(cam_target - cam_pos); // target -> pos
  cam_right = glm::cross(cam_front, UP);
  cam_up = glm::cross(cam_front, cam_right);

  render_width = render_dimensions.x;
  render_height = render_dimensions.y;
}

void Camera::set_fov(float a_fov) {
  fov = a_fov;
  // clamp to valid fov-range
  if (fov < FOV_MIN)
    fov = FOV_MIN;
  if (fov > FOV_MAX)
    fov = FOV_MAX;
}

void Camera::update_fov(float offset) {
  fov += offset;
  // clamp to valid fov-range
  if (fov < FOV_MIN)
    fov = FOV_MIN;
  if (fov > FOV_MAX)
    fov = FOV_MAX;
}

void Camera::set_render_dimensions(float width, float height) {
  render_width = width;
  render_height = height;
}

glm::vec3 Camera::get_position() const { return position; }
glm::vec3 Camera::get_look_direction() const { return cam_front; }
glm::mat4 Camera::get_view_matrix() const {
  return glm::lookAt(position, position + cam_front, UP);
}

glm::mat4 Camera::get_projection_matrix() const {
  return glm::perspective(glm::radians(fov), render_width / render_height,
                          NEAR_CLIP_PLANE, FAR_CLIP_PLANE);
  // if (orthographic_mode)
  // return glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
}

void Camera::handle_movement(const glm::vec3 &move_input, float deltatime) {
  glm::vec3 move_distance = move_input * speed * deltatime;

  // move in the move direction from the camera's point of view
  position += move_distance.x * cam_right + move_distance.y * cam_front +
              move_distance.z * UP;
}

void Camera::handle_rotation(const glm::vec2 &rotation_offset) {

  rotation += rotation_offset * sensitivity;
  // clamp to valid pitch range
  if (rotation.y > PITCH_LIMIT)
    rotation.y = PITCH_LIMIT;
  if (rotation.y < -PITCH_LIMIT)
    rotation.y = -PITCH_LIMIT;

  // calculate new front vector with current rotation
  cam_front = glm::normalize(glm::vec3(cosf(rotation.x) * cosf(rotation.y),
                                       sinf(rotation.y),
                                       sinf(rotation.x) * cosf(rotation.y)));

  // update rest of camera vectors
  cam_right = glm::cross(cam_front, UP);
  cam_up = glm::cross(cam_front, cam_right);
}
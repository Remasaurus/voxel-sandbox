#include "Brush.h"
#include <glm/gtx/norm.hpp>

bool Brush::within_radius(const glm::ivec3 &pos, const glm::ivec3 voxel_pos) {
    return glm::length2(glm::vec3(pos - voxel_pos)) <
           static_cast<int>(radius * radius);
}

void Brush::fill(VoxelMap &map, const glm::ivec3 &pos, Voxel new_voxel) {
    map.dispatch(
        bounding_box_min(pos), bounding_box_max(pos),
        [&](const glm::ivec3 &voxel_pos, Voxel &vox) {
            if (!within_radius(pos, voxel_pos))
                return;

            if (vox.isAir()) {
                vox = new_voxel;
            }
        }
    );
}

void Brush::replace(VoxelMap &map, const glm::ivec3 &pos, Voxel new_voxel) {
    map.dispatch(
        bounding_box_min(pos), bounding_box_max(pos),
        [&](glm::ivec3 voxel_pos, Voxel &vox) {
            if (!within_radius(pos, voxel_pos))
                return;

            vox = new_voxel;
        }
    );
}

void Brush::ignite(VoxelMap &map, const glm::ivec3 &pos) {
    map.dispatch(
        bounding_box_min(pos), bounding_box_max(pos),
        [&](glm::ivec3 voxel_pos, Voxel &vox) {
            if (!within_radius(pos, voxel_pos))
                return;

            // TODO: ignite vox
            vox.data &= 0b101010;
        }
    );
}

glm::ivec3 Brush::bounding_box_min(const glm::ivec3 &pos) {
    return pos - glm::ivec3(static_cast<int>(radius));
}

glm::ivec3 Brush::bounding_box_max(const glm::ivec3 &pos) {
    return pos + glm::ivec3(static_cast<int>(radius));
}

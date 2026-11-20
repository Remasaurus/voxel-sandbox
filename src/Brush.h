#include "voxel/VoxelMap.h"

class Brush {
public:
    Brush() = default;

    unsigned int radius = 5;

    void fill(VoxelMap &map, const glm::ivec3 &pos, Voxel voxel);
    void replace(VoxelMap &map, const glm::ivec3 &pos, Voxel voxel);
    void ignite(VoxelMap &map, const glm::ivec3 &pos);

private:
    glm::ivec3 bounding_box_min(const glm::ivec3 &pos);
    glm::ivec3 bounding_box_max(const glm::ivec3 &pos);

    bool within_radius(const glm::ivec3 &pos, const glm::ivec3 voxel_pos);
};

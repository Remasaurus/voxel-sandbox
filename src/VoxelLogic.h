#include <GLFW/glfw3.h>

class VoxelLogic
{
private:
    /* data */
    enum VoxelType {
        stone   = 0,
        dirt    = 1,
        grass   = 2,
        sand    = 3,
        air     = 4,
        wood    = 5,
        water   = 6,
        fire    = 7,
        steam   = 8,
        snow    = 9 
    };

    struct voxel {
        int x; // TODO is int de goeie type?
        int y;
        int z;
        VoxelType type;
    };

    void stone_logic(voxel &voxel);

public:
    std::vector<std::vector>> grid; // TODO iew dit moeten we wel aanpassen

    VoxelLogic(/* args */);
    ~VoxelLogic();
};

VoxelLogic::VoxelLogic(/* args */)
{
}

VoxelLogic::~VoxelLogic()
{
}
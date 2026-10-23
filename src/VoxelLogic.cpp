#include "VoxelLogic.h"
// Voxel logic: what should happen for every voxel in any situation

// class stone
void VoxelLogic::stone_logic(voxel &voxel) {
    // fall down
    if (grid[voxel.x, voxel.y - 1, voxel.z] == air) {
        voxel.y -= 1;
    }
}

// class dirt
void VoxelLogic::dirt_logic(voxel &voxel) {
    // fall down
    if (grid[voxel.x, voxel.y - 1, voxel.z] == air) {
        voxel.y -= 1;
    }
    // fall down in heap TODO how to fix this clunkyness?
    // if (grid[voxel.x + 1, voxel.y - 1, voxel.z + 1] == air ||
    //     grid[voxel.x - 1, voxel.y - 1, voxel.z + 1] == air || 
    //     grid[voxel.x + 1, voxel.y - 1, voxel.z - 1] == air || 
    //     grid[voxel.x - 1, voxel.y - 1, voxel.z - 1] == air || 
    // ) {
    //     voxel.y -= 1;
    // }
    // grow grass
    if (grid[voxel.x + 1] == grass ||
        grid[voxel.x - 1] == grass ||
        grid[voxel.z + 1] == grass ||
        grid[voxel.z - 1] == grass ||
        ) 
        // random chance of growing grass each tick
        {
            if (rand() % 10000000 < 1) // TODO: randomness aanpassen
                voxel.type == grass;
        }
}

// class grass
void VoxelLogic::dirt_logic(voxel &voxel) {
    // fall down
    if (grid[voxel.y - 1] == air) {
        voxel.y -= 1;
    }
}

// class sand
void VoxelLogic::dirt_logic(voxel &voxel) {
    // fall down
    if (grid[voxel.y - 1] == air) {
        voxel.y -= 1;
    }
}

// steen
// - aarde
// - gras
//     - groeit over aarde
// - zand
//     - landt in een hoopje met een punt
// - lucht
// - hout 
//     - render bomen in combinatie met blaadjes
//     - blaadjes
//     - vuur brandt hierop
// - sneeuw
//     - randomly falls from the sky periodically
//     - smelt na een tijdje (verdwijnt)
// - vuur
//     - smelt sneeuw
//     - brandt op hout
// - water
//     - 
// - stoom                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
//     - ontstaat door water op vuur te gooien
//     - als stoom tegen een solid aankomt, wordt het weer een water druppeltje


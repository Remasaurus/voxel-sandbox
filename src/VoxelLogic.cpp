// Voxel logic: what should happen for every voxel in any situation


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


// // class stone
// void VoxelLogic::stone_update(voxel &voxel) {
//     if (grid[voxel.y - 1] == air) {
//         voxel.y -= 1;
//     }
// }

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


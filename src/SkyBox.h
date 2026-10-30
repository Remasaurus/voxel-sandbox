

#include "libs.h"

// using https://github.com/VictorGordan/opengl-tutorials/tree/main/YoutubeOpenGL%2019%20-%20Cubemaps%20%26%20Skyboxes

class SkyBox {
    public:
    void


    private:
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;

    // All the faces of the cubemap
	std::string facesCubemap[6] =
	{
		"/skybox_images/skybox-img.png", // right
		"/skybox_images/skybox-img.png", // left
		"/skybox_images/skybox-top.png", // top
		"/skybox_images/skybox-bot.png", // bottom
		"/skybox_images/skybox-img.png", // front
		"/skybox_images/skybox-img.png", // back
	};

    float skyboxVertices[] =
    {
        // Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
         1.0f, -1.0f,  1.0f,//       /|       /|
         1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
         1.0f,  1.0f,  1.0f,//      |/       |/
         1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };

    unsigned int skyboxIndices[] =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    void create_buffers();
    void create_texture();
    void attach_texture();

}

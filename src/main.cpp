#include "Application.h"
#include "Log.h"

int main(void) {
    glfwInit();
    
    // TODO: add command line argument support
    int exit_code;
    {
        Application app;
        exit_code = app.run();
    }

    glfwTerminate();

    return exit_code;
}
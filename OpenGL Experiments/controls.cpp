#include "controls.h"

controls::controls(GLFWwindow* _window) {
    //A reference to the GLFW window
    window = _window;
    
    //The position of the camera in world space
    position = glm::vec3(0, 0, 5);
    
    //The horizontal view angle: towards -z
    horizontalAngle = 3.14f;
    
    //The vertical view angle: 0 means look at the horizon
    verticalAngle = 0.0f;
    
    //The field of view
    initialFoV = 45.0f;
    
    //3 units per second
    speed = 3.0f;
    
    //Slow down the mouse's effect on rotation
    mouseSpeed = 0.005f;
}

glm::mat4 controls::getViewMatrix() {
    return viewMatrix;
}

glm::mat4 controls::getProjectionMatrix() {
    return projectionMatrix;
}

void controls::computeMatricesFromInputs() {
    //The storage for these variables is not on the stack but instead in the program's static data area
    //They are initialized once and retain their value between function calls (unless manually altered, as occurs at the end of this function)
    static double lastTime = glfwGetTime();
    static double xpos_old, ypos_old;
    
    //Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    //Get the mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    //Reset the mouse position for the next frame
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    //Compute our viewing angles
    //1. How far is the mouse from the center of the window? The greater the distance, the more we want to turn
    //2. Convert (1) to a floating point value so that multiplication goes well...
    //3. Speed up or slow down the rotation with mouseSpeed (feel free to change this)
    //4. Add or subtract from the current rotation - don't use "="
    horizontalAngle += mouseSpeed * float( xpos_old - xpos );
    verticalAngle += mouseSpeed * float( ypos_old - ypos );
    xpos_old = xpos;
    ypos_old = ypos;
    
    //Direction: spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    const float half_pi = glm::pi<float>() / 2.0f;
    
    //Right vector
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - half_pi),
                                0,
                                cos(horizontalAngle - half_pi)
                                );
    
    //Up vector: perpendicular to the previous two vectors
    glm::vec3 up = glm::cross(right, direction);
    
    //Move forward
    if (glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    //Move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    //Strafe right
    if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    //Strafe left
    if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
    
    float FoV = initialFoV;
    
    //Projection matrix: 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    
    //Camera matrix
    viewMatrix = glm::lookAt(
                           position,           // Camera is here
                           position+direction, // and looks here : at the same position, plus "direction"
                           up                  // Head is up (set to 0,-1,0 to look upside-down)
                           );
    lastTime = currentTime;
}
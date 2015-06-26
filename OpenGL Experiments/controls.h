#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class controls {
public:
    controls(GLFWwindow* _window);
    glm::mat4       getProjectionMatrix();
    glm::mat4       getViewMatrix();
    void            computeMatricesFromInputs();    
private:
    GLFWwindow*     window;
    float           horizontalAngle;
    float           verticalAngle;
    float           initialFoV;
    glm::vec3       position;
    glm::mat4       projectionMatrix;
    glm::mat4       viewMatrix;
    float           speed;
    float           mouseSpeed;
};
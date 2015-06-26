#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class objloader {
public:
    objloader();
    bool loadOBJ(const string &filename,
                 vector<glm::vec3> &out_vertices,
                 vector<glm::vec2> &out_uvs,
                 vector<glm::vec3> &out_normals);
    

private:
};
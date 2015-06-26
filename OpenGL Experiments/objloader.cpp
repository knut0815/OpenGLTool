#include "objloader.h"

objloader::objloader() {
    
}

/*
 *
 * Here is an example obj file
 *
 */

/*
    # Blender3D v249 OBJ File: untitled.blend
    # www.blender3d.org
    mtllib cube.mtl
    v 1.000000 -1.000000 -1.000000
    v 1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 -1.000000
    v 1.000000 1.000000 -1.000000
    v 0.999999 1.000000 1.000001
    v -1.000000 1.000000 1.000000
    v -1.000000 1.000000 -1.000000
    vt 0.748573 0.750412
    vt 0.749279 0.501284
    vt 0.999110 0.501077
    vt 0.999455 0.750380
    vt 0.250471 0.500702
    vt 0.249682 0.749677
    vt 0.001085 0.750380
    vt 0.001517 0.499994
    vt 0.499422 0.500239
    vt 0.500149 0.750166
    vt 0.748355 0.998230
    vt 0.500193 0.998728
    vt 0.498993 0.250415
    vt 0.748953 0.250920
    vn 0.000000 0.000000 -1.000000
    vn -1.000000 -0.000000 -0.000000
    vn -0.000000 -0.000000 1.000000
    vn -0.000001 0.000000 1.000000
    vn 1.000000 -0.000000 0.000000
    vn 1.000000 0.000000 0.000001
    vn 0.000000 1.000000 -0.000000
    vn -0.000000 -1.000000 0.000000
    usemtl Material_ray.png
    s off
    f 5/1/1 1/2/1 4/3/1
    f 5/1/1 4/3/1 8/4/1
    f 3/5/2 7/6/2 8/7/2
    f 3/5/2 8/7/2 4/8/2
    f 2/9/3 6/10/3 3/5/3
    f 6/10/4 7/6/4 3/5/4
    f 1/2/5 5/1/5 2/9/5
    f 5/1/6 6/10/6 2/9/6
    f 5/1/7 8/11/7 6/10/7
    f 8/11/7 7/12/7 6/10/7
    f 1/2/8 2/9/8 3/13/8
    f 1/2/8 3/13/8 4/14/8
 */

/*
    In the above example:
    1. # are comments
    2. usemtl and mtllib describe the material properties of the object (ignored here)
    3. v is a vertex
    4. vt is a texture coordinate
    5. vn is a normal
    6. f is a face where each of the triplets are a vertex (vertex index, uv index, normal index)
 */

bool objloader::loadOBJ(const string &filename,
                        vector<glm::vec3> &out_vertices,
                        vector<glm::vec2> &out_uvs,
                        vector<glm::vec3> &out_normals) {
    
    //Temporary variables for obj contents
    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    
    //Open the file
    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL) {
        cerr << "Failed to open the specifed file." << endl;
        return false;
    }
    
    while (true) {
        //We assume the first word of a line won't be longer than 128 characters...not the best solution
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        
        //If we're at the end of the file, quit
        if (res == EOF) {
            break;
        }
        
        if (strcmp(lineHeader, "v") == 0) { //Vertices
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) { //UVs
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) { //Normals
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0],
                                 &uvIndex[0],
                                 &normalIndex[0],
                                 &vertexIndex[1],
                                 &uvIndex[1],
                                 &normalIndex[1],
                                 &vertexIndex[2],
                                 &uvIndex[2],
                                 &normalIndex[2]);
            if (matches != 9) {
                cerr << "File can't be parsed." << endl;
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            
            for (unsigned int i = 0; i < vertexIndices.size(); i++) {
                unsigned int vertexIndex = vertexIndices[i];
                
                //Objs are indexed starting at 1
                glm::vec3 vertex = temp_vertices[vertexIndex-1];
                out_vertices.push_back(vertex);
                
                glm::vec2 uv = temp_uvs[vertexIndex-1];
                out_uvs.push_back(uv);
                
                glm::vec3 normal = temp_normals[vertexIndex-1];
                out_normals.push_back(normal);
            }
        }
    }
    return true;
}
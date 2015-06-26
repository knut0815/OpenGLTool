//GLFW for window management
//GLEW for OpenGL extensions
//GLM for GL mathematics
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "controls.h"

#define CUBE
//#define DRAW_WIREFRAME

using namespace std;

string loadFileToString(const char *filePath) { //Reads a file into memory and returns a string containing that file data
    string fileData;
    ifstream stream(filePath, ios::in); //Path and mode
    if (stream.is_open()) {
        cout << "Successfully opened file stream." << endl;
        string line = "";
        while (getline(stream, line)) { //Takes a stream and an editable string and feeds the line from the stream into the line
            fileData += "\n" + line;
        }
        stream.close();
    }
    else {
        cerr << "Failed to open file stream." << endl;
    }
    return fileData;
}

//Should change these to std::string and use string.c_str() when needed (i.e. for fopen)
GLuint loadShaders(const char *vertShaderPath, const char *fragShaderPath) {
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER); //Create IDs for our shaders
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER); //Create IDs for our shaders
    
    string vertShaderSource = loadFileToString(vertShaderPath);
    string fragShaderSource = loadFileToString(fragShaderPath);
    
    //glShaderSource requires C-style string parameters, so we do the conversion here
    const char *rawVertShaderSource = vertShaderSource.c_str();
    const char *rawFragShaderSource = fragShaderSource.c_str();
    
    glShaderSource(vertShader, 1, &rawVertShaderSource, NULL);
    glShaderSource(fragShader, 1, &rawFragShaderSource, NULL);
    
    //VERTEX SHADER
    glCompileShader(vertShader);
    GLint success = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    
    //Error logging
    if (success == GL_FALSE) {
        cerr << "Error compiling vertex shader." << endl;
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);
        
        for (int i = 0; i < errorLog.size(); i++) {
            cerr << errorLog[i];
        }
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(vertShader); // Don't leak the shader.
    }
    
    //FRAGMENT SHADER
    glCompileShader(fragShader);
    success = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    
    //Error logging
    if (success == GL_FALSE) {
        cerr << "Error compiling fragment shader." << endl;
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);
        
        for (int i = 0; i < errorLog.size(); i++) {
            cerr << errorLog[i];
        }
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(fragShader); // Don't leak the shader.
    }
    
    GLuint program = glCreateProgram(); //Create a program
    glAttachShader(program, vertShader); //Attach shaders
    glAttachShader(program, fragShader); //Attach shaders
    glLinkProgram(program); //We are going to use this program in our application
    
    cout << "Successfully loaded shader sources." << endl;
    
    return program;
}

GLuint loadBmp(const char *filePath) {
    //BMP files always begin with a 54-byte header
    unsigned char header[54];
    
    //Position in the file where the actual image data begins
    unsigned int dataPos;
    unsigned int width, height;
    
    //Will be width*height*3 (for RGB)
    unsigned int imageSize;
    
    //The actual RGB data
    unsigned char *data;
    
    //Ensure that the file was successfully opened
    FILE *file = fopen(filePath, "rb");
    if (!file) {
        cerr << "Image file could not be opened." << endl;
        return 0;
    }
    
    //Ensure that the given file is actually BMP-formatted (has a 54-byte header)
    if (fread(header, 1, 54, file) != 54) {
        cerr << "Not the correct file format. Image should be a BMP file." << endl;
        return 0;
    }
    
    //BMP file headers always begin with 'B' and 'M'
    if (header[0] != 'B' || header[1] != 'M') {
        cerr << "Not the correct file format. Image should be a BMP file." << endl;
        return 0;
    }
    
    //Read the size of the image, the location of the data in the file, etc.
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);
    
    //Since some BMP files are misformatted, make up some information
    if (imageSize == 0) {
        //One byte for each R, G, and B component
        imageSize = width*height*3;
    }
    if (dataPos == 0) {
        dataPos = 54;
    }
    
    //Create a buffer
    data = new unsigned char[imageSize];
    
    //Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);
    
    //Everything should be in memory now, so close the file
    fclose(file);
    
    //Create one OpenGL texture
    GLuint texID;
    glGenTextures(1, &texID);
    
    //"Bind" the newly created texture so that all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texID);
    
    //Give the image data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    //When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //Generate mipmaps, by the way.
    glGenerateMipmap(GL_TEXTURE_2D);
    
    cout << "Successfully loaded a texture from the provided BMP file." << endl;
    
    return texID;
}

int main(void) {
    //The window
    GLFWwindow* window;
    
    //Initialize the GLFW library
    if (!glfwInit()) {
        cerr << "GLFW failed to initialize" << endl;
        return -1;
    }
    
    /* 
     *
     * OPTIONAL window hints (settings)
     *
     */
    
    //4 anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    //Set the OpenGL major and minor versions to 3 (essentially setting up OpenGL 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    //We only want the core OpenGL functionality...we don't care about backwards-compatibility right now
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //Create a window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        cerr << "GLFW window failed to create" << endl;
        glfwTerminate();
        return -1;
    }
    
    //Hides the cursor and provides virtual, unlimited cursor movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //Check what shader version is supported
    printf("Support shader language: version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    //Make the window's context current
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    
    //Make sure GLEW was initialized properly
    if (glewInit() != GLEW_OK) {
        cerr << "GLEW failed to initialize" << endl;
        glfwTerminate();
        return -1;
    }

    //How to use relative paths:
    //1. In Xcode, navigate to Product -> Scheme -> Edit Scheme
    //2. Select the Run tab from the table view on the left side of the window
    //3. Under the Options tab, change the "Working Directory" to this project's directory
    GLuint program = loadShaders("basic.vert", "basic.frag");
    
    //Generate a vertex array
    GLuint vaoID; //An ID in OpenGL
    glGenVertexArrays(1, &vaoID); //Creates a vertex array; takes a reference to the ID we defined above, because internally, OpenGL is going to edit this ID and return it
    glBindVertexArray(vaoID); //Tells OpenGL we are going to use /modify this array now
    
#ifdef CUBE
    //A 6-sided cube (12 triangles)
    static const GLfloat verts[] = {
        // X, Y, Z: the bottom left of the window is (-1,-1), the top right is (1,1), and the center is (0,0)
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        
        1.0f,-1.0f, 1.0f,////
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    
    //Two UV coordinatesfor each vertex, created with Blender
    static const GLfloat uvs[] = {
        0.000059f, 1.0f-0.000004f,
        0.000103f, 1.0f-0.336048f,
        0.335973f, 1.0f-0.335903f,
        1.000023f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.336024f, 1.0f-0.671877f,
        0.667969f, 1.0f-0.671889f,
        1.000023f, 1.0f-0.000013f,
        0.668104f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.000059f, 1.0f-0.000004f,
        0.335973f, 1.0f-0.335903f,
        0.336098f, 1.0f-0.000071f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.336024f, 1.0f-0.671877f,
        1.000004f, 1.0f-0.671847f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.668104f, 1.0f-0.000013f,
        0.335973f, 1.0f-0.335903f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.668104f, 1.0f-0.000013f,
        0.336098f, 1.0f-0.000071f,
        0.000103f, 1.0f-0.336048f,
        0.000004f, 1.0f-0.671870f,
        0.336024f, 1.0f-0.671877f,
        0.000103f, 1.0f-0.336048f,
        0.336024f, 1.0f-0.671877f,
        0.335973f, 1.0f-0.335903f,
        0.667969f, 1.0f-0.671889f,
        1.000004f, 1.0f-0.671847f,
        0.667979f, 1.0f-0.335851f
    };
#else 
    //A single triangle
    static const GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    }
#endif
    
    //Generate a VBO
    GLuint vboID; //An ID in OpenGL
    glGenBuffers(1, &vboID); //Create a vertex buffer object with the ID we defined above
    glBindBuffer(GL_ARRAY_BUFFER, vboID); //Tells OpenGL we are going to use / modify this buffer now
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); //Fill the buffer with data
    
    //Generate a buffer for storing UV coordinates
    GLuint uvID;
    glGenBuffers(1, &uvID);
    glBindBuffer(GL_ARRAY_BUFFER, uvID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    
    //Load the texture
    GLuint tex = loadBmp("uvtemplate.bmp");
    
    //Get a handle for the "samp" uniform
    GLuint texID = glGetUniformLocation(program, "samp");
    
    
    
    
    /*
     *
     * Global GL settings
     *
     */
    
    //Set the background color of our application
    glClearColor(0.35f, 0.35f, 0.35f, 1.0f);
    
    //Stores the depth ("z" value) of each fragment in a buffer so that each time you want to write a fragment, we first check to see if we should (i.e. it is closer than any previous fragment)
    glEnable(GL_DEPTH_TEST);
    
    //Accept a fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    
    
    
    /*
     *
     * Transformation matrices
     *
     */

    controls controls(window);

    //OLDER DEFAULT VIEW
//    //Projection matrix: 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
//    //View (camera) matrix
//    glm::mat4 View = glm::lookAt(
//                                 glm::vec3(4,3,3), //Camera is at (4,3,3) in World Space
//                                 glm::vec3(0,0,0), //Looking at the origin
//                                 glm::vec3(0,1,0)  //Head is up (set to 0, -1, 0 to look upside-down)
//                                );
//    
//    //Model matrix: an identity matrix (model will be at the origin)
//    glm::mat4 Model = glm::mat4(1.0f);
//    
//    //The ModelViewProjection matrix is the combination of the previous 3 matrices
//    glm::mat4 ModelViewProjection = Projection * View * Model;
    
#ifdef DRAW_WIREFRAME
    //Draw the wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    
    //Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        controls.computeMatricesFromInputs();
        glm::mat4 Projection = controls.getProjectionMatrix();
        glm::mat4 View = controls.getViewMatrix();
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 ModelViewProjection = Projection * View * Model;
        /*
         *
         * All rendering happens below
         *
         */
        
        //First, clear the background color AND the depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Bind the VBO - not necessary to re-copy here
        //The subsequent calls to glVertexAttribPointer will reference this buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        
        //Define attributes 0 and 1 (3 is the size of each vertex, GL_FLOAT is the type, GL_FALSE disables any normalization, and the last two zeros mean there's no stride or offset on the vertices)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        //Turn on automatic vertex fetching for the attributes at position 0 (this will be the vertex position) and 1 (this will be the vertex color)
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, uvID);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(3);
        
        //Use the shaders we've loaded above
        glUseProgram(program);
        
        //Uniforms need to be set AFTER the call to glUseProgram
        GLuint MatrixID = glGetUniformLocation(program, "ModelViewProjection");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &ModelViewProjection[0][0]);
    
        //Bind the texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(texID, 0);
        
        //Draw the vertices
#ifdef CUBE
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
#else
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
        
        //Disable the attributes at position 0 and 1 (the vertex color and position)
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        //Swap front and back buffers
        glfwSwapBuffers(window);
        
        //Poll for and process events
        glfwPollEvents();
    }
    
    //Cleanup VBO and shader
    glDeleteBuffers(1, &vboID);
    glDeleteBuffers(1, &uvID);
    glDeleteProgram(program);
    glDeleteTextures(1, &texID);
    glDeleteVertexArrays(1, &vaoID);
    
    //Close the OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}
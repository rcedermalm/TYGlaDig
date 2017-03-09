//
// Created by Linnea Bergman on 2017-03-03.
//

#include <iostream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#ifndef TYGLADIG_TRIANGLESOUP_H
#define TYGLADIG_TRIANGLESOUP_H


class TriangleSoup {
private:
    GLuint VAO; // Vertex array object
    int nVerts; // number of vertices in vertex array
    int nTris; // number of triangles in index array
    GLuint vertexBuffer; // vertex array buffer id to bind to GL_BIND_BUFFER
    GLuint indexBuffer; // intex buffer id to bind to GL_ELEMENT_ARRAY_BUFFER
    float *vertexArray;
    GLuint *indexArray;

    void printError(const char *errType, const char *errMsg);
public:
    // Constructor: initialise a TriangleSoup object to all zeros
    TriangleSoup();

    // Destructor: clean up allocated data in a TriangleSoup object
    ~TriangleSoup();

    // Clean up allocated data in a TriangleSoup object
    void Clean();

    // Create a sphere
    void createSphere(float radius, int segments);

    // Print data from TriangleSoup object
    void print();

    // Print information about a TriangleSoup object
    void printInfo();

    // Render geometry in a TriangleSoup object
    void render();
};


#endif //TYGLADIG_TRIANGLESOUP_H

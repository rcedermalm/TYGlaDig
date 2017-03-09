//
// Created by Linnea Bergman on 2017-03-03.
//

#include "TriangleSoup.h"

// Constructor: Initialise a TriangleSoup object to all zeros
TriangleSoup::TriangleSoup()
 : VAO(0), nVerts(0), nTris(0), vertexBuffer(0), indexBuffer(0) {
    vertexArray = NULL;
    indexArray = NULL;
}

// Destructor: Clean up allocated data in a TriangleSoup object
TriangleSoup::~TriangleSoup() {
    Clean();
}

// Clean up allocated data in a TriangleSoup object
void TriangleSoup::Clean() {
    if(glIsVertexArray(VAO)) {
        glDeleteVertexArrays(1, &VAO);
    }
    VAO = 0;

    if(glIsBuffer(vertexBuffer)) {
        glDeleteBuffers(1, &vertexBuffer);
    }
    vertexBuffer = 0;

    if(glIsBuffer(indexBuffer)) {
        glDeleteBuffers(1, &indexBuffer);
    }
    indexBuffer = 0;

    if(vertexArray) {
        delete[] vertexArray;
    }

    if(indexArray) {
        delete[] indexArray;
    }

    nVerts = 0;
    nTris = 0;
}

// Create a sphere
/*
 * createSphere(float radius, int segments)
 *
 * Create a TriangleSoup objectwith vertex and index arrays
 * to draw a textured sphere with normals.
 * Increasing the parameter 'segments' yields more triangles.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The arrays are allocated by malloc() inside the
 * function and should be disposed of using free() when they are no longer
 * needed, e.g with the function soupDelete().
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 */
void TriangleSoup::createSphere(float radius, int segments) {
    int i, j, base, i0;
    float x, y, z, R;
    double theta, phi;
    int vSegs, hSegs;
    int stride = 8;

    // Delete any previous content in the TriangleSoup object
    Clean();

    vSegs = segments;
    // Minimum amount of segments
    if(vSegs < 2) {
        vSegs = 2;
    }
    hSegs = vSegs * 2;
    nVerts = 1 + (vSegs-1) * (hSegs+1) + 1; // Top + Middle + Bottom
    nTris = hSegs + (vSegs-2) * (hSegs*2) + hSegs; // Top + Middle + Bottom
    vertexArray = new float[nVerts * 8];
    indexArray = new GLuint[nTris * 3];

    // The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
    // First vertex: top pole (+z is "up" in object local coords)
    vertexArray[0] = 0.0f;
    vertexArray[1] = 0.0f;
    vertexArray[2] = radius;
    vertexArray[3] = 0.0f;
    vertexArray[4] = 0.0f;
    vertexArray[5] = 1.0f;
    vertexArray[6] = 0.5f;
    vertexArray[7] = 1.0f;

    // Last vertex: Bottom pole
    base = (nVerts-1) * stride;
    vertexArray[base+0] = 0.0f;
    vertexArray[base+1] = 0.0f;
    vertexArray[base+2] = -radius;
    vertexArray[base+3] = 0.0f;
    vertexArray[base+4] = 0.0f;
    vertexArray[base+5] = -1.0f;
    vertexArray[base+6] = 0.5f;
    vertexArray[base+7] = 0.0f;

    // All the other vertices
    for(j = 0; j < vSegs -1; j++) {
        theta = (double)(j+1)/vSegs*M_PI;
        z = cos(theta);
        R = sin(theta);
        for(i = 0; i <= hSegs; i++) {
            phi = (double)i/hSegs*2.0*M_PI;
            x = R * cos(phi);
            y = R * sin(phi);
            base = (1+j*(hSegs+1)+i) * stride;
            vertexArray[base] = radius*x;
            vertexArray[base+1] = radius*y;
            vertexArray[base+2] = radius*z;
            vertexArray[base+3] = x;
            vertexArray[base+4] = y;
            vertexArray[base+5] = z;
            vertexArray[base+6] = (float)i/hSegs;
            vertexArray[base+7] = 1.0f-(float)(j+1)/vSegs;
        }
    }

    // The index array: triplets of integers, one for each triangle
    // Top cap
    for(i=0; i<hSegs; i++) {
        indexArray[3*i] = 0;
        indexArray[3*i+1] = 1+i;
        indexArray[3*i+2] = 2+i;
    }

    // Middle part (possibly empty if vSegs=2)
    for(j=0; j<vSegs-2; j++) {
        for(i=0; i<hSegs; i++) {
            base = 3*(hSegs + 2*(j*hSegs + i));
            i0 = 1 + j*(hSegs+1) + i;
            indexArray[base] = i0;
            indexArray[base+1] = i0+hSegs+1;
            indexArray[base+2] = i0+1;
            indexArray[base+3] = i0+1;
            indexArray[base+4] = i0+hSegs+1;
            indexArray[base+5] = i0+hSegs+2;
        }
    }

    // Bottom cap
    base = 3*(hSegs + 2*(vSegs-2)*hSegs);
    for(i=0; i<hSegs; i++) {
        indexArray[base+3*i] = nVerts-1;
        indexArray[base+3*i+1] = nVerts-2-i;
        indexArray[base+3*i+2] = nVerts-3-i;
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &(VAO));
    glBindVertexArray(VAO);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Present our vertex coordinates to OpenGL
    glBufferData(GL_ARRAY_BUFFER, 8*nVerts * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0); // Vertex coordinates
    glEnableVertexAttribArray(1); // Normals
    glEnableVertexAttribArray(2); // Texture coordinates

    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3, 6 (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)0);                   // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat))); // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat))); // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*nTris*sizeof(GLuint), indexArray, GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the buffers while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* Print data from a TriangleSoup object, for debugging purposes */
void TriangleSoup::print() {
    int i;

    printf("TriangleSoup vertex data:\n\n");
    for(i = 0; i < nVerts; i++) {
        printf("%d: %8.2f %8.2f %8.2f\n", i, vertexArray[8*i], vertexArray[8*i+1], vertexArray[8*i+2]);
    }
    printf("\nTriangleSoup face index data:\n\n");
    for(i = 0; i < nTris; i++) {
        printf("%d: %d %d %d\n", i, indexArray[3*i], indexArray[3*i+1], indexArray[3*i+2]);
    }
}

/* Print information about a TriangleSoup object (stats and extents) */
void TriangleSoup::printInfo() {
    int i;
    float x, y, z, xmin, xmax, ymin, ymax, zmin, zmax;

    printf("TriangleSoup information:\n");
    printf("vertices : %d\n", nVerts);
    printf("triangles: %d\n", nTris);
    xmin = xmax = vertexArray[0];
    ymin = ymax = vertexArray[1];
    zmin = zmax = vertexArray[2];
    for(i = 1; i < nVerts; i++) {
        x = vertexArray[8*i];
        y = vertexArray[8*i+1];
        z = vertexArray[8*i+2];
        printf("x y z : %8.2f %8.2f %8.2f\n", x, y, z);
        if(x<xmin) xmin = x;
        if(x>xmax) xmax = x;
        if(y<ymin) ymin = y;
        if(y>ymax) ymax = y;
        if(z<zmin) zmin = z;
        if(z>zmax) zmax = z;
    }

    printf("xmin: %8.2f\n", xmin);
    printf("xmax: %8.2f\n", xmax);
    printf("ymin: %8.2f\n", ymin);
    printf("ymax: %8.2f\n", ymax);
    printf("zmin: %8.2f\n", zmin);
    printf("zmax: %8.2f\n", zmax);
}

/* Render the geometry in a TriangleSoup object */
void TriangleSoup::render() {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3 * nTris, GL_UNSIGNED_INT, (void*)0);
    // (mode, vertex count, type, element array buffer offset)
    glBindVertexArray(0);

}

/*
 * private
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void TriangleSoup::printError(const char *errType, const char *errMsg) {
    fprintf(stderr, "%s: %s\n", errType, errMsg);
}
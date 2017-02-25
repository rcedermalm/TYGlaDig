#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// SOIL
//#define SOIL_STATIC
//#include <SOIL.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Classes
#include "ShaderProgram.hpp"
#include "Camera.h"
#include "Particle.h"

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

/*******************************************
 ****** FUNCTION/VARIABLE DECLARATIONS *****
 *******************************************/

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

// Time variables
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Camera variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Stores key information if the key is pressed or not
bool keys[1024];

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

glm::vec3 theSpringForce(Particle p1, Particle p2, float L0, float k);
glm::vec3 theDampForce(Particle p1, Particle p2, float b);

bool calcvelocity = false;
glm::vec3 RungeKuttaForVel(Particle p, float h);
glm::vec3 RungeKuttaForPosDiff(Particle p, float h);


/*******************************************
 **************    MAIN     ****************
 *******************************************/

// The MAIN function, from here we start the application and run the rendering loop
int main()
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    if(!glfwInit()) {
        std::cout << "Failed to initialise GLFW" << std::endl;
        return 1;
    }

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);

    if (window == nullptr){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLFW Options
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    int width, height;

    /*************** Callback functions ****************/
    // Decides what should be done when a key is pressed
    glfwSetKeyCallback(window, key_callback);
    // Decides what should be done when the mouse is in use
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /************** Declare variables **************/
    GLfloat k = 1.0f;
    GLfloat b = 0.1f;
    GLfloat L0 = 0.2f;
    GLfloat L0cross = sqrtf((float)pow(L0,2)*2);
    GLfloat m = 1.0f;
    GLfloat h = 0.005f;

    //To keep track of the differences between positions (for the loop)

    Particle theParticles[] = {
            Particle(m, glm::vec3(0.0f, 0.0f, 0.0f)), // Particle 0
            Particle(m, glm::vec3(  L0, 0.0f, 0.0f)), // Particle 1
            Particle(m, glm::vec3(  L0,   L0, 0.0f)), // Particle 2
            Particle(m, glm::vec3(0.0f,   L0, 0.0f)), // Particle 3
            Particle(m, glm::vec3( -L0,   L0, 0.0f)), // Particle 4
            Particle(m, glm::vec3( -L0, 0.0f, 0.0f)), // Particle 5
            Particle(m, glm::vec3( -L0,  -L0, 0.0f)), // Particle 6
            Particle(m, glm::vec3(0.0f,  -L0, 0.0f)), // Particle 7
            Particle(m, glm::vec3(  L0+0.15f,  -L0-0.15f, 0.0f))  // Particle 8
    };

    GLuint nrOfParticles = ARRAY_SIZE(theParticles);
    glm::vec3 newCubePositions[nrOfParticles];
   /* Particle new1 = Particle(m, glm::vec3(  -0.2f, 0.0f, 0.0f) );
    Particle new2 = Particle(m, glm::vec3(   L0, 0.0f, 0.0f) );
    Particle new3 = Particle(m, glm::vec3(   L0, L0,   0.0f) );
    Particle new4 = Particle(m, glm::vec3( 0.0f, L0,   0.0f) );
*/
    //CUBE
    GLfloat vertices[] = {
            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,

            -0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,

            -0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,

             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,

            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f, -0.01f, -0.01f,  1.0f, 1.0f, 1.0f,

            -0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
             0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f,  0.01f,  1.0f, 1.0f, 1.0f,
            -0.01f,  0.01f, -0.01f,  1.0f, 1.0f, 1.0f,
    };
/*
    glm::vec3 cubePositions[] = {
            new1.getPos(),
            new2.getPos(),
            new3.getPos(),
            new4.getPos()
    };
*/
    glm::vec3 cubePositions[] = {
            theParticles[0].getPos(),
            theParticles[1].getPos(),
            theParticles[2].getPos(),
            theParticles[3].getPos(),
            theParticles[4].getPos(),
            theParticles[5].getPos(),
            theParticles[6].getPos(),
            theParticles[7].getPos(),
            theParticles[8].getPos()
    };

    /***************** Vertex Buffer Objects (VBO) ******************/
    GLuint VBO;
    glGenBuffers(1, &VBO); // Create Buffer ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind a buffer to the ID
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies the vertices data into the buffer

    /***************** Vertex Array Objects (VAO) ******************/
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); //Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    /***************** Shaders ********************/
    // Build and compile our shader program
    std::string vertexFilename = "../shaders/vertexShader.vert";
    std::string fragmentFilename = "../shaders/fragmentShader.frag";
    ShaderProgram theShaders(vertexFilename, "", "", "", fragmentFilename);
    theShaders();

    /**************** Uniform variables **********************/
    GLint modelLoc = glGetUniformLocation(theShaders, "model");


    /******************* RENDER LOOP ********************/
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        // OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        /**************** RENDER STUFF ****************/
        // Structural springs and dampers
        glm::vec3 Fk01 = theSpringForce(theParticles[0], theParticles[1], L0, k);
        glm::vec3 Fb01 = theDampForce(theParticles[0], theParticles[1], b);
        glm::vec3 Fk12 = theSpringForce(theParticles[1], theParticles[2], L0, k);
        glm::vec3 Fb12 = theDampForce(theParticles[1], theParticles[2], b);
        glm::vec3 Fk32 = theSpringForce(theParticles[3], theParticles[2], L0, k);
        glm::vec3 Fb32 = theDampForce(theParticles[3], theParticles[2], b);
        glm::vec3 Fk03 = theSpringForce(theParticles[0], theParticles[3], L0, k);
        glm::vec3 Fb03 = theDampForce(theParticles[0], theParticles[3], b);
        glm::vec3 Fk43 = theSpringForce(theParticles[4], theParticles[3], L0, k);
        glm::vec3 Fb43 = theDampForce(theParticles[4], theParticles[3], b);
        glm::vec3 Fk54 = theSpringForce(theParticles[5], theParticles[4], L0, k);
        glm::vec3 Fb54 = theDampForce(theParticles[5], theParticles[4], b);
        glm::vec3 Fk50 = theSpringForce(theParticles[5], theParticles[0], L0, k);
        glm::vec3 Fb50 = theDampForce(theParticles[5], theParticles[0], b);
        glm::vec3 Fk65 = theSpringForce(theParticles[6], theParticles[5], L0, k);
        glm::vec3 Fb65 = theDampForce(theParticles[6], theParticles[5], b);
        glm::vec3 Fk67 = theSpringForce(theParticles[6], theParticles[7], L0, k);
        glm::vec3 Fb67 = theDampForce(theParticles[6], theParticles[7], b);
        glm::vec3 Fk70 = theSpringForce(theParticles[7], theParticles[0], L0, k);
        glm::vec3 Fb70 = theDampForce(theParticles[7], theParticles[0], b);
        glm::vec3 Fk78 = theSpringForce(theParticles[7], theParticles[8], L0, k);
        glm::vec3 Fb78 = theDampForce(theParticles[7], theParticles[8], b);
        glm::vec3 Fk81 = theSpringForce(theParticles[8], theParticles[1], L0, k);
        glm::vec3 Fb81 = theDampForce(theParticles[8], theParticles[1], b);

        // Shear springs and dampers
        glm::vec3 Fk02 = theSpringForce(theParticles[0], theParticles[2], L0cross, k);
        glm::vec3 Fb02 = theDampForce(theParticles[0], theParticles[2], b);
        glm::vec3 Fk13 = theSpringForce(theParticles[1], theParticles[3], L0cross, k);
        glm::vec3 Fb13 = theDampForce(theParticles[1], theParticles[3], b);
        glm::vec3 Fk53 = theSpringForce(theParticles[5], theParticles[3], L0cross, k);
        glm::vec3 Fb53 = theDampForce(theParticles[5], theParticles[3], b);
        glm::vec3 Fk04 = theSpringForce(theParticles[0], theParticles[4], L0cross, k);
        glm::vec3 Fb04 = theDampForce(theParticles[0], theParticles[4], b);
        glm::vec3 Fk60 = theSpringForce(theParticles[6], theParticles[0], L0cross, k);
        glm::vec3 Fb60 = theDampForce(theParticles[6], theParticles[0], b);
        glm::vec3 Fk75 = theSpringForce(theParticles[7], theParticles[5], L0cross, k);
        glm::vec3 Fb75 = theDampForce(theParticles[7], theParticles[5], b);
        glm::vec3 Fk71 = theSpringForce(theParticles[7], theParticles[1], L0cross, k);
        glm::vec3 Fb71 = theDampForce(theParticles[7], theParticles[1], b);
        glm::vec3 Fk80 = theSpringForce(theParticles[8], theParticles[0], L0cross, k);
        glm::vec3 Fb80 = theDampForce(theParticles[8], theParticles[0], b);

        // Bend springs and dampers
        glm::vec3 Fk42 = theSpringForce(theParticles[4], theParticles[2], 2.0f*L0, k);
        glm::vec3 Fb42 = theDampForce(theParticles[4], theParticles[2], b);
        glm::vec3 Fk51 = theSpringForce(theParticles[5], theParticles[1], 2.0f*L0, k);
        glm::vec3 Fb51 = theDampForce(theParticles[5], theParticles[1], b);
        glm::vec3 Fk68 = theSpringForce(theParticles[6], theParticles[8], 2.0f*L0, k);
        glm::vec3 Fb68 = theDampForce(theParticles[6], theParticles[8], b);
        glm::vec3 Fk64 = theSpringForce(theParticles[6], theParticles[4], 2.0f*L0, k);
        glm::vec3 Fb64 = theDampForce(theParticles[6], theParticles[4], b);
        glm::vec3 Fk73 = theSpringForce(theParticles[7], theParticles[3], 2.0f*L0, k);
        glm::vec3 Fb73 = theDampForce(theParticles[7], theParticles[3], b);
        glm::vec3 Fk82 = theSpringForce(theParticles[8], theParticles[2], 2.0f*L0, k);
        glm::vec3 Fb82 = theDampForce(theParticles[8], theParticles[2], b);

        // Calculate and set the acceleration of the particles
        theParticles[0].setAcc((1/m)*(+Fk03-Fb03+Fk01-Fb01-Fk50+Fb50-Fk70+Fb70+Fk13-Fb13+Fk04-Fb04-Fk60+Fb60-Fk80+Fb80));
        theParticles[1].setAcc((1/m)*(-Fk01+Fb01+Fk12-Fb12-Fk81+Fb81+Fk13-Fb13-Fk71+Fb71-Fk51+Fb51));
        //theParticles[2].setAcc((1/m)*(-Fk12+Fb12-Fk32+Fb32-Fk13+Fb13-Fk42+Fb42-Fk82+Fb82));
        theParticles[3].setAcc((1/m)*(+Fk32-Fb32-Fk03+Fb03-Fk43+Fb43-Fk13+Fb13-Fk53+Fb53-Fk73+Fb73));
        //theParticles[4].setAcc((1/m)*(+Fk43-Fb43-Fk54+Fb54-Fk04+Fb04+Fk42-Fb42-Fk64+Fb64));
        theParticles[5].setAcc((1/m)*(+Fk50-Fb50+Fk54-Fb54-Fk65+Fb65+Fk53-Fb53-Fk75+Fb75+Fk51-Fb51));
        theParticles[6].setAcc((1/m)*(+Fk67-Fb67+Fk65-Fb65+Fk60-Fb60+Fk68-Fb68+Fk64-Fb64));
        theParticles[7].setAcc((1/m)*(+Fk78-Fb78+Fk70-Fb70-Fk67+Fb67+Fk71-Fb71+Fk75-Fb75+Fk73-Fb73));
        theParticles[8].setAcc((1/m)*(+Fk81-Fb81-Fk78+Fb78+Fk80-Fb80-Fk68+Fb68+Fk82-Fb82));
        theParticles[2].setAcc(glm::vec3(0.0f,0.0f,0.0f));
        theParticles[4].setAcc(glm::vec3(0.0f,0.0f,0.0f));

        /*// Calculate the spring and the damp forces
        glm::vec3 Fk1 = theSpringForce(new1, new2, L0, k);
        glm::vec3 Fb1 = theDampForce(new1, new2, b);
        glm::vec3 Fk2 = theSpringForce(new2, new3, L0, k);
        glm::vec3 Fb2 = theDampForce(new2, new3, b);
        glm::vec3 Fk3 = theSpringForce(new3, new4, L0, k);
        glm::vec3 Fb3 = theDampForce(new3, new4, b);
        glm::vec3 Fk4 = theSpringForce(new4, new1, L0, k);
        glm::vec3 Fb4 = theDampForce(new4, new1, b);

        glm::vec3 Fk13 = theSpringForce(new1, new3, L0cross, k);
        glm::vec3 Fb13 = theDampForce(new1, new3, b);
        glm::vec3 Fk24 = theSpringForce(new2, new4, L0cross, k);
        glm::vec3 Fb24 = theDampForce(new2, new4, b);

        // Calculate and set the acceleration of the particles
        new1.setAcc((1/m)*(-Fk4+Fb4+Fk1-Fb1+Fk13-Fb13));
        new2.setAcc((1/m)*(-Fk1+Fb1+Fk2-Fb2+Fk24-Fb24));
        new3.setAcc((1/m)*(-Fk2+Fb2+Fk3-Fb3-Fk13+Fb13));
        new4.setAcc((1/m)*(-Fk3+Fb3+Fk4-Fb4-Fk24+Fb24));
        */
        /*for(GLuint i = 0; i < nrOfParticles; i++){
            theParticles[i].setPos(RungeKuttaForPosDiff(theParticles[i],h)+theParticles[i].getPos());
            newCubePositions[i] += RungeKuttaForPosDiff(theParticles[i],h);
            theParticles[i].setVel(RungeKuttaForVel(theParticles[i],h));
        }*/

/*        // Calculate and set the positions of the particles
        new1.setPos(RungeKuttaForPosDiff(new1,h) + new1.getPos());
        new2.setPos(RungeKuttaForPosDiff(new2,h) + new2.getPos());
        new3.setPos(RungeKuttaForPosDiff(new3,h) + new3.getPos());
        new4.setPos(RungeKuttaForPosDiff(new4,h) + new4.getPos());

        // Update the sum of the differences
        newCubePositions[0] += RungeKuttaForPosDiff(new1, h);
        newCubePositions[1] += RungeKuttaForPosDiff(new2, h);
        newCubePositions[2] += RungeKuttaForPosDiff(new3, h);
        newCubePositions[3] += RungeKuttaForPosDiff(new4, h);

        // Calculate and set the velocities of the particles
        new1.setVel(RungeKuttaForVel(new1, h));
        new2.setVel(RungeKuttaForVel(new2, h));
        new3.setVel(RungeKuttaForVel(new3, h));
        new4.setVel(RungeKuttaForVel(new4, h));
*/
        //Draw cubes
        glBindVertexArray(VAO);  // Bind VAO
        for(GLuint i = 0; i < nrOfParticles; i++)
        {
            theParticles[i].setPos(RungeKuttaForPosDiff(theParticles[i],h)+theParticles[i].getPos());
            newCubePositions[i] += RungeKuttaForPosDiff(theParticles[i],h);
            theParticles[i].setVel(RungeKuttaForVel(theParticles[i],h));

            //Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            glm::vec3 theTransVec = (newCubePositions[i]);
            model = glm::translate(model, theTransVec);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0); // Unbind VAO

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}


/*******************************************
 ******** Function initialisation **********
 *******************************************/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (GLfloat)xpos;
        lastY = (GLfloat)ypos;
        firstMouse = false;
    }

    GLfloat xoffset = (GLfloat)(xpos - lastX);
    GLfloat yoffset = (GLfloat)(lastY - ypos); // Reversed since y-coordinates go from bottom to left
    lastX = (GLfloat)xpos;
    lastY = (GLfloat)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void do_movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD,deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD,deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT,deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT,deltaTime);
}

glm::vec3 theSpringForce(Particle p1, Particle p2, GLfloat L0, GLfloat k){
    GLfloat lengthBParticles = sqrtf(GLfloat(pow(p2.getPos().x-p1.getPos().x,2) + pow(p2.getPos().y-p1.getPos().y,2) + pow(p2.getPos().z-p1.getPos().z,2)));
    glm::vec3 length = p2.getPos() - p1.getPos();
    glm::vec3 Fk = k*(lengthBParticles-L0)*(normalize(length));
    return Fk;
}

glm::vec3 theDampForce(Particle p1, Particle p2, GLfloat b){
    glm::vec3 Fb = b * (p1.getVel() - p2.getVel());
    return Fb;
}

glm::vec3 RungeKuttaForVel(Particle p, GLfloat h){

    glm::vec3 next, k1, k2, k3, k4;

    k1 = p.getAcc();
    k2 = p.getAcc() + (h/2.0f)*k1;
    k3 = p.getAcc() + (h/2.0f)*k2;
    k4 = p.getAcc() + h*k3;
    next = p.getVel() + (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);

    return next;
}

glm::vec3 RungeKuttaForPosDiff(Particle p, GLfloat h){
    glm::vec3 next, k1, k2, k3, k4;
    k1 = p.getVel();
    k2 = p.getVel() + (h/2.0f)*k1;
    k3 = p.getVel() + (h/2.0f)*k2;
    k4 = p.getVel() + h*k3;
    next = (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);
    return next;
}


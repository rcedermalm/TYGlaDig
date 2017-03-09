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
#include <TriangleSoup.h>

// Classes
#include "ShaderProgram.hpp"
#include "Camera.h"
#include "Particle.h"

/*******************************************
 ******     STRUCT DECLARATIONS        *****
 *******************************************/

// Struct for the ball
/*
typedef struct {
    int X;
    int Y;
    int Z;
}vertices;
*/

/*******************************************
 ******     VARIABLE DECLARATIONS      *****
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

// variables for the ball
/*
const int spaceVertices = 10;
const int vertexAmount = (90/spaceVertices)*(360/spaceVertices)*4;

vertices vertex[vertexAmount];
 */

/*******************************************
 ******     FUNCTION DECLARATIONS      *****
 *******************************************/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

glm::vec3 theSpringForce(Particle p1, Particle p2, float L0, float k);
glm::vec3 theDampForce(Particle p1, Particle p2, float b);
glm::vec3 RungeKuttaForVel(Particle p, float h);
glm::vec3 RungeKuttaForPosDiff(Particle p, float h);

// Functions for the ball
/*
glm::vec3 normalise(glm::vec3 vector);
void ballCollision(Particle particle, const glm::vec3 centre, const GLfloat radius);
void createBall(float nrSubdivisions, float transX, float transY, float transZ);
void displayBall(float radius);
 */

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    GLfloat k = 1.0f; // spring constant
    GLfloat b = 0.1f; // damping constant
    GLfloat L0 = 0.1f; // rest length of the structural springs
    GLfloat L0cross = sqrtf((float)pow(L0,2)*2); // rest length of the shear springs
    GLfloat m = 1.0f; // mass of the particles
    GLfloat h = 0.007f; // length of step for RK4 calculations

    // Ball
    /*
    float ballCentreX = 0.5f;
    float ballCentreY = -0.5f;
    glm::vec3 ballCentrePos(ballCentreX, ballCentreY, 0.0f);  // Center of ball
    float ballRadius = 0.5f;   // Radius of ball
    float ballTime = 0.0f; // Counter used to calculate the z position of the ball
     */
    TriangleSoup ball;
    float ballRadius = 0.1f;
    int ballSegments = 8;

    // Create all the particles and put them in a grid
    // Always use an odd number
    const GLuint clothWidth = 9, clothHeight = 9;
    Particle theParticles[clothHeight][clothWidth];

    float heightCounter = 5.0f;
    for(GLuint i = 0; i < clothHeight; i++ ){
        float widthCounter = -5.0f;
        for(GLuint j = 0; j < clothWidth; j++){
            theParticles[i][j] = Particle(m, glm::vec3(widthCounter*L0, 0.0f, heightCounter*L0));
            if(i == 0 & j == 0 || i == 0 & j == (clothHeight - 1) ) {
                theParticles[i][j].makeStationary();
            }
            widthCounter++;
        }
        heightCounter--;
    }

    //Just to move the particle in the right-bottom corner a bit more to the right and down so that the fabric moves
    //theParticles[clothHeight-1][clothWidth-1].setPos(theParticles[clothHeight-1][clothWidth-1].getPos() + glm::vec3(0.1f, -0.17f, 0.0f));

    // Create a grid with the positions in the beginning and initialize a grid for
    // calculating the new positions (calculations for that one are made in the render loop)
    glm::vec3 cubePositions[clothHeight][clothWidth];
    glm::vec3 newCubePositions[clothHeight][clothWidth];
    for(GLuint i = 0; i < clothHeight; i++){
        for(GLuint j = 0; j < clothWidth; j++){
            cubePositions[i][j] = theParticles[i][j].getPos();
            newCubePositions[i][j] = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    // Create a cube
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

    // Create ball
    ball.createSphere(ballRadius, ballSegments);
    ball.printInfo();

    /***************** Vertex Buffer Objects (VBO) ******************/
    GLuint VBO;
    glGenBuffers(1, &VBO); // Create Buffer ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind a buffer to the ID
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies the vertices data into the buffer

    /***************** Vertex Array Objects (VAO) ******************/
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    /***************** Shaders ********************/
    // Build and compile the shader program
    std::string vertexFilename = "../shaders/vertexShader.vert";
    std::string fragmentFilename = "../shaders/fragmentShader.frag";
    ShaderProgram theShaders(vertexFilename, "", "", "", fragmentFilename);
    theShaders();

    /**************** Uniform variables **********************/
    GLint modelLoc = glGetUniformLocation(theShaders, "model");
    GLint viewLoc = glGetUniformLocation(theShaders, "view");
    GLint projLoc = glGetUniformLocation(theShaders, "projection");

    /****************************************************/
    /******************* RENDER LOOP ********************/
    /****************************************************/

    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltaTime of current frame
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        // OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Create camera transformation
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera.Zoom, (float)WIDTH/(float)HEIGHT, 0.1f, 1000.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // The state of the left mouse button (checks later if it's pressed or not
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        /**************** RENDER STUFF ****************/

        glm::vec3 gravity = glm::vec3(0.0f, -0.00098f*2, 0.0f);
        //ballTime++;
        //ballCentrePos.z = cos(ballTime/50.0)*7; // Update z position of ball

        // Calculate the forces acting on the particles
        for(GLuint i = 0; i < clothHeight; i++){
            for(GLuint j = 0; j < clothWidth; j++){

                /** Kanske implementera att koden innanför for-loopen körs på GPU:n så att allt går fortare, kolla upp
                 * OpenCL. Kan dock bli överkurs, möjligt att det bara räcker att sitta på en stationär dator.
                 */

                glm::vec3 theForce = glm::vec3(0.0f, 0.0f, 0.0f);

                // Bend springs and dampers
                if(j<clothWidth-2){ theForce += theSpringForce(theParticles[i][j], theParticles[i][j+2], (2.0f)*L0, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j], theParticles[i][j+2], b);}
                if(i>2){ theForce += theSpringForce(theParticles[i][j], theParticles[i-2][j], (2.0f)*L0, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j], theParticles[i-2][j], b);}
                if(j>2){ theForce += (-1.0f) * theSpringForce(theParticles[i][j-2], theParticles[i][j], (2.0f)*L0, k);
                    theForce += theDampForce(theParticles[i][j-2], theParticles[i][j], b);}
                if(i<clothHeight-2){ theForce += (-1.0f) * theSpringForce(theParticles[i+2][j], theParticles[i][j], (2.0f)*L0, k);
                    theForce += theDampForce(theParticles[i+2][j], theParticles[i][j], b);}

                // Shear springs and dampers
                if(i!=0 && j!=clothWidth-1){ theForce += theSpringForce(theParticles[i][j], theParticles[i-1][j+1], L0cross, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j], theParticles[i-1][j+1], b);}
                if(i!=0 && j!=0){ theForce += theSpringForce(theParticles[i][j], theParticles[i-1][j-1], L0cross, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j], theParticles[i-1][j-1], b);}
                if(i!= clothHeight-1 && j!= 0){ theForce += (-1.0f) * theSpringForce(theParticles[i+1][j-1], theParticles[i][j], L0cross, k);
                    theForce += theDampForce(theParticles[i+1][j-1], theParticles[i][j], b);}
                if(i!= clothHeight-1 && j!=clothWidth-1){ theForce += (-1.0f) * theSpringForce(theParticles[i+1][j+1], theParticles[i][j], L0cross, k);
                    theForce += theDampForce(theParticles[i+1][j+1], theParticles[i][j], b);}


                // Structural springs and dampers
                if(j!=clothWidth-1){theForce += theSpringForce(theParticles[i][j], theParticles[i][j+1], L0, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j],theParticles[i][j+1], b);}
                if(i!=0){ theForce += theSpringForce(theParticles[i][j], theParticles[i-1][j], L0, k);
                    theForce += (-1.0f) * theDampForce(theParticles[i][j], theParticles[i-1][j], b);}
                if(j!=0){ theForce += (-1.0f) * theSpringForce(theParticles[i][j-1], theParticles[i][j], L0, k);
                    theForce += theDampForce(theParticles[i][j-1], theParticles[i][j], b);}
                if(i!=clothHeight-1){ theForce += (-1.0f) * theSpringForce(theParticles[i+1][j], theParticles[i][j], L0, k);
                    theForce += theDampForce(theParticles[i+1][j], theParticles[i][j], b);}

                if (state == GLFW_PRESS && (i == (clothHeight/2)-1) && (j == (clothWidth/2)-1)){
                    theForce += (glm::vec3(0.0f, 1.0f, 0.0f));
                }

                // Add gravity
                theForce += gravity;

                // Set the current acceleration of the particle
                theParticles[i][j].setAcc((1/m)*(theForce));

                // Resolve collision with ball
                //ballCollision(theParticles[i][j], ballCentrePos, ballRadius);
            }
        }

        /**************************** Draw cubes ****************************/
        glBindVertexArray(VAO);  // Bind VAO
        for(GLuint i = 0; i < clothHeight; i++) {
            for(GLuint j = 0; j < clothWidth;j++) {
                if (!theParticles[i][j].isStationary()) {
                    // Set the new positions and velocities of the particles
                    theParticles[i][j].setPos(RungeKuttaForPosDiff(theParticles[i][j], h) + theParticles[i][j].getPos());
                    newCubePositions[i][j] += (RungeKuttaForPosDiff(theParticles[i][j], h));
                    theParticles[i][j].setVel(RungeKuttaForVel(theParticles[i][j], h));
                }

                // Calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model;
                model = glm::translate(model, cubePositions[i][j]);
                glm::vec3 theTransVec = (newCubePositions[i][j]);
                model = glm::translate(model, theTransVec);
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                /** Kolla upp om det går att rendera linjer mellan varje partikel så att det blir mer tydligt hur tyget
                 *  rör sig. Kolla även upp om det är möjligt att rendera så att tomrummen fylls, aka att det faktiskt
                 *  ser ut som ett tyg och inte ett gridsystem.
                 */

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        /**************************** Draw ball ****************************/
        ball.render();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void do_movement() {
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

// Calculates the spring force acting on a particle
glm::vec3 theSpringForce(Particle p1, Particle p2, GLfloat L0, GLfloat k){
    GLfloat lengthBParticles = sqrtf(GLfloat(pow(p2.getPos().x-p1.getPos().x,2) + pow(p2.getPos().y-p1.getPos().y,2) + pow(p2.getPos().z-p1.getPos().z,2)));
    glm::vec3 length = p2.getPos() - p1.getPos();

    glm::vec3 Fk = k*(lengthBParticles-L0)*(normalize(length));
    return Fk;
}

// Calculates the damping force acting on a particle
glm::vec3 theDampForce(Particle p1, Particle p2, GLfloat b){
    glm::vec3 Fb = b * (p1.getVel() - p2.getVel());
    return Fb;
}

// Calculate the new velocity using RK4
glm::vec3 RungeKuttaForVel(Particle p, GLfloat h){
    glm::vec3 next, k1, k2, k3, k4;

    k1 = p.getAcc();
    k2 = p.getAcc() + (h/2.0f)*k1;
    k3 = p.getAcc() + (h/2.0f)*k2;
    k4 = p.getAcc() + h*k3;
    next = p.getVel() + (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);

    return next;
}

// Calculate the difference between the new position and the old one using RK4
glm::vec3 RungeKuttaForPosDiff(Particle p, GLfloat h){
    glm::vec3 next, k1, k2, k3, k4;
    k1 = p.getVel();
    k2 = p.getVel() + (h/2.0f)*k1;
    k3 = p.getVel() + (h/2.0f)*k2;
    k4 = p.getVel() + h*k3;
    next = (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);
    return next;
}

//Functions for the ball
/*
glm::vec3 normalise(glm::vec3 vector){
    float length = vector.length();
    return glm::vec3(vector.x/length, vector.y/length, vector.z/length);
}

// Detect collision between particle and ball
void ballCollision(Particle particle, const glm::vec3 centre, const float radius) {
    glm::vec3 collisionVector = particle.getPos() - centre;
    float lengthIntersection = collisionVector.length();
    // If particle is inside the ball
    if ( lengthIntersection < radius) {
        // Project the particle to the surface of the ball
        particle.setPos(normalise(collisionVector)*(radius - lengthIntersection));
    }
}

void createBall(float nrSubdivisions, float transX, float transY, float transZ) {
    int currentVertex = 0;
    int i, j;
    for(j = 0; j <= 90 - spaceVertices; j += spaceVertices) {
        for(i = 0; i <= 360 - spaceVertices; i += spaceVertices) {
            vertex[currentVertex].X = nrSubdivisions * sin((i)/180*M_PI) * sin((j)/180*M_PI) - transX; // Calculate X value
            vertex[currentVertex].Y = nrSubdivisions * cos((i)/180*M_PI) * sin((j)/180*M_PI) + transY; // Calculate Y value
            vertex[currentVertex].Z = nrSubdivisions * cos((j)/180*M_PI) - transZ; // Calculate Z value

            currentVertex++;

            // Add spaceVertices to j values
            vertex[currentVertex].X = nrSubdivisions * sin((i)/180*M_PI) * sin((j + spaceVertices)/180*M_PI) - transX;
            vertex[currentVertex].Y = nrSubdivisions * cos((i)/180*M_PI) * sin((j + spaceVertices)/180*M_PI) + transY;
            vertex[currentVertex].Z = nrSubdivisions * cos((j + spaceVertices)/180*M_PI) - transZ;

            currentVertex++;

            // Add spaceVertices to i values
            vertex[currentVertex].X = nrSubdivisions * sin((i + spaceVertices)/180*M_PI) * sin((j)/180*M_PI) - transX;
            vertex[currentVertex].Y = nrSubdivisions * cos((i + spaceVertices)/180*M_PI) * sin((j)/180*M_PI) + transY;
            vertex[currentVertex].Z = nrSubdivisions * cos((j)/180*M_PI) - transZ;

            currentVertex++;

            // Add spaceVertices to both the i and j values
            vertex[currentVertex].X = nrSubdivisions * sin((i + spaceVertices)/180*M_PI) * sin((j + spaceVertices)/180*M_PI) - transX;
            vertex[currentVertex].Y = nrSubdivisions * cos((i + spaceVertices)/180*M_PI) * sin((j + spaceVertices)/180*M_PI) + transY;
            vertex[currentVertex].Z = nrSubdivisions * cos((j + spaceVertices)/180*M_PI) - transZ;

            currentVertex++;
        }
    }
}

void displayBall(float radius) {
    // Scale the ball
    glScalef(0.0125 * radius, 0.0125 * radius, 0.0125 * radius);

    // Rotate to frontal position
    glRotatef(90, 1, 0, 0);

    // Draw it using triangle strips
    glBegin(GL_TRIANGLE_STRIP);
    for(int i = 0; i <= vertexAmount; i++) {
        glVertex3f(vertex[i].X, vertex[i].Y, -vertex[i].Z);
    }
    for(int i = 0; i <= vertexAmount; i++) {
        glVertex3f(vertex[i].X, vertex[i].Y, vertex[i].Z);
    }
    glEnd();
}
*/

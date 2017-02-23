#include <iostream>
#include <complex>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//SOIL
//#define SOIL_STATIC
#include <SOIL.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>



#include "ShaderProgram.hpp"
#include "Camera.h"
#include "Particle.h"


/*******************************************
 ****** FUNCTION/VARIABLE DECLARATIONS *****
 *******************************************/

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

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

bool calcvelocity = true;
glm::vec3 RungeKutta(Particle p, float h);

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


    float k = 1.0f;
    float b = 0.1f;
    float L0 = 0.5f;
    float m = 1.0f;
    float h = 0.1f;

    Particle new1 = Particle(m, glm::vec3( -0.25f, 0.0f, 0.0f) );
    Particle new2 = Particle(m, glm::vec3(   L0, 0.0f, 0.0f) );
    Particle new3 = Particle(m, glm::vec3(   L0, L0,   0.0f) );
    Particle new4 = Particle(m, glm::vec3( 0.0f, L0,   0.0f) );

    GLfloat verticesp[] = {
            new1.getPos().x, new1.getPos().y, new1.getPos().z, 1.0f, 1.0f, 0.0f,
            new2.getPos().x, new2.getPos().y, new2.getPos().z, 1.0f, 1.0f, 0.0f,
            new3.getPos().x, new3.getPos().y, new3.getPos().z, 1.0f, 1.0f, 0.0f,
            new4.getPos().x, new4.getPos().y, new4.getPos().z, 1.0f, 1.0f, 0.0f
    };


    GLuint indices[]{
        0, 1, 2,
        2, 3, 0
    };

    GLuint VBO;
    GLuint EBO;
    GLuint VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // Create Buffer ID
    glGenBuffers(1, &EBO); // Create Buffer ID

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind a buffer to the ID
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesp), verticesp, GL_STATIC_DRAW); // Copies the vertices data into the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind a buffer to the ID
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copies the vertices data into the buffer


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); //Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    //CUBE
    /*GLfloat vertices[] = {
            -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };
*/
    /***************** Vertex Buffer Objects (VBO) ******************/
    /*GLuint VBO;
    glGenBuffers(1, &VBO); // Create Buffer ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind a buffer to the ID
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies the vertices data into the buffer
*/
    /***************** Vertex Array Objects (VAO) ******************/
    /*GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); //Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);*/

    /***************** Shaders ********************/
    // Build and compile our shader program
    std::string vertexFilename = "../shaders/vertexShader.vert";
    std::string fragmentFilename = "../shaders/fragmentShader.frag";
    ShaderProgram theShaders(vertexFilename, "", "", "", fragmentFilename);
    theShaders();

    /**************** Uniform variables **********************/
    GLint modelLoc = glGetUniformLocation(theShaders, "model");
    GLint viewLoc = glGetUniformLocation(theShaders, "view");
    GLint projLoc = glGetUniformLocation(theShaders, "projection");
    GLint updateLoc = glGetUniformLocation(theShaders, "updatePos");
    glm::vec3 update;


    std::cout << modelLoc;
    std::cout << viewLoc;
    std::cout << projLoc;
    std::cout << updateLoc;
    /******************* RENDER LOOP ********************/
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        //do_movement();

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        //OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /**************** RENDER STUFF ****************/
        // Create camera transformation
        //glm::mat4 view;
       // glm::mat4 projection;
        //view = camera.GetViewMatrix();
        //projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 1000.0f);

        //Pass the matrices to the shader
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glm::vec3 Fk1 = theSpringForce(new1, new2, L0, k);
        glm::vec3 Fb1 = theDampForce(new1, new2, b);
        glm::vec3 Fk2 = theSpringForce(new2, new3, L0, k);
        glm::vec3 Fb2 = theDampForce(new2, new3, b);
        glm::vec3 Fk3 = theSpringForce(new3, new4, L0, k);
        glm::vec3 Fb3 = theDampForce(new3, new4, b);
        glm::vec3 Fk4 = theSpringForce(new4, new1, L0, k);
        glm::vec3 Fb4 = theDampForce(new4, new1, b);

        new1.setAcc((1/m)*(-Fk4+Fb4+Fk1-Fb1));
        new2.setAcc((1/m)*(-Fk1+Fb1+Fk2-Fb2));
        new3.setAcc((1/m)*(-Fk2+Fb2+Fk3-Fb3));
        new4.setAcc((1/m)*(-Fk3+Fb3+Fk4-Fb4));

        new1.setVel(RungeKutta(new1, h));
        new2.setVel(RungeKutta(new2, h));
        new3.setVel(RungeKutta(new3, h));
        new4.setVel(RungeKutta(new4, h));
        calcvelocity = false;

        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 4; i++) {
            if (i == 0){
                update = RungeKutta(new1, h);
                new1.setPos(update);
            }
            if (i == 1) {
                update = RungeKutta(new2, h);
                new2.setPos(update);
            }
            if (i == 2){
                update = RungeKutta(new3, h);
                new3.setPos(update);
            }
            if (i == 3){
                update = RungeKutta(new4, h);
                new4.setPos(update);
            }
            glUniform3f(updateLoc, update[0], update[1], update[2]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0); // Unbind VAO
        /*
        update = RungeKutta(new1, h);
        glUniform3f(updateLoc, update[0], update[1], update[2]);
        new1.setPos(update);

        update = RungeKutta(new2, h);
        glUniform3f(updateLoc, update[0], update[1], update[2]);
        new2.setPos(update);

        update = RungeKutta(new3, h);
        glUniform3f(updateLoc, update[0], update[1], update[2]);
        new3.setPos(update);

        update = RungeKutta(new4, h);
        glUniform3f(updateLoc, update[0], update[1], update[2]);
        new4.setPos(update);

        //new2.setPos(RungeKutta(new2, h));
        //new3.setPos(RungeKutta(new3, h));
        //new4.setPos(RungeKutta(new4, h));
        calcvelocity = true;

        //new1.print();
        glBindVertexArray(VAO);  // Bind VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Unbind VAO
*/
        //Draw cubes
        //glBindVertexArray(VAO);  // Bind VAO
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        /*for(GLuint i = 0; i < 10; i++)
        {
            //Calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        //glBindVertexArray(0); // Unbind VAO

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

glm::vec3 theSpringForce(Particle p1, Particle p2, float L0, float k){
    float lengthBParticles = (float)sqrt(pow(p2.getPos().x-p1.getPos().x,2) + pow(p2.getPos().y-p1.getPos().y,2) + pow(p2.getPos().z-p1.getPos().z,2));
    glm::vec3 length = p2.getPos() - p1.getPos();
    glm::vec3 Fk = k*(lengthBParticles-L0)*(normalize(length));
    return Fk;
}

glm::vec3 theDampForce(Particle p1, Particle p2, float b){
    glm::vec3 Fb = b * (p1.getVel() - p2.getVel());
    return Fb;
}

glm::vec3 RungeKutta(Particle p, float h){

    glm::vec3 next, k1, k2, k3, k4;

    if(calcvelocity) {
        k1 = p.getAcc();
        k2 = p.getAcc() + (h/2.0f)*k1;
        k3 = p.getAcc() + (h/2.0f)*k2;
        k4 = p.getAcc() + h*k3;
        next = p.getVel() + (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);
    }
    else{
        k1 = p.getVel();
        k2 = p.getVel() + (h/2.0f)*k1;
        k3 = p.getVel() + (h/2.0f)*k2;
        k4 = p.getVel() + h*k3;
        next = p.getPos() + (h/6.0f)*(k1 + 2.0f*k2 + 2.0f*k3 + k4);
    }
    return next;
}
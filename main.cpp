#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "ShaderProgram.hpp"


/*******************************************
 ********** Function declarations **********
 *******************************************/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    if(!glfwInit())
    {
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

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Define the viewport dimensions
    int width, height;


    /*************** Callback functions ****************/
    // Decides what should be done when a key is pressed
    glfwSetKeyCallback(window, key_callback);


    /************** Declare variables **************/
    GLfloat vertices[] = {
            // Positions (XYZ)    // Colors (RGB)
            0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,

    };

    GLuint indices[] = {
            0, 1, 3,    //first triangle
            1, 2, 3     //second triangle
    };

    /***************** EBOs ******************/
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /***************** VBOs ******************/
    // Create Buffer ID
    GLuint VBO;
    glGenBuffers(1, &VBO);

    //Bind a buffer to the ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copies the vertices data into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /***************** VAOs ******************/
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //BIND correct VBO and specify location (0)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); //Positions
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Colors
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); //Texture Coords

    // Enable all VAOs
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);


    /***************** Shaders ********************/
    // Build and compile our shader program
    std::string vertexFilename = "../shaders/vertexShader.vert";
    std::string fragmentFilename = "../shaders/fragmentShader.frag";
    ShaderProgram theShaders(vertexFilename, "", "", "", fragmentFilename);
    theShaders();


    /******************* RENDER LOOP ********************/
    while (!glfwWindowShouldClose(window))
    {

        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Update window size
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        //OpenGL settings
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /**************** RENDER STUFF ****************/

        // Bind VAO
        glBindVertexArray(VAO);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Unbind VAO
        glBindVertexArray(0);

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
}
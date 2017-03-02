//
// Created by Rebecca on 2017-02-23.
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


#ifndef TYGLADIG_PARTICLE_H
#define TYGLADIG_PARTICLE_H

class Particle{

private:
    GLfloat mass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    bool stationary;

public:
    Particle(GLfloat theMass, glm::vec3 thePosition);
    Particle(){}

    GLfloat getMass();
    glm::vec3 getPos();
    glm::vec3 getVel();
    glm::vec3 getAcc();

    void setMass(GLfloat newMass);
    void setPos(glm::vec3 newPosition);
    void setVel(glm::vec3 newVelocity);
    void setAcc(glm::vec3 newAcceleration);

    void makeStationary();
    bool isStationary();

    void print();
};



#endif //TYGLADIG_PARTICLE_H

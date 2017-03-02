//
// Created by Rebecca on 2017-02-23.
//

#include "Particle.h"

Particle::Particle(GLfloat theMass, glm::vec3 thePosition){
    mass = theMass;
    position = thePosition;
    velocity = glm::vec3{0.0f, 0.0f, 0.0f};
    acceleration = glm::vec3(0.0f,0.0f,0.0f);
    stationary = false;
};


GLfloat Particle::getMass(){
    return mass;
}

glm::vec3 Particle::getPos(){
    return position;
}

glm::vec3 Particle::getVel(){
    return velocity;
}

glm::vec3 Particle::getAcc(){
    return acceleration;
}

void Particle::setMass(GLfloat newMass) {
    mass = newMass;
}

void Particle::setPos(glm::vec3 newPosition) {
    position = newPosition;
}

void Particle::setVel(glm::vec3 newVelocity) {
    velocity = newVelocity;
}

void Particle::setAcc(glm::vec3 newAcceleration) {
    acceleration = newAcceleration;
}

void Particle::makeStationary() {
    stationary = true;
}

bool Particle::isStationary() {
    return stationary;
}

void Particle::print(){
    std::cout << "Position: " << position.x << "," << position.y << "," << position.z << std::endl
              << "Velocity: " << velocity.x << "," << velocity.y << "," << velocity.z << std::endl
              << "Acceleration: " << acceleration.x << "," << acceleration.y << "," << acceleration.z << std::endl;
}
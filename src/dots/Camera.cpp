/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include "Camera.hpp"

#include <stdio.h>

Camera::Camera() :
    eye(0.0f, 0.0f, 0.0f),
    at(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    speed(0.05f)
{
}

Camera::~Camera(){}

glm::mat4 Camera::view() const
{
    return glm::lookAt(eye, eye + at, up);
}

void Camera::move_forward()
{
    this->eye += this->speed * this->at;
}

void Camera::move_backward()
{
    this->eye -= this->speed * this->at;
    //printf("eye(%f, %f, %f)\n", this->eye.x, this->eye.y, this->eye.z);
}

void Camera::move_up()
{
    this->eye += this->speed * this->up;
}

void Camera::move_right()
{
    this->eye -= glm::normalize(glm::cross(this->at, this->up)) * this->speed;
}

void Camera::move_down()
{
    this->eye -= this->speed * this->up;
}

void Camera::move_left()
{
    this->eye += glm::normalize(glm::cross(this->at, this->up)) * this->speed;
}

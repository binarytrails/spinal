/*
 * @file
 * @author Vsevolod (Seva) Ivanov
 * @copyright Copyright 2017 Vsevolod (Seva) Ivanov. All rights reserved.
*/

#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
    public:
        Camera();
        ~Camera();

        glm::mat4 view() const;

        void move_up();
        void move_right();
        void move_down();
        void move_left();

        void move_forward();
        void move_backward();

    private:
        glm::vec3 eye;
        glm::vec3 at;
        glm::vec3 up;

        GLfloat speed;
};

/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

Window* window;
Shader* shader;
Camera* camera;

std::string data_f = "data/five_y";

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> vertices_i;

GLuint vbo, vao, ebo;

glm::mat4 view;
glm::mat4 projection;

// callbacks {

static void framebuffer_size_cb(GLFWwindow* w, int width, int height)
{
    window->width(width);
    window->height(height);
    glViewport(0, 0, width, height);
}

static void key_cb(GLFWwindow* w, int key, int scancode, int action, int mode)
{
    //printf("keyboard: %i\n", key);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(w, GL_TRUE);
    }

    switch (key)
    {
        case GLFW_KEY_LEFT:
            //mesh->rotate(glm::vec3(0, 1, 0));
            break;

        case  GLFW_KEY_RIGHT:
            //mesh->rotate(glm::vec3(0, -1, 0));
            break;

        case GLFW_KEY_UP:
            //mesh->rotate(glm::vec3(1, 0, 0));
            break;

        case GLFW_KEY_DOWN:
            //mesh->rotate(glm::vec3(-1, 0, 0));
            break;

        case GLFW_KEY_W:
            camera->move_down();
            break;
        
        case GLFW_KEY_S:
            camera->move_up();
            break;
        
        case GLFW_KEY_A:
            camera->move_left();
            break;
        
        case GLFW_KEY_D:
            camera->move_right();
            break;
    }
}

static void mouse_scroll_cb(GLFWwindow *w, double xoffset, double yoffset)
{
    if (yoffset > 0)        camera->move_forward();
    else if (yoffset < 0)   camera->move_backward();
}

// } callbacks

bool load_data_file()
{
    std::ifstream ifs;
    ifs.open(data_f);

	if (!ifs.is_open())
        return false;

    GLfloat x, y, z;
    unsigned short n;

    ifs >> n;
    printf("Found %i data points\n", n);

    for(unsigned short i = 0; i < n; i++)
    {
        ifs >> x >> y >> z;
        vertices.push_back(glm::vec3(x, y, z));
        printf("(%f, %f, %f)\n", x, y, z);
    }

    ifs.close();
    return true;
}

void gen_vertices_i()
{
    // TODO
}

void init_buffers()
{
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) * vertices.size(),
                 &vertices[0], GL_STATIC_DRAW);

    // has to be before ebo bind
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(vertices_i) * vertices_i.size(),
                 &vertices_i[0],
                 GL_STATIC_DRAW);

    // enable vao -> vbo pointing
    glEnableVertexAttribArray(0);
    // setup formats of my vao attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    // unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind vao
    glBindVertexArray(0);
}

void draw_loop()
{
    while (!glfwWindowShouldClose(window->get()))
    {
        glfwPollEvents();

        projection = glm::perspective(
            45.0f,
            (GLfloat) window->width() / (GLfloat) window->height(),
            0.1f, 100.0f);

        // clear the colorbuffer
        glClearColor(255, 255, 255, 0); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        view = glm::translate(camera->view(), glm::vec3(0.0f, 0.0f, -1.0f));

        // TODO render

        glfwSwapBuffers(window->get());
    }
}

int main(int argc, char *argv[])
{
    // init

    camera = new Camera();
    window = new Window(800, 800, "Spinal");

    glfwSetKeyCallback(window->get(), key_cb);
    glfwSetFramebufferSizeCallback(window->get(), framebuffer_size_cb);
    glfwSetScrollCallback(window->get(), mouse_scroll_cb);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, window->width(), window->height());
    
 	if (!load_data_file())
    {
        std::cout << "Can't open data file '" << data_f << "'" << std::endl;
        return 1;
    }

    gen_vertices_i();

    init_buffers();

    Shader shader("shaders/default.vs", "shaders/default.fs");
    shader.use();

    draw_loop();

    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    delete camera;
    delete window;

    return 0;
}

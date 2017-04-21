/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <stdio.h>
#include <iostream>

#include <vector>
#include <unistd.h> // sleep()
#include <libserialport.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

// bash colors
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

bool parse_spinal_serial(const std::string data)

class Spinal
{
    public:
        Spinal();

        draw();

    private:
        ~Spinal()

        init_glfw();
        init_glew();
        init_opengl();
        init_buffers();
        init_bno_points();
        init_serial();

        upload();

        // coords system
        upload_mvp();
        void rotate_model(const glm::vec3 spin)
        glm::vec3 rotate_point(glm::vec3 point, const glm::vec3 spin)

        read_serial();
        parse_serial();
        // exclusive substring without start and end
        std::string substr_segment(std::string start, std::string end, std::string str)
        print_segment(const uint8_t id, const char* segment);

        void find_serial_ports();
        const char* first_serial_port();

        void gen_vertices_i();
        generate_spine();
        glm::mat4 euler_angles(const glm::vec3 spin);
        // FIXME slight shift to bottom on each generation
        std::vector<glm::vec3> catmullrom_spline()

        // attributes

        const glm::vec3 ERROR_VEC3 = glm::vec3(-1, -1, -1);
        const std::vector<glm::vec3> ERROR_VEC(5, ERROR_VEC3);

        Window* window;
        Shader* shader;
        Camera* camera;

        GLuint vbo, vao, ebo;

        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        // from lowest to highest sensor
        std::vector<glm::vec3> vertices;
        std::vector<GLushort>  vertices_i;
        // x == roll; y == pitch; z == yaw
        std::vector<glm::vec3> vertices_r(5, ERROR_VEC3);

        GLfloat rotate_angle = 1.0f / 20.0f;
        GLenum render_m      = GL_POINTS;
        glm::vec4 BG_COLOR(255, 255, 255, 0);

        std::string serial_buff = "";
        struct sp_port *serial_p;
        const char* serial_url = "/dev/ttyUSB0";

        bool GENERATE_SPINE = false;
        const bool VERBOSE_DEBUG = false;
};

// callbacks

static void framebuffer_size_cb(GLFWwindow* w, int width, int height)

static void key_cb(GLFWwindow* w, int key, int scancode, int action, int mode)

static void mouse_scroll_cb(GLFWwindow *w, double xoffset, double yoffset)

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

class Spinal
{
    public:
        Spinal();
        ~Spinal();

        void draw();

    private:
        void init_glfw();
        void init_glew();
        void init_opengl();
        void init_buffers();
        void init_bno_points();
        void init_serial();

        void upload();

        //! Coordinates system
        void upload_mvp();
        void rotate_model(const glm::vec3 spin);
        glm::vec3 rotate_point(glm::vec3 point, const glm::vec3 spin);

        void read_serial();
        void parse_serial();
        //! Exclusive substring without start & end
        std::string substr_segment(std::string start,
                                   std::string end, std::string str);
        void print_segment(const uint8_t id, const char* segment);

        void find_serial_ports();
        const char* first_serial_port();

        void gen_spine();
        void gen_vertices_i();
        glm::mat4 gen_euler_angles(const glm::vec3 spin);
        // FIXME slight shift to bottom on each generation
        std::vector<glm::vec3> gen_catmullrom_spline();

        // attributes

        const glm::vec3 ERROR_VEC3 = glm::vec3 (-1, -1, -1);
        const std::vector<glm::vec3> ERROR_VEC;// (5, ERROR_VEC3);

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
        std::vector<glm::vec3> vertices_r; //(5, ERROR_VEC3);

        GLfloat rotate_angle = 1.0f / 20.0f;
        GLenum render_m = GL_POINTS;
        glm::vec4 BG_COLOR = glm::vec4 (255, 255, 255, 0);

        std::string serial_buff;
        struct sp_port *serial_p;
        const char* serial_url;

        bool GENERATE_SPINE;
        const bool VERBOSE_DEBUG;
};

//! GLFW Window context to allow the usage of class instances in callbacks
struct CallbackContext
{
    Spinal* spinal;
};

//! Returning a CallbackContext attached to a GLFW window
static CallbackContext* getWindowContext(GLFWwindow* w)
{
    return static_cast<CallbackContext*>(glfwGetWindowUserPointer(w));
}

static CallbackContext callbackContext;

// callbacks

//! GLFW window framebuffer callback
static void framebuffer_size_cb(GLFWwindow* w, int width, int height)
{
    CallbackContext* cbcPtr = getWindowContext(w);
}

static void key_cb(GLFWwindow* w, int key, int scancode, int action, int mode)
{
    CallbackContext* cbcPtr = getWindowContext(w);
}

static void mouse_scroll_cb(GLFWwindow *w, double xoffset, double yoffset)
{
    CallbackContext* cbcPtr = getWindowContext(w);
}

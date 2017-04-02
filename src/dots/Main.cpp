/*
 * @file
 * @author Vsevolod (Seva) Ivanov
 * @copyright Copyright 2017 Vsevolod (Seva) Ivanov. All rights reserved.
 *
 * FIXME
 *  (1) Spine IMU sensors order shift
 *  (2) Fast serial communication
*/

#include <stdio.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <unistd.h> // sleep()
#include <libserialport.h>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

const glm::vec3 ERROR_VEC3 = glm::vec3(-1, -1, -1);

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

std::string data_f   = "data/five_y";
GLfloat rotate_angle = 1.0f / 20.0f;
GLenum render_m      = GL_POINTS;

struct sp_port *serial_p;
// can be overwritten with arg1 e.g. ./run.sh /dev/ttyUSB1
const char* serial_url = "/dev/ttyUSB0";
unsigned int SERIAL_TIMEOUT_MS = 3000;

int in_turn = 0;
std::string in_data[5];

bool compute_catmullrom_spline()
{
    if (vertices_r.size() < 5)
        return false;

    std::vector<glm::vec3> vbuffer1 = vertices_r;

    if (vbuffer1.size() < 4)
    {
        printf("A minimum of 4 points is requiered "
               "to generate a Catmull-Rom Spline.\n");
        return false;
    }

    printf("Generating Catmull-Rom Spline..\n");

    GLfloat s = 0.5f;

    glm::mat4 basis(
        -s,     2-s,    s-2,    s,      // [0][0]-[0][3]
        2*s,    s-3,    3-2*s,  -s,
        -s,     0,      s,      0,
        0,      1,      0,      0
    );

    float tmax = 10.0f;
    float step = 1.0f / tmax;

    // add artificial before first
    vbuffer1.insert(vbuffer1.begin(), vbuffer1.at(0) - step);
    // add artificial after last
    vbuffer1.push_back(vbuffer1.at(vbuffer1.size()-1) + step);

    std::vector<glm::vec3> vbuffer2;

    // for n segments with n+3 control points
    for (uint16_t i = 1; i < vbuffer1.size() - 2; i++)
    {
        // brute force (for n segments)
        for (float t = 0.0f; t < 1.0f - step; t += step)
        {
            glm::vec3 p0 = vbuffer1.at(i - 1);
            glm::vec3 p1 = vbuffer1.at(i);
            glm::vec3 p2 = vbuffer1.at(i + 1);
            glm::vec3 p3 = vbuffer1.at(i + 2);

            glm::vec4 params = glm::vec4(t*t*t, t*t, t, 1.0f);

            glm::mat4x3 control(p0, p1, p2, p3);

            /* operations order:
             *      mat4 * vec4 -> vec4 is column vector
             *      vec4 * mat4 -> vec4 is row vector
             */

            glm::vec3 pn = params * (
                    glm::transpose(basis) * glm::transpose(control)
            );
            //printf("\n p_%i t=%f (%f, %f, %f)\n\n", i, t, pn.x, pn.y, pn.z);

            vbuffer2.push_back(pn);
        }
    }

    vertices.clear();
    vertices = vbuffer2;

    return true;
}

void rotate_model(const glm::vec3 spin)
{
    model = glm::rotate(model, spin.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, spin.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, spin.z, glm::vec3(0, 0, 1));
}

glm::vec3 rotate_point(glm::vec3 point, const glm::vec3 spin)
{
    point = glm::rotateX(point, spin.x);
    point = glm::rotateY(point, spin.y);
    point = glm::rotateZ(point, spin.z);
    return point;
}

void gen_vertices_i()
{
    vertices_i.clear();

    if (render_m == GL_POINTS)
    {
        for (int p = 0; p < vertices.size(); p++)
        {
            vertices_i.push_back(p);
        }
    }
    else if (render_m == GL_LINES)
    {
        for (int p = 0; p < vertices.size() - 1; p++)
        {
            // edge
            vertices_i.push_back(p);
            vertices_i.push_back(p + 1);
            //printf("Adding edge between p%i <-> p%i\n", p, p + 1);
        }
    }
}

void upload_to_gpu()
{
    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(glm::vec3) * vertices.size(),
                     &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // indices
    glBindVertexArray(vao);
        glDrawElements(render_m, vertices_i.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

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

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(w, GL_TRUE);
                break;

            case GLFW_KEY_P:
                render_m = GL_POINTS;
                gen_vertices_i();
                upload_to_gpu();
                break;

            case GLFW_KEY_L:
                render_m = GL_LINES;
                gen_vertices_i();
                upload_to_gpu();
                break;
        }
    }

    switch (key)
    {
        case GLFW_KEY_LEFT:
            rotate_model(glm::vec3(0, rotate_angle, 0));
            break;

        case GLFW_KEY_RIGHT:
            rotate_model(glm::vec3(0, -1.0f * rotate_angle, 0));
            break;

        case GLFW_KEY_UP:
            rotate_model(glm::vec3(rotate_angle, 0, 0));
            break;

        case GLFW_KEY_DOWN:
            rotate_model(glm::vec3(-1.0f * rotate_angle, 0, 0));
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

    vertices.clear();

    for(unsigned short i = 0; i < n; i++)
    {
        ifs >> x >> y >> z;
        vertices.push_back(glm::vec3(x, y, z));
        printf("p%i (%f, %f, %f)\n", i, x, y, z);
    }

    ifs.close();
    return true;
}

void find_serial_ports()
{
    struct sp_port **ports;

    sp_list_ports(&ports);

    for (int i = 0; ports[i]; i++)
        printf("Found port: '%s'.\n", sp_get_port_name(ports[i]));

    sp_free_port_list(ports);
}

const char* get_first_serial_port()
{
    char *name;
    struct sp_port **ports;

    sp_list_ports(&ports);
        name = sp_get_port_name(ports[0]);
    sp_free_port_list(ports);

    return name;
}

bool init_spinal_serial()
{
    //serial_url = get_first_serial_port();
    printf("Opening port '%s' \n", serial_url);

    sp_return error = sp_get_port_by_name(serial_url, &serial_p);

    if (error != SP_OK)
    {
        printf("Error finding serial device\n");
        return 0;
    }

    error = sp_open(serial_p, SP_MODE_READ);

    if (error != SP_OK)
    {
        printf("Error opening serial device\n");
        return 0;
    }

    sp_set_baudrate(serial_p, 9600);
    return 1;
}

glm::mat4 compute_euler_angles(const glm::vec3 spin)
{
    float roll  = spin.x;
    float pitch = spin.y;
    float yaw   = spin.z;

    float c1 = cos(glm::radians(roll));
    float s1 = sin(glm::radians(roll));
    float c2 = cos(glm::radians(pitch)); // intrinsic rotation
    float s2 = sin(glm::radians(pitch));
    float c3 = cos(glm::radians(yaw));
    float s3 = sin(glm::radians(yaw));

    return glm::mat4(
        c2 * c3,    s1 * s3 + c1 * c3 * s2,     c3 * s1 * s2 - c1 * s3, 0,
        -s2,        c1 * c2,                    c2 * s1,                0,
        c2 * s3,    c1 * s2 * s3 - c3 * s1,     c1 * c3 + s1 * s2 * s3, 0,
        0,          0,                          0,                      1
    );
}

void print_spinal_segment(const uint16_t id, const std::string segment)
{
    switch (id)
    {
        case 0:
            printf(RED      "%s\n"   RESET, segment.c_str());
            break;
        case 1:
            printf(GREEN    "%s\n"   RESET, segment.c_str());
            break;
        case 2:
            printf(MAGENTA  "%s\n"   RESET, segment.c_str());
            break;
        case 3:
            printf(YELLOW   "%s\n"   RESET, segment.c_str());
            break;
        case 4:
            printf(CYAN     "%s\n\n" RESET, segment.c_str());
            break;
    }
}

// FIXME 2 fast communication: make robust and disregard too short results
// exclusive substring without start and end
std::string substr_ex(std::string start, std::string end, std::string str)
{
    std::string r;
    try
    {
        unsigned first = str.find(start);
        unsigned last = str.find(end);

        r = str.substr(first + start.length(), last - first - start.length());
    }
    catch (const std::out_of_range& e)
    {
        fprintf(stderr, RED "Exception std::out_of_range in substr_ex(..)\n"
                            "Returning '%s'\n" RESET, r.c_str());
    }
    return r;
}

bool parse_spinal_serial(const std::string data)
{
    int id = -1;
    glm::vec3 euler_angles = ERROR_VEC3;

    try
    {
        id = std::stoi(substr_ex("bno", "x", data), nullptr, 10);
    }
    catch (const std::invalid_argument& e)
    {
        return false;
    }

    euler_angles = glm::vec3(
        (GLfloat) std::stof(substr_ex("x", "y", data)),
        (GLfloat) std::stof(substr_ex("y", "z", data)),
        (GLfloat) std::stof(substr_ex("z", "$", data))
    );

    /*
    std::cout << "id: " << id <<
                 " x: " << euler_angles.x <<
                 " y: " << euler_angles.y <<
                 " z: " << euler_angles.z << std::endl;
    */

    if (id == -1 || euler_angles == ERROR_VEC3)
    {
        fprintf(stderr, RED "ID or euler_angles aren't extracted\n" RESET);
        return false;
    }

    // has previous rotation (initialized)
    if (vertices_r.at(id) != ERROR_VEC3)
    {
        print_spinal_segment(id, data);

        // spin = current - last
        glm::vec3 spin = euler_angles - vertices_r.at(id);

        glm::mat4 euler_rotation = compute_euler_angles(spin);

        // apply spin
        vertices.at(id) = glm::vec4(vertices.at(id), 0.0f) * euler_rotation;

        //upload_to_gpu(); flickering if only upload
    }
    // else initialize
    vertices_r.at(id) = euler_angles;

    return true;
}

void read_spinal_serial()
{
    int bytes_waiting = sp_input_waiting(serial_p);

    if (bytes_waiting > 0)
    {
        //printf("Bytes waiting %i\n", bytes_waiting);
        char byte_buff[512];
        int byte_num = 0;

        //FIXME 1 nonblock mutex
        byte_num = sp_nonblocking_read(serial_p, byte_buff, 512);
        //byte_num = sp_blocking_read(serial_p, byte_buff, 512, SERIAL_TIMEOUT_MS);

        for (int i = 0; i < byte_num; i++)
        {
            char c = byte_buff[i];

            in_data[in_turn] += c;

            if (c != '$')
                continue;

            // FIXME 1 IMU sensors order shift
            // i.e bno1x00.00y11.11z22.22$
            std::string data(in_data[in_turn]);
            parse_spinal_serial(data);

            //compute_catmullrom_spline();

            in_data[in_turn] = "";
            in_turn  = (in_turn + 1) % 5;
            //sleep(10);
            break;
        }
    }
    fflush(stdout);
}

void init_buffers()
{
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) * vertices.size(),
                 &vertices[0], GL_STREAM_DRAW);

    // has to be before ebo bind
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(vertices_i) * vertices_i.size(),
                 &vertices_i[0],
                 GL_STREAM_DRAW);

    // enable vao -> vbo pointing
    glEnableVertexAttribArray(0);
    // setup formats of my vao attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    // unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind vao
    glBindVertexArray(0);
}

void render()
{
    view = glm::translate(camera->view(), glm::vec3(0.0f, 0.0f, -2.0f));

    projection = glm::perspective(
        45.0f,
        (GLfloat) window->width() / (GLfloat) window->height(),
        0.1f, 100.0f);

    shader->use();

    // locate in shaders gpu
    GLint modelLoc = glGetUniformLocation(shader->programId, "model");
    GLint viewLoc = glGetUniformLocation(shader->programId, "view");
    GLint projLoc = glGetUniformLocation(shader->programId, "projection");

    // send to shaders on gpu
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void draw_loop()
{
    while (!glfwWindowShouldClose(window->get()))
    {
        glfwPollEvents();

        // clear the colorbuffer
        glClearColor(0, 0, 0, 0); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        read_spinal_serial();
        upload_to_gpu();
        render();

        glfwSwapBuffers(window->get());
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        serial_url = argv[1];

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

    shader = new Shader("shaders/default.vs", "shaders/default.fs");

 	if (!load_data_file())
    {
        std::cout << "Can't open data file '" << data_f << "'" << std::endl;
        return 1;
    }
    gen_vertices_i();
    init_buffers();
    upload_to_gpu();

    printf("Available serial ports:\n");
    find_serial_ports();

    if (!init_spinal_serial())
        return 1;

    draw_loop();

    sp_close(serial_p);

    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    delete shader;
    delete camera;
    delete window;

    return 0;
}

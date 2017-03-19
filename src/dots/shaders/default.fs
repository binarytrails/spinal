/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#version 330 core

in vec3 pos;
out vec4 color;

void main()
{
    vec3 c1 = vec3(abs(pos.x), abs(pos.y), abs(pos.z));
    color = vec4(c1, 1.0f);
}

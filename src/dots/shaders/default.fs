/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#version 330 core

in vec3 pos;
out vec4 color;

void main()
{
    float opacity = 1;

    vec3 black = vec3(0);
    vec3 colors = vec3(pos.x, pos.y, pos.z);
    vec3 greys = vec3(1) * pos.y;
    
    color = vec4(black, opacity);
}

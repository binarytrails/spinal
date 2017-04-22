/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include "Spinal.hpp"

int main(int argc, char *argv[])
{
//    if (argc > 1)
//        serial_url = argv[1];

    Spinal* spinal = new Spinal();

    spinal->draw();

    delete spinal;
    return 0;
}

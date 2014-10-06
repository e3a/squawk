
#include "lame/lame.h"
#include "squawk.h"

int main(int ac, const char* av[]) {



    bool result = libav.parse(audiofile, av[1] );
    std::cout << result << std::endl << audiofile.album << audiofile.title << std::endl;

}


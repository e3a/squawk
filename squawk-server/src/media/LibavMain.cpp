
#include "libavcpp.h"
#include "squawk.h"

int main(int ac, const char* av[]) {

    squawk::media::LibAVcpp libav;
    struct squawk::media::Audiofile audiofile;
    bool result = libav.parse(audiofile, "", av[1] );
    std::cout << result << std::endl << audiofile.album << audiofile.title << std::endl;

}

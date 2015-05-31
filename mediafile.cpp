#include "mediafile.h"
#include "commons/media.h"
mediafile::mediafile()
{
}


int main(int, const char* av[]) {
    std::cout << "load mediafile: " << av[1] << std::endl;

    commons::media::MediaFile file = commons::media::MediaParser::parseFile( av[1] );
    std::cout << "mediafile has " << file.getAudioStreams().size() << " audiostreams and " << file.getVideoStreams().size() << " videostreams." << std::endl;
    std::cout << file << std::endl;

}

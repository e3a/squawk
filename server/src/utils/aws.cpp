#include "amazonfacade.h"

int main(int argc, char *argv[]){
    amazon::AmazonFacade facade;
    amazon::BookResult result = facade.bookByIsbn("AKIAIC5IGJNCXA3YR2FA", "Sx/qBFtxr0ryNpcaUQ4RLaTCWRkB89MTyEY/GNhk", "9780786751075");
    std::cout << result.title() << std::endl;
}

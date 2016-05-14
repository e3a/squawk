#include <signal.h>

#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include "squawkconfig.h"
#include "squawkserver.h"

int main ( int ac, const char* av[] ) {

    try {
        squawk::SquawkConfig * squawk_config = new squawk::SquawkConfig();

        if ( ! squawk_config->parse ( ac, av ) ) {
            exit ( 1 );
        }

        squawk_config->load ( squawk_config->configFile() );

        if ( ! squawk_config->validate() ) {
            exit ( 1 );
        }

        squawk_config->save ( squawk_config->configFile() );

        //load the logger
        if ( squawk_config->logger() != "" ) {
            log4cxx::PropertyConfigurator::configure ( squawk_config->logger() );

        } else {
            log4cxx::BasicConfigurator::configure();
        }

        //ensure the tmp directory exist
        if ( ! boost::filesystem::is_directory ( squawk_config->tmpDirectory() ) ) {
            boost::filesystem::create_directory ( squawk_config->tmpDirectory() );
        }

        // Block all signals for background thread.
        sigset_t new_mask;
        sigfillset ( &new_mask );
        sigset_t old_mask;
        pthread_sigmask ( SIG_BLOCK, &new_mask, &old_mask );

        squawk::SquawkServer::instance()->start ( squawk_config );

        // Restore previous signals.
        pthread_sigmask ( SIG_SETMASK, &old_mask, 0 );

        // Wait for signal indicating time to shut down.
        sigset_t wait_mask;
        sigemptyset ( &wait_mask );
        sigaddset ( &wait_mask, SIGINT );
        sigaddset ( &wait_mask, SIGQUIT );
        sigaddset ( &wait_mask, SIGTERM );
        pthread_sigmask ( SIG_BLOCK, &wait_mask, 0 );
        int sig = 0;

        std::cout << "wait for signal" << std::endl;

        sigwait ( &wait_mask, &sig );

        std::cout << "shutdown server" << std::endl;

        squawk::SquawkServer::instance()->stop();

        return 0;

    } catch ( std::exception& e ) {
        std::cerr << e.what() << std::endl;

    } catch ( int & e ) {
        std::cerr << "int:" << e << std::endl;

    } catch ( char * e ) {
        std::cerr << "char*:" << e << std::endl;

    } catch ( std::string * e ) {
        std::cerr << "std:" << e << std::endl;

    } catch ( ... ) {
        try {
            std::exception_ptr eptr = std::current_exception();

            if ( eptr != std::exception_ptr() ) {
                std::rethrow_exception ( eptr );
            }

        } catch ( const std::exception& e ) {
            std::cout << "Caught exception \"" << e.what() << "\"\n";
        }
    }

    return 1;
}


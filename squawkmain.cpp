#include <signal.h>

#include "server/src/squawkconfig.h"
#include "server/src/squawkserver.h"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

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
        el::Configurations default_conf_;
        default_conf_.setGlobally( el::ConfigurationType::Enabled, "true" );
        default_conf_.setGlobally( el::ConfigurationType::MillisecondsWidth, "6" );
        default_conf_.setGlobally( el::ConfigurationType::Format, "%datetime [%logger] %msg" );
        default_conf_.setGlobally( el::ConfigurationType::PerformanceTracking, "true" );
        default_conf_.set(el::Level::Debug, el::ConfigurationType::Format, "%datetime %level [%logger] %func: %msg" );
        default_conf_.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level [%logger] %func: %msg" );
        default_conf_.set(el::Level::Fatal, el::ConfigurationType::Format, "%datetime %level [%logger] %func: %msg" );

        if ( squawk_config->logger() != "" ) {
            default_conf_.setGlobally( el::ConfigurationType::Filename, squawk_config->logger() );
            default_conf_.setGlobally( el::ConfigurationType::MaxLogFileSize, "2097152" ); //2MB
            default_conf_.setGlobally( el::ConfigurationType::LogFlushThreshold, "0" ); // Flush after every 100 logs
            default_conf_.setGlobally( el::ConfigurationType::ToFile, "true" );
            default_conf_.setGlobally( el::ConfigurationType::ToStandardOutput, "false" );

        } else {
            default_conf_.setGlobally( el::ConfigurationType::ToFile, "false" );
            default_conf_.setGlobally( el::ConfigurationType::ToStandardOutput, "true" );
        }

        el::Loggers::reconfigureAllLoggers(default_conf_);
        el::Loggers::reconfigureLogger("upnp", default_conf_);
        el::Configurations http_conf_;
        http_conf_.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %msg" );
        el::Loggers::reconfigureLogger("http", http_conf_);

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

        sigwait ( &wait_mask, &sig );

        squawk::SquawkServer::instance()->stop();

        return 0;

    } catch ( ... ) {
        try {
            std::exception_ptr eptr = std::current_exception();

            if ( eptr != std::exception_ptr() ) {
                std::rethrow_exception ( eptr );
            }

        } catch ( const std::exception& e ) {
            LOG(FATAL) << "Caught Exception: < " << e.what() << " >";
        }
    }
    return 1;
}


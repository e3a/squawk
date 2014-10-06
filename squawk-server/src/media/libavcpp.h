#ifndef LIBAVCPP_H
#define LIBAVCPP_H

#include "squawk.h"
#include "log4cxx/logger.h"

extern "C"{

#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

namespace squawk {
namespace media {

class LibAVcpp : public MetadataParser {
public:
    LibAVcpp();
    virtual bool parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, const std::string file);
private:
    static log4cxx::LoggerPtr logger;
};
}}
#endif // LIBAVCPP_H

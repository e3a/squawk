/*
    The UPNP Connection Manager
    Copyright (C) 2013  <etienne> <etienne@mail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef UPNPCONNECTIONMANAGER_H
#define UPNPCONNECTIONMANAGER_H

#include <list>
#include <map>
#include <string>

#include "http.h"
#include "upnp.h"
#include "xml.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

/**
 * @brief The UpnpConnectionManager class.
 */
class UpnpConnectionManager : public http::HttpServlet {
public:
    UpnpConnectionManager( const std::string & path, http::HttpServletContext ) : HttpServlet(path) {}

    UpnpConnectionManager( const std::string & path ) : HttpServlet(path) {}
    virtual void do_post(::http::HttpRequest & request, ::http::HttpResponse & response);
    virtual void do_default( const std::string & method, http::HttpRequest & request, http::HttpResponse & response );
private:
    static log4cxx::LoggerPtr logger;
    std::string SOURCE_TYPES =
        "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN," \
        "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM," \
        "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED," \
        "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_50_AC3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HD_60_AC3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO," \
        "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_MULT5," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AC3," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_940," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L31_HD_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L32_HD_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_HP_HD_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_1080i_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_HD_720p_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_AAC," \
        "http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_AAC," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_50_AC3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HD_60_AC3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_HP_HD_AC3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA," \
        "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL," \
        "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO," \
        "http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AAC," \
        "http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AMR," \
        "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3," \
        "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE," \
        "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL," \
        "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO," \
        "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL," \
        "http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL_MULT5," \
        "http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320," \
        "http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_320," \
        "http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO," \
        "http-get:*:audio/mp4:DLNA.ORG_PN=AAC_MULT5_ISO," \
        "http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM," \
        "http-get:*:image/jpeg:*," \
        "http-get:*:video/avi:*," \
        "http-get:*:video/divx:*," \
        "http-get:*:video/x-matroska:*," \
        "http-get:*:video/mpeg:*," \
        "http-get:*:video/mp4:*," \
        "http-get:*:video/x-ms-wmv:*," \
        "http-get:*:video/x-msvideo:*," \
        "http-get:*:video/x-flv:*," \
        "http-get:*:video/x-tivo-mpeg:*," \
        "http-get:*:video/quicktime:*," \
        "http-get:*:audio/mp4:*," \
        "http-get:*:audio/x-wav:*," \
        "http-get:*:audio/x-flac:*," \
        "http-get:*:application/ogg:*";
};
} // upnp
} // squawk
#endif // UPNPCONNECTIONMANAGER_H

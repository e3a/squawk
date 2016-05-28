#ifndef VIDEO_H
#define VIDEO_H

#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>

#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

namespace commons {
/**
 * @brief Media Utilities.
 */
namespace media {

inline std::string time_to_string(int playtime) {
  int _seconds = playtime;
  std::stringstream ss;
  if(_seconds > 3600) {
    int hours = _seconds / 3600;
    if(hours < 10) {
      ss << "0";
    }
    ss << hours << ":";
    _seconds = _seconds - (hours * 3600);
  } else ss << "00:";
  if(_seconds > 60) {
    int minutes = _seconds / 60;
    if(minutes < 10) {
      ss << "0";
    }
    ss << minutes << ":";
    _seconds = _seconds - (minutes * 60);
  } else ss << "00:";
  if(_seconds < 10) {
    ss << "0";
  }
  ss << _seconds << ".000";
  return ss.str();
}

enum class CODEC {
    MPEG1 = 0,
    MPEG2 = 1,
    MPEG3 = 2,
    FLAC = 3,
    APE = 4,
    VORBIS = 5,
    AAC = 6,
    AC3 = 7,
    MPC = 8,
    
    MJPEGB = 9,
    H264 = 10,
    UNKNOWN = 11
};

static std::string codec_names[] = {
  "MPEG1", "MPEG2", "MPEG3", "FLAC", "APE", "VORBIS", "AAC", "AC3", "MPC",
  "MPFPEGB", "H264", 
  "UNKNOWN"
};

enum class DLNA_PROFILE {
  JPEG_SM, 
  JPEG_MED,
  JPEG_LRG,
  JPEG_RES_,
  JPEG_TN,
  JPEG_SM_ICO,
  JPEG_LRG_ICO,
  
  PNG_TN,
  PNG_SM_ICO,
  PNG_LRG_ICO,
  PNG_LRG,
  
  AC3,

  AMR_3GPP,
  AMR_WBplus,
  
  ATRAC3plus,
  
  LPCM,
  LPCM_low,
  LPCM_MPS,
  
  MP3,
  MP3X,
  
  AAC_ADTS,
  AAC_ADTS_192,
  AAC_ADTS_320,
  AAC_ISO,
  AAC_ISO_192,
  AAC_ISO_320,
  AAC_LTP_ISO,
  AAC_LTP_MULT5_ISO,
  AAC_LTP_MULT7_ISO,
  AAC_MULT5_ADTS,
  HEAAC_L2_ADTS,
  HEAAC_L2_ISO,
  HEAACv2_L2_128,
  HEAAC_L2_ISO_128,
  HEAAC_L3_ADTS,
  HEAAC_L3_ISO,
  HEAAC_MULT5_ADTS,
  HEAAC_MULT5_ISO,
  HEAAC_L2_ADTS_320,
  HEAAC_L2_ISO_320,
  BSAC_ISO,
  BSAC_MULT5_ISO,
  AAC_MPS,
  ALS_ISO,
  ALS_MULT5_ISO,
  HEAAC_L4,
  HEAAC_MPS,
  HEAAC_MULT7,
  HEAACv2_L2,
  HEAACv2_L2_320,
  HEAACv2_L2_MPS_DAB,
  HEAACv2_L3,
  HEAACv2_L4,
  HEAACv2_MULT5,
  HEAACv2_MULT7,
  MPEG2_AAC_MPS,
  WMABASE,
  WMAFULL,
  WMAPRO,
  WMALSL,
  WMALSL_MULT5,
  
  MPEG1,
  
  MPEG_PS_NTSC,
  MPEG_PS_NTSC_XAC3,
  MPEG_PS_PAL,
  MPEG_PS_PAL_XAC3,
  MPEG_TS_SD_NA,
  MPEG_TS_SD_NA_T,
  MPEG_TS_SD_NA_ISO,
  MPEG_TS_HD_NA,
  MPEG_TS_HD_NA_T,
  MPEG_TS_HD_NA_ISO,
  MPEG_TS_SD_EU,
  MPEG_TS_SD_EU_T,
  MPEG_TS_SD_EU_ISO,
  MPEG_TS_SD_KO,
  MPEG_TS_SD_KO_T,
  MPEG_TS_SD_KO_ISO,
  MPEG_TS_HD_KO,
  MPEG_TS_HD_KO_T,
  MPEG_TS_HD_KO_ISO,
  MPEG_TS_HD_KO_XAC3,
  MPEG_TS_HD_KO_XAC3_T,
  MPEG_TS_HD_KO_XAC3_ISO,
  MPEG_TS_HD_NA_XAC3,
  MPEG_TS_HD_NA_XAC3_T,
  MPEG_TS_HD_NA_XAC3_ISO,
  MPEG_TS_SD_KO_XAC3,
  MPEG_TS_SD_KO_XAC3_T,
  MPEG_TS_SD_KO_XAC3_ISO,
  MPEG_TS_SD_NA_XAC3,
  MPEG_TS_SD_NA_XAC3_T,
  MPEG_TS_SD_NA_XAC3_ISO,
  MPEG_TS_MP_LL_AAC,
  MPEG_TS_MP_LL_AAC_T,
  MPEG_TS_MP_LL_AAC_ISO,
  MPEG_ES_PAL,
  MPEG_ES_NTSC,
  MPEG_ES_PAL_XAC3,
  MPEG_ES_NTSC_XAC3,
  DIRECTV_TS_SD,
  MPEG_PS_HD_DTS,
  MPEG_PS_HD_DTSHD,
  MPEG_PS_HD_DTSHD_HRA,
  MPEG_PS_HD_DTSHD_MA,
  MPEG_PS_SD_DTS,
  MPEG_TS_DTS_ISO,
  MPEG_TS_DTS_T,
  MPEG_TS_DTSHD_HRA_ISO,
  MPEG_TS_DTSHD_HRA_T,
  MPEG_TS_DTSHD_MA_ISO,
  MPEG_TS_DTSHD_MA_T,
  MPEG_TS_HD_50_L2_ISO,
  MPEG_TS_HD_50_L2_T,
  MPEG_TS_HD_X_50_L2_T,
  MPEG_TS_HD_X_50_L2_ISO,
  MPEG_TS_HD_60_L2_ISO,
  MPEG_TS_HD_60_L2_T,
  MPEG_TS_HD_X_60_L2_T,
  MPEG_TS_HD_X_60_L2_ISO,
  MPEG_TS_HD_NA_MPEG1_L2_ISO,
  MPEG_TS_HD_NA_MPEG1_L2_T,
  MPEG_TS_JP_T,
  MPEG_TS_SD_50_AC3_T,
  MPEG_TS_SD_50_L2_T,
  MPEG_TS_SD_60_AC3_T,
  MPEG_TS_SD_60_L2_T,
  MPEG_TS_SD_EU_AC3_ISO,
  MPEG_TS_SD_EU_AC3_T,
  MPEG_TS_SD_JP_MPEG1_L2_T,
  MPEG_TS_SD_NA_MPEG1_L2_ISO,
  MPEG_TS_SD_NA_MPEG1_L2_T,
  MPEG_DIRECTV_SD_MPEG1_L2,
  MPEG_DIRECTV_SD_MPEG1_L2_T,
  MPEG_TS_NA_ISO,
  MPEG_TS_SD_DTS_ISO,
  MPEG_TS_HD_DTS_ISO,
  MPEG_TS_SD_EU_DTS_ISO,
  MPEG_TS_NA_3DFC_ISO,
  
  MPEG4_P2_MP4_SP_AAC,
  MPEG4_P2_MP4_SP_HEAAC,
  MPEG4_P2_MP4_SP_ATRAC3plus,
  MPEG4_P2_MP4_SP_AAC_LTP,
  MPEG4_P2_MP4_SP_L2_AAC,
  MPEG4_P2_MP4_SP_L2_AMR,
  MPEG4_P2_TS_SP_AAC,
  MPEG4_P2_TS_SP_AAC_T,
  MPEG4_P2_TS_SP_AAC_ISO,
  MPEG4_P2_TS_SP_MPEG1_L3,
  MPEG4_P2_TS_SP_MPEG1_L3_T,
  MPEG4_P2_TS_SP_MPEG1_L3_ISO,
  MPEG4_P2_TS_SP_AC3,
  MPEG4_P2_TS_SP_AC3_T,
  MPEG4_P2_TS_SP_AC3_ISO,
  MPEG4_P2_TS_SP_MPEG2_L2,
  MPEG4_P2_TS_SP_MPEG2_L2_T,
  MPEG4_P2_TS_SP_MPEG2_L2_ISO,
  MPEG4_P2_ASF_SP_G726,
  MPEG4_P2_MP4_SP_VGA_AAC,
  MPEG4_P2_MP4_SP_VGA_HEAAC,
  MPEG4_P2_MP4_ASP_AAC,
  MPEG4_P2_MP4_ASP_HEAAC,
  MPEG4_P2_MP4_ASP_HEAAC_MULT5,
  MPEG4_P2_MP4_ASP_ATRAC3plus,
  MPEG4_P2_TS_ASP_AAC,
  MPEG4_P2_TS_ASP_AAC_T,
  MPEG4_P2_TS_ASP_AAC_ISO,
  MPEG4_P2_TS_ASP_MPEG1_L3,
  MPEG4_P2_TS_ASP_MPEG1_L3_T,
  MPEG4_P2_TS_ASP_MPEG1_L3_ISO,
  MPEG4_P2_TS_ASP_AC3,
  MPEG4_P2_TS_ASP_AC3_T,
  MPEG4_P2_TS_ASP_AC3_ISO,
  MPEG4_P2_MP4_ASP_L5_SO_AAC,
  MPEG4_P2_MP4_ASP_L5_SO_HEAAC,
  MPEG4_P2_MP4_ASP_L5_SO_HEAAC_MULT5,
  MPEG4_P2_ASF_ASP_L5_SO_G726,
  MPEG4_P2_MP4_ASP_L4_SO_AAC,
  MPEG4_P2_MP4_ASP_L4_SO_HEAAC,
  MPEG4_P2_MP4_ASP_L4_SO_HEAAC_MULT5,
  MPEG4_P2_ASF_ASP_L4_SO_G726,
  MPEG4_H263_MP4_P0_L10_AAC,
  MPEG4_H263_MP4_P0_L10_AAC_LTP,
  MPEG4_H263_3GPP_P0_L10_AMR_WBplus,
  MPEG4_P2_TS_CO_AC3,
  MPEG4_P2_TS_CO_AC3_T,
  MPEG4_P2_TS_CO_AC3_ISO,
  MPEG4_P2_TS_CO_MPEG2_L2,
  MPEG4_P2_TS_CO_MPEG2_L2_T,
  MPEG4_P2_TS_CO_MPEG2_L2_ISO,
  MPEG4_P2_3GPP_SP_L0B_AAC,
  MPEG4_P2_3GPP_SP_L0B_AMR,
  MPEG4_H263_3GPP_P3_L10_AMR,
  MPEG4_H263_3GPP_P0_L10_AMR,
  MPEG4_H263_3GPP_P0_L45_AMR,
  MPEG4_H263_3GPP_P0_L45_AMR_WBplus,
  MPEG4_H263_MP4_P0_L45_HEAACv2_L2,
  MPEG4_P2_3GPP_SP_L0B_AMR_WBplus,
  MPEG4_P2_3GPP_SP_L3_AMR_WBplus,
  MPEG4_P2_MP4_NDSD,
  MPEG4_P2_MP4_SP_L0B_HEAACv2_L2,
  MPEG4_P2_MP4_SP_L3_HEAACv2_L2,
  MPEG4_P2_MP4_SP_L5_AAC,
  MPEG4_P2_MP4_SP_L6_AAC,
  MPEG4_P2_MP4_SP_L6_AAC_LTP,
  MPEG4_P2_MP4_SP_L6_HEAAC_L2,
  MPEG4_P2_MP4_SP_VGA_AAC_res,
  MPEG4_P2_MP4_SP_VGA_HEAAC_res,
  
  AVC_TS_MP_SD_AAC_MULT5,
  AVC_TS_MP_SD_AAC_MULT5_T,
  AVC_TS_MP_SD_AAC_MULT5_ISO,
  AVC_TS_MP_SD_HEAAC_L2,
  AVC_TS_MP_SD_HEAAC_L2_T,
  AVC_TS_MP_SD_HEAAC_L2_ISO,
  AVC_TS_MP_SD_MPEG1_L3,
  AVC_TS_MP_SD_MPEG1_L3_T,
  AVC_TS_MP_SD_MPEG1_L3_ISO,
  AVC_TS_MP_SD_AC3,
  AVC_TS_MP_SD_AC3_T,
  AVC_TS_MP_SD_AC3_ISO,
  AVC_TS_MP_SD_AAC_LTP,
  AVC_TS_MP_SD_AAC_LTP_T,
  AVC_TS_MP_SD_AAC_LTP_ISO,
  AVC_TS_MP_SD_AAC_LTP_MULT5,
  AVC_TS_MP_SD_AAC_LTP_MULT5_T,
  AVC_TS_MP_SD_AAC_LTP_MULT5_ISO,
  AVC_TS_MP_SD_AAC_LTP_MULT7,
  AVC_TS_MP_SD_AAC_LTP_MULT7_T,
  AVC_TS_MP_SD_AAC_LTP_MULT7_ISO,
  AVC_TS_MP_SD_BSAC,
  AVC_TS_MP_SD_BSAC_T,
  AVC_TS_MP_SD_BSAC_ISO,
  AVC_MP4_MP_SD_AAC_MULT5,
  AVC_MP4_MP_SD_HEAAC_L2,
  AVC_MP4_MP_SD_MPEG1_L3,
  AVC_MP4_MP_SD_AC3,
  AVC_MP4_MP_SD_AAC_LTP,
  AVC_MP4_MP_SD_AAC_LTP_MULT5,
  AVC_MP4_MP_SD_AAC_LTP_MULT7,
  AVC_MP4_MP_SD_ATRAC3plus,
  AVC_MP4_BL_L3L_SD_AAC,
  AVC_MP4_BL_L3L_SD_HEAAC,
  AVC_MP4_BL_L3_SD_AAC,
  AVC_MP4_MP_SD_BSAC,
  AVC_TS_BL_CIF30_AAC_MULT5,
  AVC_TS_BL_CIF30_AAC_MULT5_T,
  AVC_TS_BL_CIF30_AAC_MULT5_ISO,
  AVC_TS_BL_CIF30_HEAAC_L2,
  AVC_TS_BL_CIF30_HEAAC_L2_T,
  AVC_TS_BL_CIF30_HEAAC_L2_ISO,
  AVC_TS_BL_CIF30_MPEG1_L3,
  AVC_TS_BL_CIF30_MPEG1_L3_T,
  AVC_TS_BL_CIF30_MPEG1_L3_ISO,
  AVC_TS_BL_CIF30_AC3,
  AVC_TS_BL_CIF30_AC3_T,
  AVC_TS_BL_CIF30_AC3_ISO,
  AVC_TS_BL_CIF30_AAC_LTP,
  AVC_TS_BL_CIF30_AAC_LTP_T,
  AVC_TS_BL_CIF30_AAC_LTP_ISO,
  AVC_TS_BL_CIF30_AAC_LTP_MULT5,
  AVC_TS_BL_CIF30_AAC_LTP_MULT5_T,
  AVC_TS_BL_CIF30_AAC_LTP_MULT5_ISO,
  AVC_TS_BL_CIF30_AAC_940,
  AVC_TS_BL_CIF30_AAC_940_T,
  AVC_TS_BL_CIF30_AAC_940_ISO,
  AVC_MP4_BL_CIF30_AAC_MULT5,
  AVC_MP4_BL_CIF30_HEAAC_L2,
  AVC_MP4_BL_CIF30_MPEG1_L3,
  AVC_MP4_BL_CIF30_AC3,
  AVC_MP4_BL_CIF30_AAC_LTP,
  AVC_MP4_BL_CIF30_AAC_LTP_MULT5,
  AVC_MP4_BL_L2_CIF30_AAC,
  AVC_MP4_BL_CIF30_BSAC,
  AVC_MP4_BL_CIF30_BSAC_MULT5,
  AVC_MP4_BL_CIF15_HEAAC,
  AVC_MP4_BL_CIF15_AMR,
  AVC_TS_MP_HD_AAC_MULT5,
  AVC_TS_MP_HD_AAC_MULT5_T,
  AVC_TS_MP_HD_AAC_MULT5_ISO,
  AVC_TS_MP_HD_HEAAC_L2,
  AVC_TS_MP_HD_HEAAC_L2_T,
  AVC_TS_MP_HD_HEAAC_L2_ISO,
  AVC_TS_MP_HD_MPEG1_L3,
  AVC_TS_MP_HD_MPEG1_L3_T,
  AVC_TS_MP_HD_MPEG1_L3_ISO,
  AVC_TS_MP_HD_AC3,
  AVC_TS_MP_HD_AC3_T,
  AVC_TS_MP_HD_AC3_ISO,
  AVC_TS_MP_HD_AAC,
  AVC_TS_MP_HD_AAC_T,
  AVC_TS_MP_HD_AAC_ISO,
  AVC_TS_MP_HD_AAC_LTP,
  AVC_TS_MP_HD_AAC_LTP_T,
  AVC_TS_MP_HD_AAC_LTP_ISO,
  AVC_TS_MP_HD_AAC_LTP_MULT5,
  AVC_TS_MP_HD_AAC_LTP_MULT5_T,
  AVC_TS_MP_HD_AAC_LTP_MULT5_ISO,
  AVC_TS_MP_HD_AAC_LTP_MULT7,
  AVC_TS_MP_HD_AAC_LTP_MULT7_T,
  AVC_TS_MP_HD_AAC_LTP_MULT7_ISO,
  AVC_TS_BL_CIF15_AAC,
  AVC_TS_BL_CIF15_AAC_T,
  AVC_TS_BL_CIF15_AAC_ISO,
  AVC_TS_BL_CIF15_AAC_540,
  AVC_TS_BL_CIF15_AAC_540_T,
  AVC_TS_BL_CIF15_AAC_540_ISO,
  AVC_TS_BL_CIF15_AAC_LTP,
  AVC_TS_BL_CIF15_AAC_LTP_T,
  AVC_TS_BL_CIF15_AAC_LTP_ISO,
  AVC_TS_BL_CIF15_BSAC,
  AVC_TS_BL_CIF15_BSAC_T,
  AVC_TS_BL_CIF15_BSAC_ISO,
  AVC_MP4_BL_CIF15_AAC,
  AVC_MP4_BL_CIF15_AAC_520,
  AVC_MP4_BL_CIF15_AAC_LTP,
  AVC_MP4_BL_CIF15_AAC_LTP_520,
  AVC_MP4_BL_CIF15_BSAC,
  AVC_MP4_BL_L12_CIF15_HEAAC,
  AVC_MP4_BL_L1B_QCIF15_HEAAC,
  AVC_3GPP_BL_CIF30_AMR_WBplus,
  AVC_3GPP_BL_CIF15_AMR_WBplus,
  AVC_3GPP_BL_QCIF15_AAC,
  AVC_3GPP_BL_QCIF15_AAC_LTP,
  AVC_3GPP_BL_QCIF15_HEAAC,
  AVC_3GPP_BL_QCIF15_AMR_WBplus,
  AVC_3GPP_BL_QCIF15_AMR,
  AVC_3GPP_BL_CIF15_AMR_WBplus_res,
  AVC_3GPP_BL_CIF30_AMR_WBplus_res,
  AVC_3GPP_BL_L1B_QCIF15_AMR,
  AVC_3GPP_BL_L1B_QCIF15_AMR_WBplus,
  AVC_MP4_BL_CIF15_HEAACv2_L2,
  AVC_MP4_BL_CIF30_HEAACv2_L2,
  AVC_MP4_HP_HD_HEAACv2_L4,
  AVC_MP4_HP_SD_HEAACv2_L4,
  AVC_TS_HP_HD_HEAACv2_L4_ISO,
  AVC_TS_HP_HD_HEAACv2_L4_T,
  AVC_TS_HP_SD_HEAACv2_L4_ISO,
  AVC_TS_HP_SD_HEAACv2_L4_T,
  AVC_TS_HD_60_AC3,
  AVC_TS_HD_60_AC3_T,
  AVC_TS_HD_60_AC3_ISO,
  AVC_TS_HD_50_AC3,
  AVC_TS_HD_50_AC3_T,
  AVC_TS_HD_50_AC3_ISO,
  /* TODO */

  /*
  MP3 = 0,
  AAC_ISO_320 = 1,
  AAC_ISO = 2,
  AAC_MULT5_ISO = 3,
  MPEG1 = 4, */
  UNKNOWN
};

static std::string dlna_profile_names[] = {
  "JPEG_SM", 
  "MP3", "AAC_ISO_320", "AAC_ISO", "AAC_MULT5_ISO", "MPEG1",
  "UNKNOWN"
};

/**
 * @brief The MediaException class
 */
class MediaException : public std::exception {
public:

    static constexpr int FILE_EXCEPTION = 1; /** error with file */
    static constexpr int STREAM_EXCEPTION = 2; /** error with open media streams */
    static constexpr int CODEC_EXCEPTION = 3; /** error with open codec */

  explicit MediaException (int _code, std::string _what) throw() : _code(_code), _what(_what) {}
  virtual ~MediaException() throw() {}
  virtual const char* what() const throw() {
return _what.c_str();
  };
  int code() throw() {
return _code;
  }
private:
  int _code;
  std::string _what;
};

/**
 * @brief The AudioStream class
 */
class AudioStream {
public:
    /**
     * @brief AudioStream
     */
    AudioStream() : _bitrate(0), _bits_per_sample(0), _channels(0), _sample_frequency(0), 
		    _codec(CODEC::UNKNOWN), _dlna_profile(DLNA_PROFILE::UNKNOWN) {}
    /**
     * @brief bitrate
     * @return
     */
    int bitrate() const { return _bitrate; }
    /**
     * @brief bitrate
     * @param bitrate
     */
    void bitrate( int bitrate ) { _bitrate = bitrate; }
    /**
     * @brief bitsPerSample
     * @return
     */
    int bitsPerSample() const { return _bits_per_sample; }
    /**
     * @brief bitsPerSample
     * @param bits_per_sample
     */
    void bitsPerSample( int bits_per_sample ) { _bits_per_sample = bits_per_sample; }
    /**
     * @brief channels
     * @return
     */
    int channels() const { return _channels; }
    /**
     * @brief channels
     * @param channels
     */
    void channels( int channels ) { _channels = channels; }
    /**
     * @brief codec id
     * @return
     */
    CODEC codec() const { return _codec; }
    /**
     * @brief codec id
     * @param codec
     */
    void codec( CODEC codec ) { _codec = codec; }
    /**
     * @brief sampleFrequency
     * @return
     */
    int sampleFrequency() const { return _sample_frequency; }
    /**
     * @brief sampleFrequency
     * @param sample_frequency
     */
    void sampleFrequency( int sample_frequency ) { _sample_frequency = sample_frequency; }
    /**
     * @brief dlna profile
     * @return
     */
    DLNA_PROFILE dlnaProfile() const { return _dlna_profile; }
    /**
     * @brief dlna profile
     * @param dlna profile
     */
    void dlnaProfile( DLNA_PROFILE dlna_profile ) { _dlna_profile = dlna_profile; }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const AudioStream &obj) {
        os <<  "AudioStream (DLNA: " << dlna_profile_names[(int)obj._dlna_profile] << ")" << std::endl;
        os << " codec id: " << codec_names[(int)obj._codec] << std::endl;
        os << " channels: " << obj._channels << ", bitrate: " << obj._bitrate << ", bits_per_sample: " <<
              obj._bits_per_sample << ", frequency: " << obj._sample_frequency << std::endl;
        return os;
    }
private:
    int _bitrate, _bits_per_sample, _channels, _sample_frequency;
    CODEC _codec;
    DLNA_PROFILE _dlna_profile;
};
/**
 * @brief The VideoStream class
 */
class VideoStream {
public:
    /**
     * @brief VideoStream
     */
    VideoStream() : _width(0), _height(0), _bitrate(0), _codec(CODEC::UNKNOWN) {}
    /**
     * @brief bitrate
     * @return
     */
    int bitrate() const { return _bitrate; }
    /**
     * @brief bitrate
     * @param bitrate
     */
    void bitrate( int bitrate ) { _bitrate = bitrate; }
    /**
     * @brief codec type
     * @return
     */
    CODEC codec() const { return _codec; }
    /**
     * @brief codec type
     * @param codec
     */
    void codec( CODEC codec ) {  _codec = codec; }
    /**
     * @brief height
     * @return
     */
    int height() const { return _height; }
    /**
     * @brief height
     * @param height
     */
    void height( int height ) {  _height = height; }
    /**
     * @brief width
     * @return
     */
    int width() const { return _width; }
    /**
     * @brief width
     * @param width
     */
    void width( int width ) {  _width = width; }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const VideoStream &obj) {
        os <<  "VideoStream::" << std::endl;
        os << " codec id: " << codec_names[(int)obj._codec] << std::endl;
        os << " bitrate: " << obj._bitrate << ", width: " << obj._width << ", height: " << obj._height << std::endl;
        return os;
    }
private:
    int _width, _height, _bitrate;
    CODEC _codec;
};
/**
 * @brief The MediaFile class
 */
class MediaFile {
public:
    /**
     * @brief The meta tag names.
     */
    enum TAG { ARTIST, ALBUM, COMPOSER, DISC, TRACK, TITLE, YEAR, GENRE, COMMENT };
    /**
     * @brief MediaFile
     * @param filename
     */
    MediaFile( std::string filename ) : filename(filename), _duration(0) {}
    /**
     * @brief duration
     * @return
     */
    int duration() const { return _duration; }
    /**
     * @brief duration
     * @param duration
     */
    void duration( int duration ) { _duration = duration; }
    /**
     * @brief size
     * @return
     */
    int size() { return boost::filesystem::file_size( filename ); }
    /**
     * @brief name
     * @return
     */
    std::string name() {
        int start = 0;
        int end = filename.size();
        if( filename.find("/") != std::string::npos ) {
            start = filename.find_last_of("/") + 1;
        }
        if( filename.find(".", start ) != std::string::npos ) {
            end = filename.find_last_of(".");
        }
        return filename.substr(start, end - start);
    }
    /**
     * @brief getAudioStreams
     * @return
     */
    std::vector<AudioStream> getAudioStreams() const {
        return audio_streams;
    }
    /**
     * @brief getVideoStreams
     * @return
     */
    std::vector<VideoStream> getVideoStreams() const {
        return video_streams;
    }
    /**
     * @brief addTag
     * @param tag
     * @param value
     */
    void addTag( TAG tag, std::string value ) {
        tags[tag] = value;
    }
    /**
     * @brief getTag
     * @param tag
     * @return
     */
    std::string getTag( TAG tag ) {
        return tags[tag];
    }
    /**
     * @brief tagNames
     * @return
     */
    std::vector<TAG> tagNames() const {
        std::vector<TAG> result;
        for(auto & itrTag : tags ) {
            result.push_back( itrTag.first );
        }
        return result;
    }
    /**
     * @brief check if tags contains key.
     * @param name tag key
     * @return
     */
    bool hasTag( const TAG & key ) const {
        return tags.find( key ) != tags.end();
    }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os, const MediaFile &obj) {
        os <<  "MediaFile::" << std::endl;
        os << " duration: " <<  time_to_string( obj._duration ) << std::endl;
        os << "Meta Information: " << std::endl;
        for( auto & iter : obj.audio_streams ) std::cout << iter;
        for( auto & iter : obj.video_streams ) std::cout << iter;
        for( auto & iter : obj.tags ) {
            os << "\t" << iter.first << " = " << iter.second << std::endl;
        }
        return os;
    }
private:
    std::string filename;
    int _duration;
    std::vector<VideoStream> video_streams;
    std::vector<AudioStream> audio_streams;
    std::map<TAG, std::string > tags;
    void addVideoStream( VideoStream && video ) {
        video_streams.push_back( video );
    }
    void addAudioStream( AudioStream && audio ) {
        audio_streams.push_back( audio );
    }
    friend class MediaParser;
};

class MediaParser {
public:
    static MediaFile parseFile( std::string filename ) {

        MediaFile media_file( filename );

        AVFormatContext *fmt_ctx = NULL;
        AVCodecContext *input_codec_context = NULL;

        av_register_all();
        open_input_file(filename.c_str(), &fmt_ctx );
	
        /** Make sure that there is only one stream in the input file. */
        if ( fmt_ctx->nb_streams > 0 ) {
            AVDictionaryEntry *tag = NULL;
            media_file.duration( fmt_ctx->duration / AV_TIME_BASE ); //set the playlength
            for( unsigned int i=0; i<fmt_ctx->nb_streams; i++ ) { //TODO remove output
                std::cout << "input stream " << i << ", type: " << fmt_ctx->streams[i]->codec->codec_type << std::endl;
                switch( fmt_ctx->streams[i]->codec->codec_type ) {
                    case AVMEDIA_TYPE_UNKNOWN: std::cout << "AVMEDIA_TYPE_UNKNOWN" << std::endl; break;
                    case AVMEDIA_TYPE_VIDEO: {
			input_codec_context = fmt_ctx->streams[i]->codec;
			
			switch( input_codec_context->codec_id ) {
			  case AV_CODEC_ID_MPEG1VIDEO:
			    std::cout << "AV_CODEC_ID_MPEG1VIDEO" << std::endl;
			    break;
			case AV_CODEC_ID_MPEG2VIDEO:
			  std::cout << "AV_CODEC_ID_MPEG2VIDEO" << std::endl;
			  break;
			case AV_CODEC_ID_MPEG2TS:
			  std::cout << "AV_CODEC_ID_MPEG2TS" << std::endl;
			  break;
			case AV_CODEC_ID_MPEG4:
			  std::cout << "AV_CODEC_ID_MPEG4" << std::endl;
			  break;
			case AV_CODEC_ID_MPEG4SYSTEMS:
			  std::cout << "AV_CODEC_ID_MPEG4SYSTEMS" << std::endl;
			  break;
			case AV_CODEC_ID_H264:
			  std::cout << "AV_CODEC_ID_H264" << fmt_ctx->iformat->name << std::endl;
			  break;
			default: 
			  std::cout << "CODEC id not found:" << input_codec_context->codec_id << ", " << input_codec_context->codec_name << std::endl;
			};

                        VideoStream videofile;
                        videofile.width( input_codec_context->width );
                        videofile.height( input_codec_context->height );
                        videofile.codec( convertCodec(input_codec_context->codec_id) );
                        videofile.bitrate( input_codec_context->bit_rate );
                        media_file.addVideoStream( std::move( videofile ) );
                        avcodec_close(input_codec_context);
                        break;
                    }
                    case AVMEDIA_TYPE_AUDIO: {
			input_codec_context = fmt_ctx->streams[i]->codec;
                        AudioStream audiofile;
			switch( input_codec_context->codec_id ) {
			  case AV_CODEC_ID_MP3:
			    audiofile.dlnaProfile( DLNA_PROFILE::MP3 );
			    audiofile.codec( CODEC::MPEG3 );
			    break;
			  case AV_CODEC_ID_AAC:
			    if( input_codec_context->channels <= 2 && input_codec_context->bit_rate <= 320000 ) {
			      audiofile.dlnaProfile( DLNA_PROFILE::AAC_ISO_320 );
			      audiofile.codec( CODEC::AAC );
			    } else if( input_codec_context->channels <= 2 && input_codec_context->bit_rate <= 576000 ) {
			      audiofile.dlnaProfile( DLNA_PROFILE::AAC_ISO );
			      audiofile.codec( CODEC::AAC );
			    } else if( input_codec_context->channels <= 6 && input_codec_context->bit_rate <= 1440000 ) {
			      //TODO set profile audiofile.dlnaProfile( DLNA_PROFILE::AAC_MULT5_ISO );
			      audiofile.codec( CODEC::AAC );
			    } else {
			      std::cout << "UNKNOWN AAC CODEC: " << std::endl;
			    }			      
			    break;
		  
			    /* TODO
			    audiofile.dlnaProfile( DLNA_PROFILE::MP3 );
			    audiofile.codec( CODEC::MPEG3 );
			    break; */
			    
/*			  case CODEC_ID_AC3: 
			  case CODEC_ID_DTS:
			  case CODEC_ID_WMAV1:
			  case CODEC_ID_WMAV2:
			  case CODEC_ID_WMAPRO:
			  case CODEC_ID_MP2: 
			  case CODEC_ID_AMR_NB: */
			  default:
			    std::cout << "Unkown Codec:" << input_codec_context->codec_id << ":" << input_codec_context->codec_name << 
					 ", " << input_codec_context->codec_tag << ", " << input_codec_context->codec_type << std::endl;
			}
			
                        audiofile.bitrate( input_codec_context->bit_rate );
                        audiofile.bitsPerSample( input_codec_context->bits_per_raw_sample );
                        audiofile.channels( input_codec_context->channels );
                        audiofile.sampleFrequency( input_codec_context->sample_rate );
                        audiofile.codec( convertCodec(input_codec_context->codec_id) );
                        media_file.addAudioStream( std::move( audiofile ) );
                        avcodec_close(input_codec_context);
                        break;
                    }
                    case AVMEDIA_TYPE_DATA: std::cout << "AVMEDIA_TYPE_DATA" << std::endl; break;
                    case AVMEDIA_TYPE_SUBTITLE: std::cout << "AVMEDIA_TYPE_SUBTITLE" << std::endl; break;
                    case AVMEDIA_TYPE_ATTACHMENT: {

                        std::cout << "AVMEDIA_TYPE_ATTACHMENT" << std::endl;
                        open_input_decoder(i, &fmt_ctx, &input_codec_context);
                        std::cout << "AVMEDIA_TYPE_ATTACHMENT, codec_id:" << input_codec_context->codec_id << std::endl;
                        if( input_codec_context->codec_id == AV_CODEC_ID_MJPEG ) {
                            std::cout << "code is AV_CODEC_ID_MJPEG:" << std::endl;
                        } else if( input_codec_context->codec_id == AV_CODEC_ID_MJPEGB ) {
                            std::cout << "code is AV_CODEC_ID_MJPEGB:" << std::endl;
                        } else if( input_codec_context->codec_id == AV_CODEC_ID_LJPEG ) {
                            std::cout << "code is AV_CODEC_ID_LJPEG:" << std::endl;
                        }
                        break;
                    }
                    case AVMEDIA_TYPE_NB: std::cout << "AVMEDIA_TYPE_NB" << std::endl; break;
                    default: std::cout << "UNKNOWN" << std::endl;
                }
             }

            //add the meta information
            while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
                std::cout << tag->key << ":"<< tag->value << std::endl;
                if(std::strcmp(tag->key, "TITLE")==0 ||
                        std::strcmp(tag->key, "title")==0) {
                    media_file.addTag( MediaFile::TITLE, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "ARTIST")==0 ||
                          std::strcmp(tag->key, "artist")==0 ||
                          std::strcmp(tag->key, "album_artist")==0 ||
                          std::strcmp(tag->key, "ALBUM ARTIST")==0 ||
                          std::strcmp(tag->key, "ENSEMBLE")==0 ||
                          std::strcmp(tag->key, "PERFORMER")==0) {
                    media_file.addTag( MediaFile::ARTIST, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "COMPOSER")==0) {
                    media_file.addTag( MediaFile::COMPOSER, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "ALBUM")==0 ||
                          std::strcmp(tag->key, "album")==0) {
                    media_file.addTag( MediaFile::ALBUM, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "DATE")==0 ||
                          std::strcmp(tag->key, "date")==0 ||
                          std::strcmp(tag->key, "RELEASEYEAR")==0) {
                    media_file.addTag( MediaFile::YEAR, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "track")==0) {
                    media_file.addTag( MediaFile::TRACK, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "GENRE")==0 ||
                          std::strcmp(tag->key, "genre")==0) {
                    media_file.addTag( MediaFile::GENRE, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "disc")==0) {
                    media_file.addTag( MediaFile::DISC, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "COMMENT")==0 ||
                          std::strcmp(tag->key, "DESCRIPTION")==0) {
                    media_file.addTag( MediaFile::COMMENT, boost::trim_copy( std::string( tag->value ) ) );
                } else if(std::strcmp(tag->key, "TRACKTOTAL")!=0 &&
                          std::strcmp(tag->key, "TLEN")!=0 &&
                          std::strcmp(tag->key, "encoded_by")!=0 &&
                          std::strcmp(tag->key, "encoder")!=0 &&
                          std::strcmp(tag->key, "ENCODER")!=0 &&
                          std::strcmp(tag->key, "HDTRACKS")!=0 &&
                          std::strcmp(tag->key, "SOURCE")!=0 &&
                          std::strcmp(tag->key, "CATALOG #")!=0 &&
                          std::strcmp(tag->key, "ALBUM DYNAMIC RANGE")!=0 &&
                          std::strcmp(tag->key, "DYNAMIC RANGE")!=0 &&
                          std::strcmp(tag->key, "CHANNELS")!=0 &&
                          std::strcmp(tag->key, "URL")!=0 &&
                          std::strcmp(tag->key, "TOTALTRACKS")!=0 &&
                          std::strcmp(tag->key, "language")!=0 &&
                          std::strcmp(tag->key, "ITUNES_CDDB_1")!=0 &&
                          std::strcmp(tag->key, "ENGINEER")!=0 &&
                          std::strcmp(tag->key, "ORIGINATOR")!=0 &&
                          std::strcmp(tag->key, "DISCTOTAL")!=0 &&
                          std::strcmp(tag->key, "DISCOGS_")!=0 &&
                          std::strcmp(tag->key, "REPLAYGAIN_")!=0 &&
                          std::strcmp(tag->key, "Unknown Frame:")!=0 ) {
                    std::cout << " * " << tag->key << " = " << tag->value << std::endl;
                }
            }

        } else {
            std::cout << "no audio stram found:'" << filename << "'" << std::endl;
            avformat_close_input(&fmt_ctx);
            throw MediaException( MediaException::STREAM_EXCEPTION, "no stream in the input file found.");
        }

        avformat_close_input(&fmt_ctx);
        return media_file;
    }

private:
    static std::string get_error_text(const int error) {
        static char error_buffer[255];
        av_strerror(error, error_buffer, sizeof(error_buffer));
        return std::string( error_buffer );
    }

    /** TODO get the media information **/
//    static void getMediaInformation() {

//    }

    static CODEC convertCodec( const AVCodecID & codec_id ) {
      if( codec_id == AV_CODEC_ID_MP1 )
	return CODEC::MPEG1; 
      if( codec_id == AV_CODEC_ID_MP2 )
	return CODEC::MPEG2; 
      if( codec_id == AV_CODEC_ID_MP3 )
	return CODEC::MPEG3; 
      if( codec_id == AV_CODEC_ID_FLAC )
	return CODEC::FLAC; 
      if( codec_id == AV_CODEC_ID_AAC )
	return CODEC::AAC; 
      if( codec_id == AV_CODEC_ID_AC3 )
	return CODEC::AC3; 
      if( codec_id == AV_CODEC_ID_APE )
	return CODEC::APE; 
      if( codec_id == AV_CODEC_ID_MUSEPACK7 ||
	  codec_id == AV_CODEC_ID_MUSEPACK8 )
	return CODEC::MPC; 
      if( codec_id == AV_CODEC_ID_VORBIS )
	return CODEC::VORBIS; 
      
      if( codec_id == AV_CODEC_ID_H264 )
	return CODEC::H264; 

      std::cerr << "unknown codec id:" << codec_id;
      return CODEC::UNKNOWN;
    }
    
    /** Open an input file and the required decoder. */
    static void open_input_file(const char *filename,
                               AVFormatContext **input_format_context ) {
        int error;

        /** Open the input file to read from it. */
        if ((error = avformat_open_input(input_format_context, filename, NULL, NULL)) < 0) {
            throw MediaException( MediaException::FILE_EXCEPTION, "unable to open media file: " + std::string( filename ) + " " + get_error_text(error));
        }

        /** Get information on the input file (number of streams etc.). */
        if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
            avformat_close_input(input_format_context);
            throw MediaException( MediaException::STREAM_EXCEPTION, "Could not find stream info: " + get_error_text(error));
        }

    }


    /** TODO remove! Open an input file and the required decoder. */
    static void open_input_decoder(int stream_index,
                               AVFormatContext **input_format_context,
                               AVCodecContext **input_codec_context ) {
        AVCodec *input_codec;

        /** Find a decoder for the audio stream. */
        if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[stream_index]->codec->codec_id))) {
            throw MediaException( MediaException::CODEC_EXCEPTION, "Could not find input codec: " );
        }

        /** Open the decoder for the audio stream to use it later. */
        int error;
        if ((error = avcodec_open2((*input_format_context)->streams[stream_index]->codec, input_codec, NULL)) < 0) {
            avformat_close_input(input_format_context);
            throw MediaException( MediaException::FILE_EXCEPTION, "Could not open input codec: " + get_error_text(error) );
        }

        /** Save the decoder context for easier access later. */
        *input_codec_context = (*input_format_context)->streams[stream_index]->codec;
    }
};
}}
#endif // VIDEO_H

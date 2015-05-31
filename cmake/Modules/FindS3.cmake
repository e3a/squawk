# - Try to find S3, libs3 library
# Once done this will define
#
#  S3_FOUND - system has Flac
#  S3_INCLUDE_DIR - the Flac include directory
#  S3_LIBRARIES - Link these to use Flac
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(S3_INCLUDE_DIR AND S3_LIBRARIES)
    # Already in cache, be silent
    set(S3_FIND_QUIETLY TRUE)
endif(S3_INCLUDE_DIR AND S3_LIBRARIES)

FIND_PATH(S3_INCLUDE_DIR libs3.h)

FIND_LIBRARY(S3_LIBRARIES NAMES s3 )

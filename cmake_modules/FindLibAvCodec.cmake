# Copyright (c) 2009, Whispersoft s.r.l.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# * Neither the name of Whispersoft s.r.l. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# Finds AvCodec library
#
#  AvCodec_INCLUDE_DIR - where to find avcodec.h, etc.
#  AvCodec_LIBRARIES   - List of libraries when using AvCodec.
#  AvCodec_FOUND       - True if AvCodec found.
#

find_path(AvCodec_INCLUDE_DIR1 libavcodec/avcodec.h
  /opt/local/include
  /usr/local/include
  /usr/include
  )
find_path(AvCodec_INCLUDE_DIR2 ffmpeg/avcodec.h
  /opt/local/include
  /usr/local/include
  /usr/include
  )
if ( AvCodec_INCLUDE_DIR1 )
  set (AV_CODEC_INCLUDE_FILE libavcodec/avcodec.h)
  set (AvCodec_INCLUDE_DIR ${AvCodec_INCLUDE_DIR1})
endif  ( AvCodec_INCLUDE_DIR1 )
if ( AvCodec_INCLUDE_DIR2 )
  set (AV_CODEC_INCLUDE_FILE ffmpeg/avcodec.h)
  set (AvCodec_INCLUDE_DIR ${AvCodec_INCLUDE_DIR2})
endif  ( AvCodec_INCLUDE_DIR2 )

set(AvCodec_NAMES avcodec)
find_library(AvCodec_LIBRARY
  NAMES ${AvCodec_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
)

if (AvCodec_INCLUDE_DIR AND AvCodec_LIBRARY)
   set(AvCodec_FOUND TRUE)
   set( AvCodec_LIBRARIES ${AvCodec_LIBRARY} )
else (AvCodec_INCLUDE_DIR AND AvCodec_LIBRARY)
   set(AvCodec_FOUND FALSE)
   set(AvCodec_LIBRARIES)
endif (AvCodec_INCLUDE_DIR AND AvCodec_LIBRARY)

if (AvCodec_FOUND)
   if (NOT AvCodec_FIND_QUIETLY)
      message(STATUS "Found AvCodec: ${AvCodec_LIBRARY}")
   endif (NOT AvCodec_FIND_QUIETLY)
else (AvCodec_FOUND)
   if (AvCodec_FIND_REQUIRED)
      message(STATUS "Looked for AvCodec libraries named ${AvCodec_NAMES}.")
      message(STATUS "Include file detected: [${AvCodec_INCLUDE_DIR}].")
      message(STATUS "Lib file detected: [${AvCodec_LIBRARY}].")
      message(FATAL_ERROR "=========> Could NOT find AvCodec library")
   endif (AvCodec_FIND_REQUIRED)
endif (AvCodec_FOUND)

mark_as_advanced(
  AvCodec_LIBRARY
  AvCodec_INCLUDE_DIR
  )

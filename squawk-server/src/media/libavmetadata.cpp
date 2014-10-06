/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2014  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "libavmetadata.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

namespace squawk {
namespace media {

void LibAvMetadata::parse(struct squawk::media::Audiofile * audiofile, const std::string * file) {
    AVFormatContext *fmt_ctx = NULL;
    AVDictionaryEntry *tag = NULL;
    int ret;

  av_register_all();
  if ((ret = avformat_open_input(&fmt_ctx, file.c_str(), NULL, NULL)))
      return; //ret;

  while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
      printf("%s=%s\n", tag->key, tag->value);
  }

  avformat_close_input(&fmt_ctx);
  return; // 0;  
}
}}

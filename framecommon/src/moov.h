/*******************************************************************************
 moov.h

 moov - A library for splitting Quicktime/MPEG4 files.
 http://h264.code-shop.com

 Copyright (C) 2007 CodeShop B.V.

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
******************************************************************************/ 

#pragma once
#include <stdint.h>
typedef long long __int64;
//typedef long long int64_t;
#define UINT64_MAX 0xffffffffffffffffULL 


unsigned int moov_seek(unsigned char* moov_data,
                              uint64_t*  moov_size,
                              float start_time,
                              float end_time,
                              uint64_t* mdat_start,
                              uint64_t* mdat_size,
                              uint64_t offset);


/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_UTIL_HH
#define SCGRAPH_UTIL_HH

#include <string>

#include <sys/types.h>

/** these functions read big endian data.
    they advance the data pointer by the 
    appropriate amount  */
int32_t read_int32_t (unsigned char **data);
int16_t read_int16_t (unsigned char **data);
int8_t  read_int8_t (unsigned char **data);
float   read_float (unsigned char **data);
std::string read_string (unsigned char **data);



#endif

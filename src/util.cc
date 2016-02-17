/* Copyright 2006 Florian Paul Schmidt */

#include "util.h"

int32_t read_int32_t (unsigned char **data)
{
	int32_t tmp;
	tmp = (*data)[0] << 24 | (*data)[1] << 16 | (*data)[2] << 8 | (*data)[3];
	*data += 4;
	return tmp;
}

float read_float (unsigned char **data)
{

	union foo {float f; int32_t i;} tmp;
	tmp.i = ((*data)[0] << 24) | ((*data)[1] << 16) | ((*data)[2] << 8) | (*data)[3];
	*data += 4;
	return tmp.f;
}

int16_t read_int16_t (unsigned char **data)
{
	int16_t tmp;
	tmp = (*data)[0] << 8 | (*data)[1];
	*data += 2;
	return tmp;
}

int8_t read_int8_t (unsigned char **data)
{
	char tmp = **data;
	*data += 1;
	return tmp;
}

std::string read_string (unsigned char **data) 
{
	std::string tmp;

	unsigned char *tmp_data = *data;
	unsigned char length = *tmp_data;

	for (int i = 0; i < (int)length; ++i)
	{
		tmp += *(tmp_data+1+i);
	}

	*data += (1 + length);

	return tmp;
}

#include "scgcolor.h"

void scgColor::scale_alpha(float factor)
{
	this[3] *= factor;
}

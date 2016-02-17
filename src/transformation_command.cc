#include "transformation_command.h"

#include "transformation_command_visitor.h"

void Translation::accept (TransformationCommandVisitor *v) 
{
	v->visitTranslation (this);
}

void Rotation::accept (TransformationCommandVisitor *v)
{
	v->visitRotation (this);
}

void Scale::accept (TransformationCommandVisitor *v)
{
	v->visitScale (this);
}

void Linear::accept (TransformationCommandVisitor *v)
{
	v->visitLinear (this);
}



void Translation::acceptConst (TransformationCommandVisitor *v) const 
{
	v->visitTranslationConst (this);
}

void Rotation::acceptConst (TransformationCommandVisitor *v) const
{
	v->visitRotationConst (this);
}

void Scale::acceptConst (TransformationCommandVisitor *v) const
{
	v->visitScaleConst (this);
}

void Linear::acceptConst (TransformationCommandVisitor *v) const
{
	v->visitLinearConst (this);
}




#ifndef SCGRAPH_TRANSFORMATION_COMMAND
#define SCGRAPH_TRANSFORMATION_COMMAND

#include "vector_3d.h"
#include "matrix.h"
#include "cow_ptr.h"

class TransformationCommandVisitor;

/** purely virtual base struct */
struct TransformationCommand : public DeepCopyable
{
	virtual ~TransformationCommand () {};

	virtual void accept (TransformationCommandVisitor *v) = 0;
	virtual void acceptConst (TransformationCommandVisitor *v) const = 0;
};

struct Translation : public TransformationCommand
{
	Vector3D _translation_vector;

	void accept (TransformationCommandVisitor *v);
	virtual Translation* deepCopy() {
		return new Translation(*this);
	}
void acceptConst (TransformationCommandVisitor *v) const;
};

struct Rotation : public TransformationCommand
{
	Vector3D _rotation_vector;
	float    _rotation_angle;

	void accept (TransformationCommandVisitor *v);

	virtual Rotation* deepCopy() {
		return new Rotation(*this);
	}

	void acceptConst (TransformationCommandVisitor *v) const;
};

struct Scale : public TransformationCommand
{
	Vector3D _scaling_vector;

	void accept (TransformationCommandVisitor *v);
	void acceptConst (TransformationCommandVisitor *v) const;

	virtual Scale* deepCopy() {
		return new Scale(*this);
	}

};

struct Linear : public TransformationCommand
{
	Matrix _transformation_matrix;

	void accept (TransformationCommandVisitor *v);
	void acceptConst (TransformationCommandVisitor *v) const;

	virtual Linear* deepCopy() {
		return new Linear(*this);
	}
};

#endif

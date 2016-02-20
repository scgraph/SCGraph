#ifndef SCGRAPH_TRANSFORMATION_COMMAND
#define SCGRAPH_TRANSFORMATION_COMMAND

#include "ofVec3f.h"
#include "ofMatrix4x4.h"
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
	ofVec3f _translation_vector;

	void accept (TransformationCommandVisitor *v);
	virtual Translation* deepCopy() {
		return new Translation(*this);
	}
void acceptConst (TransformationCommandVisitor *v) const;
};

struct Rotation : public TransformationCommand
{
	ofVec3f _rotation_vector;
	float    _rotation_angle;

	void accept (TransformationCommandVisitor *v);

	virtual Rotation* deepCopy() {
		return new Rotation(*this);
	}

	void acceptConst (TransformationCommandVisitor *v) const;
};

struct Scale : public TransformationCommand
{
	ofVec3f _scaling_vector;

	void accept (TransformationCommandVisitor *v);
	void acceptConst (TransformationCommandVisitor *v) const;

	virtual Scale* deepCopy() {
		return new Scale(*this);
	}

};

struct Linear : public TransformationCommand
{
	ofMatrix4x4 _transformation_matrix;

	void accept (TransformationCommandVisitor *v);
	void acceptConst (TransformationCommandVisitor *v) const;

	virtual Linear* deepCopy() {
		return new Linear(*this);
	}
};

#endif

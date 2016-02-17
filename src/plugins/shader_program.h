#ifndef SCGRAOH_SHADER_PROGRAM_HH
#define SCGRAOH_SHADER_PROGRAM_HH

#include "../unit.h"
#include "../graphics_visitor.h"
#include "../graphics.h"

class GShaderProgram : public GUnit
{
	cow_ptr<ShaderProgram> _l;

	public:
		GShaderProgram ();
		~GShaderProgram ();

		virtual void process_g (double delta_t);
};


class GShaderUniform : public GUnit
{
	cow_ptr<ShaderUniform> _l;
	std::vector<float> _values;

	bool _first_time;

	public:
		GShaderUniform ();
		~GShaderUniform ();

		virtual void process_g (double delta_t);
};

#endif

#ifndef SCGRAPH_SHADER_POOL_HH
#define SCGRAPH_SHADER_POOL_HH

#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>
#include <map>

#include <QtCore/QObject>

#include "glew.h"

struct ShaderPool : public QObject
{
	Q_OBJECT

	public:
		struct ShaderProgram {
			enum eShaderType {VERTEX, FRAGMENT};
	
			/**	A shader is just a pair of a string (the source) and
				a type flag denoting whether it's a fragment or a
				vertex shader
			*/
			std::vector<std::pair<std::string, int> > _shaders;
	
			/**	A shader program can declare uniform variables.
				This list holds the names of variables that should
				be exposed to the GShaderProgram unit.
			*/
			std::vector<std::string> _uniforms;
		
			ShaderProgram() {
		
			}
		
			ShaderProgram(const std::vector<std::pair<std::string, int> > &shaders) :
				_shaders(shaders)
			{
			}
		};

		/**
			A map of index to ShaderProgram.
		*/
		typedef
		std::map<unsigned int, boost::shared_ptr<ShaderProgram> > shader_programs_map_t;
		shader_programs_map_t _shader_programs;
	
	#if 0
		void add_shader_program (const std::string &filename);
	
		void tokenize_path (const std::string &path, void (ShaderPool::*function)(const std::string &directory));
	
		void traverse_directory (const std::string &directory, void (ShaderPool::*function)(const std::string &filename));
	
		void scan_directory (const std::string &directory);
	#endif
	
		virtual ~ShaderPool ();
		static ShaderPool *get_instance ();
	
		boost::shared_ptr<ShaderProgram> get_shader_program (unsigned int index);
		void add_shader_program (unsigned int index, boost::shared_ptr<ShaderProgram> program);
		void clear_shader_programs();
	
#if 0
	void add_uniform_float (const std::string &name);
	void clear_uniform_floats();
	unsigned int get_number_of_uniform_floats();
	GLint get_uniform_float_location(unsigned int index);
#endif

	signals:
		void shader_program_added (unsigned int index);
		void shader_programs_changed ();

	private:
		ShaderPool ();
		static ShaderPool *_instance;

};


#endif

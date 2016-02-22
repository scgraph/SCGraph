#ifndef SCGRAPH_QT_GL_RENDERER
#define SCGRAPH_QT_GL_RENDERER

#define GL_GLEXT_PROTOTYPES 1

//#include "glew.h"

#include "../unit.h"
#include "../graphics_visitor.h"
#include "../texture_pool.h"

#include "../string_pool.h"

#include <unordered_map>
#include <list>

#include "../shader_pool.h"

#include "../transformation_command_visitor.h"

#include "ofAppGLFWWindow.h"
#include "../scgcolor.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofMaterial.h"
#include "ofEventUtils.h"
#include "ofEvent.h"


/*

class Recorder
{

	unsigned long int _current_frame;
	QString _path;
	QString _format;

 public:
	Recorder ();
	void writeFrame (QImage img);

	void nextFrame (QImage img);

};
*/
class GLRenderer;
/*
class GLRenderWidget //: public QGLWidget
{
	//Q_OBJECT

	GLRenderer *_renderer;
// TODO
	void mousePressEvent (QMouseEvent *event);
	void mouseReleaseEvent (QMouseEvent *event);
	void mouseMoveEvent (QMouseEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void keyPressEvent (QKeyEvent *event);
	void keyReleaseEvent (QKeyEvent *event);

	GLenum _shader_program;

	bool _recording;

	//Recorder _recorder;

	// TODO GLEWContext _glew_context;

	public:
	// TODO GLEWContext *getGlewContext();

	public:
		// TODO GLRenderWidget (QWidget *parent, GLRenderer *renderer);
		void resizeGL (int w, int h);
		void paintGL ();
		void initializeGL ();
		void makeScreenshot ();
		bool toggleRecording ();
};

class GLMainWindow //: public QMainWindow
{
	//Q_OBJECT

	GLRenderer *_renderer;

	public:
		GLMainWindow (GLRenderer *renderer);

	// TODO public slots:
	// TODO	void closeEvent (QCloseEvent *e);
};
*/
#define SCGRAPH_QT_GL_RENDERER_LOOK_AT_INDEX          7
#define SCGRAPH_QT_GL_RENDERER_CLEAR_COLOR_INDEX (7 + 9)
#define SCGRAPH_QT_GL_RENDERER_FOG_INDEX     (7 + 9 + 3)

#define SCGRAPH_QT_GL_RENDERER_DEFAULT_WIDTH        640
#define SCGRAPH_QT_GL_RENDERER_DEFAULT_HEIGHT       480
#define SCGRAPH_QT_GL_RENDERER_MAXMAX_FEEDBACK_FRAMES       1024


void writeImage (ofImage img);

class GLApp : public ofBaseApp
{
    
public:
    GLRenderer *_renderer;

    GLApp (GLRenderer *renderer);
    //~GLApp ();
    void setup();
    void update();
    void draw();
    void exit();
    
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    // TODO void mouseDoubleClickEvent (QMouseEvent *event);
    void keyPressed(int key);
    void keyReleased(int key);
    

    void makeScreenshot();

};

class GLRenderer : public GUnit,
	public GraphicsVisitor, 
	public TransformationCommandVisitor
{
	enum // keep track of these pesty indeces
	{
		CLEAR_MODE = 1,
		CLEAR_COLOR = 2,
		LIGHTING = 6,
		CULLING = 7,
		TRANSPARENCY = 8,
		PERSPECTIVE = 9,
		FOV = 10,
		NEAR_PLANE = 11,
		FAR_PLANE = 12,
		EYE = 13,
		CENTER = 16,
		UP = 19,
		FOG = 22,
		FOG_MODE = 23,
		FOG_DENSITY = 24,
		FOG_START = 25,
		FOG_END = 26,
		FOG_NICENESS = 27,
		FOG_COLOR = 28,
		TEXTURING = 32,
		MAXFEEDBACKFRAMES = 33,
	};
/*
	GLMainWindow     *_main_window;
	GLRenderWidget   *_gl_widget;
*/
	bool _ready;
    
    shared_ptr<ofAppBaseWindow> _main_window;
    
    shared_ptr<GLApp> _main_app;
    

	/* the transformation matrix for user input */
	ofMatrix4x4            _transformation_matrix;

	ofMatrix4x4            _rotation_matrix;
    
    ofMatrix4x4            _look_at_matrix;
    
    ofCamera          _camera;

	bool              _show_info;
	bool              _show_help;

	bool              _full_screen;
	bool              _window_decorations;

	bool              _mouse_down;
	int               _cur_mouse_x;
	int               _cur_mouse_y;
	int               _ren_mouse_x;
	int               _ren_mouse_y;


	bool              _shift_key_down;

	bool              _up_key_down;
	bool              _down_key_down;
	bool              _right_key_down;
	bool              _left_key_down;

	/* for every cycle called with key pressed, increase by certain 
	   amount divided by time.
	   for every cycle called with key up decrease. */
	double            _forward;
	double            _sideward;
	double            _upward;

	float             _rot_x;
	float             _rot_y;

	string _window_title;

    std::array<string, 3> directions, axisnames;
    std::array<int, 3> offsets;
    
    string helptext;
	unsigned int _feedback;
	unsigned int _fbcounter;
	unsigned int _max_feedback_frames;

	std::map <int, int> 
	                  _gl_light_indexes;

	std::vector<GLuint>
                      _texture_handles;

    std::unordered_map<uint32_t, GLuint>
                      _tmp_texture_handles;

	std::vector<ofTexture>
                      _past_frame_handles;

	// first is program handle, second is vector of shader handles
	typedef 
	std::map<unsigned int, std::pair<GLenum, std::vector<GLenum> > >
                      shader_programs_map_t;

	shader_programs_map_t _shader_programs;

	// this is 0 in the case that no shader program is active
	unsigned int _current_shader_program;

	// a place to store the uniform locations per program
	typedef
	std::map<unsigned int, std::vector<GLint> > shader_uniforms_map_t;

	shader_uniforms_map_t _shader_uniforms;


	void draw_face (const Face &face);

	void upload_texture (uint32_t id, bool samep);

	ofMaterial do_material (const Material &material);
	void do_light (const Light &light);

	void do_face (const Face& face);

	void change_feedback_frames ();
	void clear_feedback_frames ();

	public:
		GLRenderer ();
		~GLRenderer ();
    
  		virtual void process_g (double delta_t);
		virtual void really_process_g (double delta_t);

		virtual void visitLightConst (const Light *l);
		virtual void visitGeometryConst (const Geometry *g);
		virtual void visitTransformationConst (const Transformation *t);
		virtual void visitLightingConst (const Lighting *l);
		virtual void visitBlendingConst (const Blending *b);
		virtual void visitCullingConst (const Culling *c);

		virtual void visitTextConst (const Text *c);

		virtual void visitShaderProgramConst (const ShaderProgram *c);
		virtual void visitShaderUniformConst (const ShaderUniform *c);

		virtual void visitTranslationConst (const Translation *t);
		virtual void visitRotationConst (const Rotation *r);
		virtual void visitScaleConst (const Scale *s);
		virtual void visitLinearConst (const Linear *l);

		void appendToWindowTitle (string toAppend);

		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseDragged(int x, int y, int button);
		// TODO void mouseDoubleClickEvent (QMouseEvent *event);
		void keyPressed(int key);
		void keyReleased(int key);

		GLuint upload_texture(boost::shared_ptr<Texture> const & texture);

		inline void upload_texture(boost::shared_ptr<Texture> const & texture, 
								   GLuint handle);

		void set_done_action (int done_action);

		void setup_texture (size_t index);

		void setup_textures ();

		void clear_textures ();

		void compile_and_link_shader_program(unsigned int index, ShaderPool::ShaderProgram *s);

		void setup_shader_programs();
		void clear_shader_program(unsigned int intex);
		void clear_shader_programs();


		double _delta_t;

	// TODO public slots:
		void init_textures ();
		void change_tmp_texture (uint32_t id, bool samep);
        void change_tmp_texture_pair (std::pair<uint32_t, bool> & p);
		void delete_tmp_texture (uint32_t index);
    
        void change_texture (unsigned int & index);
        void delete_texture(uint32_t & id);

	// TODO public slots:
		void add_shader_program (unsigned int & index);
        void change_shader_programs ();
		void change_shader_programs_evt (unsigned int & index);
};

#endif

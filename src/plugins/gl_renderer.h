#ifndef SCGRAPH_QT_GL_RENDERER
#define SCGRAPH_QT_GL_RENDERER

#define GL_GLEXT_PROTOTYPES 1

#include "glew.h"

#include "../unit.h"
#include "../graphics_visitor.h"
#include "../texture_pool.h"

#include <map>

#include "../shader_pool.h"

#include "../transformation_command_visitor.h"

#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QLabel>

#include <QtGui/QImage>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtCore/QFuture>
#include <QtCore/QtConcurrentRun>		

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFramebufferObject>

void writeImage (QImage img);

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


class GLRenderer;

class GLRenderWidget : public QGLWidget
{
	Q_OBJECT

	GLRenderer *_renderer;

	void mousePressEvent (QMouseEvent *event);
	void mouseReleaseEvent (QMouseEvent *event);
	void mouseMoveEvent (QMouseEvent *event);
	void keyPressEvent (QKeyEvent *event);
	void keyReleaseEvent (QKeyEvent *event);

	GLenum _shader_program;

	bool _recording;

	Recorder _recorder;

	GLEWContext _glew_context;

	public:
		GLEWContext *getGlewContext();

	public:
		GLRenderWidget (QWidget *parent, GLRenderer *renderer);
		void resizeGL (int w, int h);
		void paintGL ();
		void initializeGL ();
		void makeScreenshot ();
		bool toggleRecording ();
};

class GLMainWindow : public QMainWindow
{
	Q_OBJECT

	GLRenderer *_renderer;

	public:
		GLMainWindow (GLRenderer *renderer);

	public slots:
		void closeEvent (QCloseEvent *e);
};

#define SCGRAPH_QT_GL_RENDERER_LOOK_AT_INDEX          7
#define SCGRAPH_QT_GL_RENDERER_CLEAR_COLOR_INDEX (7 + 9)
#define SCGRAPH_QT_GL_RENDERER_FOG_INDEX     (7 + 9 + 3)

#define SCGRAPH_QT_GL_RENDERER_DEFAULT_WIDTH        640
#define SCGRAPH_QT_GL_RENDERER_DEFAULT_HEIGHT       480
#define SCGRAPH_QT_GL_RENDERER_MAXMAX_FEEDBACK_FRAMES       1024

class GLRenderer : public QObject, public GUnit, public GraphicsVisitor, public TransformationCommandVisitor
{
	Q_OBJECT

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
		MAXFEEDBACKFRAMES = 33
	};

	GLMainWindow     *_main_window;
	GLRenderWidget   *_gl_widget;

	bool _ready;

	/* the transformation matrix for user input */
	Matrix            _transformation_matrix;

	Matrix            _rotation_matrix;

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

	QString _window_title;

	QStringList directions, axisnames, helptexts;
	QList<int> offsets;
	QFont font;

	unsigned int _feedback;
	unsigned int _fbcounter;
	unsigned int _max_feedback_frames;

	void draw_face (const Face &face);

	std::map <int, int> 
	                  _gl_light_indexes;

	std::vector<GLuint>
                      _texture_handles;

	std::vector<GLuint>
                      _past_frame_handles;
	boost::shared_ptr<Texture> _lastFrame;

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


	void do_material (const Material &material);
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

		virtual void visitShaderProgramConst (const ShaderProgram *c);
		virtual void visitShaderUniformConst (const ShaderUniform *c);

		virtual void visitTranslationConst (const Translation *t);
		virtual void visitRotationConst (const Rotation *r);
		virtual void visitScaleConst (const Scale *s);
		virtual void visitLinearConst (const Linear *l);

		void appendToWindowTitle (QString toAppend);

		void mousePressEvent (QMouseEvent *event);
		void mouseReleaseEvent (QMouseEvent *event);
		void mouseMoveEvent (QMouseEvent *event);
		void keyPressEvent (QKeyEvent *event);
		void keyReleaseEvent (QKeyEvent *event);

		void set_done_action (int done_action);

		void setup_texture (size_t index);

		void setup_textures ();

		void clear_textures ();

		void compile_and_link_shader_program(unsigned int index, ShaderPool::ShaderProgram *s);

		void setup_shader_programs();
		void clear_shader_program(unsigned int intex);
		void clear_shader_programs();
		double _delta_t;

	public slots:
		void add_texture (unsigned int index);
		void change_textures ();

	public slots:
		void add_shader_program (unsigned int index);
		void change_shader_programs ();
};

#endif

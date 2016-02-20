#define GL_GLEXT_PROTOTYPES 1

#include "gl_renderer.h"

#include "../texture_pool.h"
#include "../options.h"
#include "../scgraph.h"



#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <cstdio>
#include <sstream>
#include <stdexcept>


//#include <QtOpenGL/QGLFormat>

/** 
	A global variable, but don't blame me, blame the 
	GLEW authors
*/
/*
GLEWContext *glewContext;

extern "C" {
	GLEWContext* glewGetContext() {
		return glewContext;
	}
}
 */
/*

Recorder::Recorder () :
	_current_frame (0)
{
	Options *options = Options::get_instance();
	_path = QString::fromStdString(options->_recording_path);
	_path.append("/recording-");
	
	QDateTime datetime = QDateTime::currentDateTime();
	_path.append(datetime.toString("yyyyMMdd-hhmmss-zzz-"));
	_path.append(QString("%1").arg(options->_graphics_rate, 0, 10));
	//_path.append(QString("%1").arg((double) (1.0/ScGraph::get_instance()->_delta_t), 0, 10));
	_path.append("fps-frame-");
	
	_format = "jpg";
}	


void Recorder::writeFrame (QImage img)
{
	try	{
		QString tmp = QString(_path);
		tmp.append(QString("%1").arg(_current_frame, 8, 10, QChar('0')));
		tmp.append(".");
		tmp.append(_format);
	
		// set quality to 95/100
		img.save(tmp, 0, 95);
	}
        catch (const std::exception& ex) {
            std::cout << "[Recorder]: " << ex.what() << std::endl;
	}
}


void Recorder::nextFrame (QImage img)
{
	QFuture<void> future = QtConcurrent::run(this,
											 &Recorder::writeFrame,
											 img);
	// proxy function to increase frames
	_current_frame++;
}


void writeImage (QImage img)
{
	try	{
		Options *options = Options::get_instance();
		QString path = QString::fromStdString(options->_recording_path);
		path.append("/screenshot-");

		QDateTime datetime = QDateTime::currentDateTime();
		path.append(datetime.toString("yyyyMMdd-hhmmss-zzz"));

		path.append(".png");

		img.save(path);

		std::cout << "[Screenshot] saved to " << path.toStdString() << std::endl;
	}
        catch (const std::exception& ex) {
            std::cout << "[Screenshot]: " << ex.what() << std::endl;
	}
}
*/
/*
GLRenderWidget::GLRenderWidget (QWidget *parent, GLRenderer *renderer) :
	QGLWidget (parent),
	_renderer (renderer),
	_recording (false)
{
	_glew_context = GLEWContext();

	setMouseTracking (true);
	setFocusPolicy (Qt::StrongFocus);

	boost::shared_ptr<Texture> _lastFrame(new Texture (1024, 1024, 4, true));
	//setAutoBufferSwap(false);
}


void GLRenderWidget::resizeGL (int w, int h)
{
	makeCurrent();
	glViewport(0, 0, (GLint)w, (GLint)h);
   	glClear(GL_COLOR_BUFFER_BIT);
	swapBuffers();
	glViewport(0, 0, (GLint)w, (GLint)h);
   	glClear(GL_COLOR_BUFFER_BIT);
}


void GLRenderWidget::paintGL ()
{
	// makeCurrent ();
	_renderer->really_process_g(_renderer->_delta_t);

	if(_recording)
		_recorder.nextFrame(grabFrameBuffer());
}

void GLRenderWidget::makeScreenshot ()
{
	QFuture<void> future = QtConcurrent::run(writeImage,
											 grabFrameBuffer());
}

bool GLRenderWidget::toggleRecording ()
{
	if(_recording) {
		_recording = false;
		_renderer->appendToWindowTitle("");
		std::cout << "[Recorder] stopped" << std::endl;
	}
	else {
		_recording = true;
		_recorder = Recorder();
		_renderer->appendToWindowTitle(" - [Recording]");
		std::cout << "[Recorder] started" << std::endl;
	}

	return _recording;
}

void GLRenderWidget::keyReleaseEvent (QKeyEvent *event)
{
	_renderer->keyReleaseEvent (event);
}

GLEWContext *GLRenderWidget::getGlewContext() 
{ 
	return &_glew_context; 
}

*/
/*
GLMainWindow::GLMainWindow ()
{

}


void GLMainWindow::closeEvent (QCloseEvent *event)
{
	_renderer->set_done_action (2);

	((QEvent*)event)->ignore ();
}

*/


GLApp::GLApp(GLRenderer *renderer) :
_renderer(renderer)
{
}

void GLApp::draw() {
    // TODO: dirty hack
    if(_renderer) {
        ScGraph* sc = ScGraph::get_instance();
        sc->lock_for_read();
        //std::cout << "[GLApp]: drawing" << std::endl;
        _renderer->really_process_g(0.1);
        //std::cout << "[GLApp]: drawing done" << std::endl;
        sc->unlock();
    }
}

void GLApp::setup() {
    /*
    
     
    //_glew_context = glewGetContext();
    // TODO glewContext = getGlewContext();
    
    /*GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
     */
    ofSetWindowShape (SCGRAPH_QT_GL_RENDERER_DEFAULT_WIDTH,
                      SCGRAPH_QT_GL_RENDERER_DEFAULT_HEIGHT);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glEnable(GL_BLEND);
#ifdef HAVE_SHADERS
    if (!GLEW_ARB_vertex_program)
    {
        std::cout << "[GGLRenderer]: Warning: vertex program extension missing" << std::endl;
    }
    
    if (!GLEW_ARB_fragment_program)
    {
        std::cout << "[GGLRenderer]: Warning: frament program extension missing" << std::endl;
    }
    
    if (!GLEW_ARB_shader_objects)
    {
        std::cout << "[GGLRenderer]: Warning: shader objects extension missing" << std::endl;
    }
#endif
    _renderer->init_textures();
    
#ifdef HAVE_SHADERS
    _renderer->change_shader_programs();
#endif
    //_shader_program = glCreateProgramObjectARB();
    
}

void GLApp::update() {
    
}

void GLApp::mousePressed(int x, int y, int button)
{
    _renderer->mousePressed(x, y, button);
}
void GLApp::mouseReleased(int x, int y, int button)
{
    _renderer->mouseReleased(x, y, button);
}
void GLApp::mouseDragged(int x, int y, int button)
{
    _renderer->mouseDragged(x, y, button);
}
void GLApp::keyPressed(int key)
{
    _renderer->keyPressed(key);
}
void GLApp::keyReleased(int key)
{
    _renderer->keyReleased(key);
}



GLRenderer::GLRenderer () :
	_ready (false),
	_transformation_matrix (ofMatrix4x4()),
	_rotation_matrix (ofMatrix4x4()),
    _look_at_matrix (ofMatrix4x4()),
	_show_info (false),
	_show_help (false),
	_full_screen (false),
	_window_decorations (true),
	_mouse_down (false),
	_cur_mouse_x(0),
	_cur_mouse_y(0),
	_ren_mouse_x(0),
	_ren_mouse_y(0),
	_shift_key_down (false),
	_up_key_down (false),
	_down_key_down (false),
	_right_key_down (false),
	_left_key_down (false),
	_forward (0),
	_sideward (0),
	_upward (0),
	_rot_x (0),
	_rot_y (0),
	_window_title("[ScGraph]: GGLRenderer - Press F1 for help"),
	// TODO font (QFont()),
	_feedback (0),
	_fbcounter (0),
	_max_feedback_frames (SCGRAPH_QT_GL_RENDERER_MAXMAX_FEEDBACK_FRAMES + 1),
	_current_shader_program (0),
	_delta_t(0.1)
{
	_transformation_matrix.makeIdentityMatrix();
	_rotation_matrix.makeIdentityMatrix();

	_gl_light_indexes[0] = GL_LIGHT0;
	_gl_light_indexes[1] = GL_LIGHT1;
	_gl_light_indexes[2] = GL_LIGHT2;
	_gl_light_indexes[3] = GL_LIGHT3;
	_gl_light_indexes[4] = GL_LIGHT4;
	_gl_light_indexes[5] = GL_LIGHT5;
	_gl_light_indexes[6] = GL_LIGHT6;
	_gl_light_indexes[7] = GL_LIGHT7;

	std::cout << "[GLRenderer]: constructor" << std::endl;

    /* TODO
	// set up display texts
	font.setPixelSize(10);

	directions << "eye" << "center" << "up";

	axisnames << "x" << "y" << "z";

	offsets.append(EYE);
	offsets.append(CENTER);
	offsets.append(UP);

	helptexts << "F1 or H - this help"
			  << "Clicking the little X - kill the node containing this GLRenderer"
			  << "R - reset view"		
			  << "I - show info"
			  << "F or double click - toggle fullscreen"
			  << "S - screenshot"
			  << "M - toggle recording"
			  << "UPARROW - forward"
			  << "DOWNARROW - backward"
			  << "RIGHTARROW - right"
			  << "LEFTARROW - left"
			  << "SHIFT-UPARROW - up"
			  << "SHIFT-DOWNARROW - down";
     */
    
    helptext = std::string("F1 or H - this help\n")
    + "Clicking the little X - kill the node containing this GLRenderer\n"
    + "R - reset view\n"
    + "I - show info\n"
    + "F or double click - toggle fullscreen\n"
    + "S - screenshot\n"
    + "M - toggle recording\n"
    + "UPARROW - forward\n"
    + "DOWNARROW - backward\n"
    + "RIGHTARROW - right\n"
    + "LEFTARROW - left\n"
    + "SHIFT-UPARROW - up\n"
    + "SHIFT-DOWNARROW - down\n";

	//TexturePool *texture_pool = TexturePool::get_instance ();
/* TODO
	connect (texture_pool, 
			 SIGNAL (texture_changed(unsigned int)), 
			 this, 
			 SLOT(change_texture(unsigned int)), 
			 Qt::QueuedConnection);

	connect (texture_pool, 
			 SIGNAL (change_tmp_texture(uint32_t, bool)), 
			 this, 
			 SLOT(change_tmp_texture(uint32_t, bool)), 
			 Qt::QueuedConnection);

	connect (texture_pool, 
			 SIGNAL (delete_texture(uint32_t)), 
			 this, 
			 SLOT(delete_tmp_texture(uint32_t)),
			 Qt::QueuedConnection);

	connect (ShaderPool::get_instance (), 
			 SIGNAL (shader_programs_changed()), 
			 this, 
			 SLOT(change_shader_programs()), 
			 Qt::QueuedConnection);
*/
	//	_gl_widget->makeCurrent();

#if 0
	change_textures ();

	change_shader_programs();
#endif
    //_ready = true;
}


void GLRenderer::compile_and_link_shader_program(unsigned int index, ShaderPool::ShaderProgram *s) {
#ifdef HAVE_SHADERS
	std::cout << "[GGLRenderer]: Compiling and linking shader program: " << index << std::endl;
	_gl_widget->makeCurrent();

	GLenum my_program;
	GLenum my_shader;

	std::pair<GLenum, std::vector<GLenum> > shader_entry;

	// Create Shader And Program Objects
	my_program = glCreateProgramObjectARB();
	shader_entry.first = my_program;

	for (size_t i = 0; i < s->_shaders.size(); ++i)
	{
		if (s->_shaders[i].second == ShaderPool::ShaderProgram::VERTEX)
		{
			my_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		}
		else {
			my_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		}

		char *source = new char[s->_shaders[i].first.length() + 1];
		strcpy(source, s->_shaders[i].first.c_str());
		std::cout << "Source: " << std::endl << source << std::endl;
		glShaderSourceARB(my_shader, 1, (const GLcharARB **)&source, NULL);
		delete[] source;

		glCompileShaderARB(my_shader);
		glAttachObjectARB(my_program, my_shader);

		shader_entry.second.push_back(my_shader);
	}	
	// Link The Program Object
	glLinkProgramARB(my_program);

	char log[100000];
	GLsizei length;
	glGetInfoLogARB(my_program, 100000, &length, (GLcharARB *)log);

	std::cout << "[GGLRenderer]: Shader log:" << std::endl << log << std::endl << "[GGLRenderer]: Shader log end." << std::endl;

#if 0

	glGetInfoLogARB(my_program, 100000, &length, (GLcharARB *)log);

	std::cout << "Shader log:" << std::endl << log << std::endl << "Shader log end." << std::endl;
#endif

	glUseProgramObjectARB(my_program);

	_shader_programs[index] = shader_entry;

	// ok, find the locations of all uniforms referenced in the shader
	std::vector<GLint> locations;
	for (size_t i = 0; i < s->_uniforms.size(); ++i) {
		GLint loc = glGetUniformLocationARB(my_program, (const GLchar*)s->_uniforms[i].c_str());
		if (loc == -1) {
			std::cout << "[GLRenderer]: Error getting uniform location!" << std::endl;
		}
		locations.push_back(loc);
	}
	if(s->_uniforms.size() > 0) 
		_shader_uniforms[index] = locations;


	glGetInfoLogARB(my_program, 100000, &length, (GLcharARB *)log);

	std::cout << "[GGLRenderer]: Shader log:" << std::endl 
			  << log << std::endl 
			  << "[GGLRenderer]: Shader log end." << std::endl;

	// deactivate new shader
	glUseProgramObjectARB(0);
#endif
}

void GLRenderer::clear_shader_program(unsigned int index) {
#ifdef HAVE_SHADERS
	for (size_t j = 0; j < _shader_programs[index].second.size(); ++j)
		glDeleteObjectARB(_shader_programs[index].second[j]);

	glDeleteObjectARB(_shader_programs[index].first);
#endif
}

void GLRenderer::setup_shader_programs() {
#ifdef HAVE_SHADERS
	ShaderPool *p = ShaderPool::get_instance();

	for (ShaderPool::shader_programs_map_t::iterator it = p->_shader_programs.begin(); it != p->_shader_programs.end(); ++it)
	{
		compile_and_link_shader_program(it->first, it->second.get());
	}
#endif
}

void GLRenderer::clear_shader_programs() {
#ifdef HAVE_SHADERS
	_gl_widget->makeCurrent();

	for (shader_programs_map_t::iterator it = _shader_programs.begin(); it != _shader_programs.end(); ++it)
	{
		clear_shader_program(it->first);
	}

	_shader_programs.clear();
#endif
}

void GLRenderer::change_shader_programs () {
#ifdef HAVE_SHADERS
	clear_shader_programs();
	setup_shader_programs();
#endif
}

void GLRenderer::add_shader_program (unsigned int index) {
	// GLenum my_program = glCreateProgramObjectARB();
}


void GLRenderer::setup_texture (size_t index)
{
	//_gl_widget->makeCurrent();
	std::cout << "setup_texture" << std::endl;

	// TODO glBindTexture (GL_TEXTURE_2D, _texture_handles[index]);
}


inline void GLRenderer::upload_texture(boost::shared_ptr<Texture> const & texture, GLuint handle) {
	glBindTexture (GL_TEXTURE_2D, handle);

	GLenum color_format;
	if(texture->_channels == 4)
		color_format = GL_RGBA;
	else
		color_format = GL_RGB;

	// use texture proxy to test if we can load this texture 
	glTexImage2D (GL_PROXY_TEXTURE_2D,
				  0, // level
				  texture->_channels, // internal format
				  texture->_width,
				  texture->_height,
				  0, // border
				  color_format,
				  GL_UNSIGNED_BYTE,
				  0);

	GLint width;

	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);

	if (width == 0) { // Can't use that texture 
		std::cout << "[TexturePool]: Warning: Can't upload texture " 
				  << handle << ". Proxy call to glTexImage2D failed" 
				  << std::endl;
	}
	else {
		glTexImage2D(
					 GL_TEXTURE_2D,
					 0, // level
					 texture->_channels, // internal format
					 texture->_width,
					 texture->_height,
					 0, // border
					 color_format,
					 GL_UNSIGNED_BYTE,
					 texture->_data);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}

GLuint GLRenderer::upload_texture(boost::shared_ptr<Texture> const & texture)
{
	GLuint tmp;
	glGenTextures (1, &tmp);
	upload_texture(texture, tmp);
	return tmp;
}

void GLRenderer::upload_texture(uint32_t id, bool samep) {

	TexturePool *texture_pool = TexturePool::get_instance ();
	if(texture_pool->_tmp_textures.count(id) > 0) {
		boost::shared_ptr<Texture> t = texture_pool->_tmp_textures.at(id);

		if(samep) {
			upload_texture(t, _tmp_texture_handles.at(id));
		}
		else {
			//std::cout << "loading frame" << std::endl;
			// TODO: is this necessary? :
            _tmp_texture_handles.insert({id, (GLuint) 0});
			GLuint tmp = upload_texture(t);
			_tmp_texture_handles.at(id) = tmp;
		}
		texture_pool->loaded_tmp_texture(id);
	}
	else {
		std::cout << "[GLRenderer] No such frame at ID " << id << std::endl;
	}
}

void GLRenderer::clear_textures ()
{
	//_main_window->makeCurrent();

	// we make everything new here :)
	glDeleteTextures (_texture_handles.size(), &_texture_handles[0]);
	
	_texture_handles.clear ();
}

void GLRenderer::delete_texture (GLuint handle) {
	// TODO _gl_widget->makeCurrent();
	glDeleteTextures(1, &handle);
}

void GLRenderer::change_texture (unsigned int index) {
	while(_texture_handles.size() <= index)
		_texture_handles.push_back(-1);

	if(_texture_handles[index] != -1)
		delete_texture(_texture_handles[index]);

	TexturePool *texture_pool = TexturePool::get_instance ();

	boost::optional<boost::shared_ptr<AbstractTexture> > t = texture_pool->get_texture(index);
	if(t && !((*t)->isVideo())) {
		std::cout << "tex: " << (*t)->_texture << std::endl;
		_texture_handles[index] = upload_texture((*t)->_texture);		
	}
}

void GLRenderer::change_tmp_texture(uint32_t id, bool samep) {
	// std::cout << "[GLRenderer] changing tmp texture " << id << std::endl;
    
	if(_tmp_texture_handles.count(id) > 0 && (!samep)) {
		delete_texture(_tmp_texture_handles.at(id));
	}
	upload_texture(id, samep);
}

void GLRenderer::delete_tmp_texture(uint32_t id) {
	if(_tmp_texture_handles.count(id) > 0)
        delete_texture(_tmp_texture_handles.at(id));
}

void GLRenderer::init_textures () {
	//_main_window->makeCurrent();
	clear_textures ();

	TexturePool *texture_pool = TexturePool::get_instance ();

	for (size_t i = 0; i < texture_pool->get_number_of_textures (); ++i) {
		_texture_handles.push_back(-1);
		
		boost::optional<boost::shared_ptr<AbstractTexture> > t = texture_pool->get_texture(i);
		if(t && !(*t)->isVideo()) 
			_texture_handles[i] = upload_texture((*t)->_texture);		
	}
	
    for( auto i = texture_pool->_tmp_textures.begin(); i != texture_pool->_tmp_textures.end(); ++i)
        upload_texture(i->first, false);
}



void GLRenderer::change_feedback_frames ()
{
	//_main_window->makeCurrent();

	// TODO texture size?

	_max_feedback_frames = std::max<unsigned int>(1, 
												  std::min<unsigned int>((unsigned int) *_control_ins[MAXFEEDBACKFRAMES], 
																		 SCGRAPH_QT_GL_RENDERER_MAXMAX_FEEDBACK_FRAMES));

	// empty data to initialize the texture
	std::vector<GLubyte> emptyData(1024 * 1024 * 4, 0);
	
	for (size_t i = 0; i < _max_feedback_frames; i++) {
		_past_frame_handles.push_back(0);
		glGenTextures(1, &_past_frame_handles[i]);
		glBindTexture(GL_TEXTURE_2D, _past_frame_handles[i]);

		glTexSubImage2D(GL_TEXTURE_2D, 
						0, 0, 0, 1024, 1024, 
						GL_RGBA, GL_UNSIGNED_BYTE, &emptyData[0]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}


void GLRenderer::clear_feedback_frames ()
{
	_main_window->makeCurrent();

	glDeleteTextures (_past_frame_handles.size(), &_past_frame_handles[0]);

	_past_frame_handles.clear ();
}

GLRenderer::~GLRenderer ()
{
    _main_app->_renderer = NULL;
	std::cout << "[GLRenderer]: destructor" << std::endl;
    std::cout << "[GLRenderer]: done" << std::endl;
	clear_textures ();

    for( auto i = _tmp_texture_handles.begin(); i != _tmp_texture_handles.end(); ++i)
		glDeleteTextures(1, &i->second);

	_tmp_texture_handles.clear();
    
	clear_feedback_frames ();

    _main_window->setWindowShouldClose();
}

void GLRenderer::do_face (const Face& face)
{
    ofSetColor(face._face_color);
    
	/*std::cout << face._texture_coordinates.size () << " " <<
	 *_control_ins[TEXTURING] << " " << face._texture_index <<
	 std::endl; */

	if((*_control_ins[TEXTURING] > 0.5) && (face._texture_coordinates.size () > 0)) {
		if (face._colors.size () > 0) {
			//std::cout << "1" << std::endl;
			for (size_t i = 0; i < face._vertices.size (); ++i)	{
                ofSetColor(face._colors[i]);
				glNormal3fv (face._normals[i].getPtr());
				glTexCoord2fv (face._texture_coordinates[i].getPtr());
				glVertex3fv (face._vertices[i].getPtr());
			}
		}
		else {
			//std::cout << "2" << std::endl;
			for (size_t i = 0; i < face._vertices.size (); ++i)
				{
					glNormal3fv (face._normals[i].getPtr());
					glTexCoord2fv (face._texture_coordinates[i].getPtr());
					glVertex3fv (face._vertices[i].getPtr());
				}
		}
	}
	else if (face._colors.size () > 0 && (face._texture_coordinates.empty() || (*_control_ins[TEXTURING] < 0.5))) {
		//std::cout << "3" << std::endl;
		for (size_t i = 0; i < face._vertices.size (); ++i)
			{
                ofSetColor(face._colors[i]);
				glNormal3fv (face._normals[i].getPtr());
				glVertex3fv (face._vertices[i].getPtr());
			}
	}
	else {
		//std::cout << "4" << std::endl;
		for (size_t i = 0; i < face._vertices.size (); ++i)
			{
				glNormal3fv (face._normals[i].getPtr());
				glVertex3fv (face._vertices[i].getPtr());
			}
	}
}


void GLRenderer::draw_face (const Face &face) {
	TexturePool *texture_pool = TexturePool::get_instance ();

	if (face._render_mode == WIREFRAME) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glLineWidth (face._thickness);
	}
	else {
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	// if lighting is enabled at this point, then select the material
    ofMaterial ma;
	if (*_control_ins[LIGHTING] > 0.5)
		ma = do_material (face._material);

	if (*_control_ins[TEXTURING] > 0.5 && face._texture_coordinates.size () > 0) {

		if(face._texture_index >= 0 		
		   && face._texture_index < _texture_handles.size()) {
			glEnable (GL_TEXTURE_2D);

			if((*texture_pool->get_texture(face._texture_index))->isVideo() == true) {
				if(_tmp_texture_handles.count(face._frame_id) > 0) {
					glBindTexture (GL_TEXTURE_2D,
                                   // TODO: had default value, is it needed?
								   _tmp_texture_handles.at(face._frame_id));
				}
			}
			else if(_texture_handles[face._texture_index] != -1)
				glBindTexture (GL_TEXTURE_2D, 
							   _texture_handles[face._texture_index]);
		}
		else {
			if(face._texture_index < 0)	{
				_feedback = std::min<unsigned int>(
												   face._texture_index * -1, 
												   _max_feedback_frames);
				//std::cout << face._texture_index << std::endl;
				glEnable (GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 
							  _past_frame_handles[
												  (_max_feedback_frames 
												   - _feedback 
												   + _fbcounter) 
												  % _max_feedback_frames]);
			}
			else {
				glDisable (GL_TEXTURE_2D);
			}
		}
	}
	else 
		{
			glDisable (GL_TEXTURE_2D);
			// std::cout << "disable" << std::endl;
		}

	// TODO: fix per vertex color for lines

	switch (face._geometry_type)
		{
		case Face::POINTS:
			if (*_control_ins[LIGHTING] > 0.5)
				glDisable (GL_LIGHTING);

			glPointSize (face._thickness);
			glBegin (GL_POINTS);
                ofSetColor(face._face_color);


			for (size_t i = 0; i < face._vertices.size (); ++i)
                glVertex3fv(face._vertices[i].getPtr());

			glEnd ();

			glPointSize (1.0);

			if (*_control_ins[LIGHTING] > 0.5)
				glEnable (GL_LIGHTING);
			break;

		case Face::LINES:
			if (*_control_ins[LIGHTING] > 0.5)
				glDisable (GL_LIGHTING);

			glLineWidth (face._thickness);

			glBegin (GL_LINES);

                ofSetColor(face._face_color);
                
            for (size_t i = 0; i < face._vertices.size (); ++i)
				glVertex3fv(face._vertices[i].getPtr());

			glEnd ();

			if (*_control_ins[LIGHTING] > 0.5)
				glEnable (GL_LIGHTING);
			break;

		case Face::LINE_STRIP:
			if (*_control_ins[LIGHTING] > 0.5)
				glDisable (GL_LIGHTING);

			glLineWidth (face._thickness);

			glBegin (GL_LINE_STRIP);
                ofSetColor(face._face_color);

			for (size_t i = 0; i < face._vertices.size (); ++i)
				glVertex3fv(face._vertices[i].getPtr());

			glEnd ();

			if (*_control_ins[LIGHTING] > 0.5)
				glEnable (GL_LIGHTING);
			break;

		case Face::LINE_LOOP:
			if (*_control_ins[LIGHTING] > 0.5)
				glDisable (GL_LIGHTING);

			glLineWidth (face._thickness);

			glBegin (GL_LINE_LOOP);

            ofSetColor(face._face_color);

			for (size_t i = 0; i < face._vertices.size (); ++i)
                glVertex3fv(face._vertices[i].getPtr());

			glEnd ();

			if (*_control_ins[LIGHTING] > 0.5)
				glEnable (GL_LIGHTING);
			break;

		case Face::TRIANGLES:
			glBegin (GL_TRIANGLES);

			do_face (face);

			glEnd ();
			break;

		case Face::QUADS:
			glBegin (GL_QUADS);

			do_face (face);

			glEnd ();
			break;

		case Face::TRIANGLE_STRIP:
			glBegin (GL_TRIANGLE_STRIP);

			do_face (face);

			glEnd ();
			break;

		case Face::TRIANGLE_FAN:
			glBegin (GL_TRIANGLE_FAN);

			do_face (face);

			glEnd ();
			break;

		case Face::QUAD_STRIP:
			glBegin (GL_QUAD_STRIP);

			do_face (face);

			glEnd ();
			break;

		case Face::POLYGON:
			glBegin (GL_POLYGON);

			do_face (face);

			glEnd ();
			break;

		default:

			break;
		}
    
    if (*_control_ins[LIGHTING] > 0.5)
        ma.end();

	glEnd ();

	glDisable (GL_TEXTURE_2D);
}

ofMaterial GLRenderer::do_material (const Material &material)
{
	// set material for face
    ofMaterial ma;
    ma.setSpecularColor(material._specular_reflection);
    ma.setDiffuseColor(material._diffuse_reflection);
    ma.setAmbientColor(material._ambient_reflection);
    ma.setEmissiveColor(material._emissive_color);
    ma.setShininess(material._shinyness);
    ma.begin();
    return ma;
    /*
	glMaterialfv (GL_FRONT, GL_SPECULAR, material._specular_reflection);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, material._diffuse_reflection);
	glMaterialfv (GL_FRONT, GL_AMBIENT, material._ambient_reflection);
	glMaterialfv (GL_FRONT, GL_EMISSION, material._emissive_color);
	glMaterialfv (GL_FRONT, GL_SHININESS, &material._shinyness);*/
}

void GLRenderer::do_light (const Light &light)
{
	int index = _gl_light_indexes[light._index];
    
    //ofLight _light;
    
	if (!light._on)
	{
        //_light.disable();
		glDisable (index);
		return;
	}

    glEnable (index);
    //_light.enable();

    //_light.setPosition(&(light._position._c[0]));
	glLightfv(index, GL_POSITION, &(light._position._c[0]));
    glLightfv(index, GL_SPOT_DIRECTION, light._spot_direction.getPtr());
    // TODO: not very nice
	glLightfv(index, GL_AMBIENT, ofVec4f(light._ambient_color[0], light._ambient_color[1], light._ambient_color[2], light._ambient_color[3]).getPtr());
	// std::cout << "am: " << light._ambient_color._c[0] << " " <<	light._ambient_color._c[1] << " " << light._ambient_color._c[2] << std::endl;
	glLightfv(index, GL_DIFFUSE, ofVec4f(light._diffuse_color[0], light._diffuse_color[1], light._diffuse_color[2], light._diffuse_color[3]).getPtr());
	glLightfv(index, GL_SPECULAR, ofVec4f(light._specular_color[0], light._specular_color[1], light._specular_color[2], light._specular_color[3]).getPtr());

	glLightf(index, GL_SPOT_EXPONENT, light._spot_exponent);
	glLightf(index, GL_SPOT_CUTOFF, light._spot_cutoff);

	glLightf(index, GL_CONSTANT_ATTENUATION, light._constant_attenuation);
	glLightf(index, GL_LINEAR_ATTENUATION, light._linear_attenuation);
	glLightf(index, GL_QUADRATIC_ATTENUATION, light._quadratic_attenuation);

}

void GLRenderer::visitLightConst (const Light *l)
{
	do_light (*l);
}

void GLRenderer::visitShaderProgramConst (const ShaderProgram *s)
{
#ifdef HAVE_SHADERS
	if (s->_on)
	{
		glUseProgramObjectARB(_shader_programs[s->_index].first);
		_current_shader_program = s->_index;
	}
	else
	{
		glUseProgramObjectARB(0);
		_current_shader_program = 0;
	}
#endif
}

void GLRenderer::visitShaderUniformConst (const ShaderUniform *s)
{
#ifdef HAVE_SHADERS
	_gl_widget->makeCurrent();

	//std::cout << "current shader program index: " << _current_shader_program << " uniform index: " << s->_uniform_index << std::endl;

	// TODO: Make more efficient..
	// std::cout << "values.size(): " << s->_values.size() << " uniform index: " << s->_uniform_index << " first value: " << s->_values[0] <<  std::endl;
	if (_shader_uniforms.find(_current_shader_program) 
		== _shader_uniforms.end()) {
		// std::cout << "[GGLRenderer] No uniform at index " << s->_uniform_index 
		//		  << " for shader " << _current_shader_program << "!" << std::endl;
		return;
	}

	switch(s->_values.size()) {
		case 1:
			glUniform1fARB(
				_shader_uniforms[_current_shader_program][s->_uniform_index], 
				s->_values[0]
			);
		break;

		case 2:
			glUniform2fvARB(
				_shader_uniforms[_current_shader_program][s->_uniform_index], 
				1,
				&s->_values[0]
			);
		break;

		case 3:
			glUniform3fvARB(
				_shader_uniforms[_current_shader_program][s->_uniform_index],
				1,
				&s->_values[0]
			);
		break;

		case 4:
			glUniform4fvARB(
				_shader_uniforms[_current_shader_program][s->_uniform_index],
				1,
				&s->_values[0]
			);
		break;

		default:
		break;
	}
	// lookup attribute
	GLint attribute = glGetAttribLocation(_shader_program[s->_index].first, _shader_programs[s->_index].second->_attributes
#endif
}

void GLRenderer::visitGeometryConst (const Geometry *g)
{
	glPushMatrix ();

	visitTransformationConst (g);

	for (size_t i = 0; i < g->_faces.size (); ++i)
		draw_face (*(g->_faces[i].inspect()));

	glPopMatrix ();
}

void GLRenderer::visitTranslationConst (const Translation *t)
{
	glTranslatef (t->_translation_vector[0], t->_translation_vector[1], t->_translation_vector[2]);
}

void GLRenderer::visitRotationConst (const Rotation *r)
{
	glRotatef (r->_rotation_angle, r->_rotation_vector[0], r->_rotation_vector[1], r->_rotation_vector[2]);
}

void GLRenderer::visitScaleConst (const Scale *s)
{
	glScalef (s->_scaling_vector[0], s->_scaling_vector[1], s->_scaling_vector[2]);
}

void GLRenderer::visitLinearConst (const Linear *l)
{
	glMultMatrixf (l->_transformation_matrix.getPtr());
}


void GLRenderer::visitTransformationConst (const Transformation *t)
{
	for (std::list <cow_ptr<TransformationCommand> >::const_reverse_iterator it = t->_commands.rbegin(); it != t->_commands.rend (); ++it)
	{
		(*it)->acceptConst (this);
	}
	// glMultMatrixf (g->_transformation_matrix.get_coefficients());
}


void GLRenderer::visitLightingConst (const Lighting *l)
{
	if (l->_on && (*_control_ins[LIGHTING] > 0.5))
	{
		glEnable (GL_LIGHTING);
	}
	else
	{
		glDisable (GL_LIGHTING);
	}
}

void GLRenderer::visitBlendingConst (const Blending *b)
{
	if ((b->_on) && (*_control_ins[TRANSPARENCY] > 0.5))
	{
		//std::cout << "on" << std::endl;
        ofEnableAlphaBlending();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glShadeModel(GL_FLAT);

		///// evil workaround to fix blending
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}
	else
	{
        //std::cout << "off" << std::endl;
        ofDisableAlphaBlending();
	}

}


void GLRenderer::visitCullingConst (const Culling *c)
{
	switch (c->_mode)
	{
		case Culling::Off:
			glDisable (GL_CULL_FACE);
		break;
		case Culling::Front:
			glEnable (GL_CULL_FACE);
			glCullFace (GL_FRONT);
		break;
		case Culling::Back:
			glEnable (GL_CULL_FACE);
			glCullFace (GL_BACK);
		break;
		case Culling::Both:
			glEnable (GL_CULL_FACE);
			glCullFace (GL_FRONT_AND_BACK);
		break;
	}
}


void GLRenderer::visitTextConst (const Text *t)
{
	glPushMatrix();
    ofMaterial ma;
	if (*_control_ins[LIGHTING] > 0.5)
		ma = do_material (t->_material);

	visitTransformationConst (t);
	//glColor4fv (&(t->_color[0]));
    ofSetColor(t->_color);

	glScalef(0.2,0.2,0.2);

	if (t->_render_mode == WIREFRAME) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glLineWidth (t->_thickness);
	}
	else {
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

#ifdef HAVE_FTGL
	StringPool *str = StringPool::get_instance();
	str->_font->FaceSize(t->_fontsize);
	str->_font->Render(t->_text.c_str());
#endif
    if (*_control_ins[LIGHTING] > 0.5)
        ma.end();

	glPopMatrix();
}


void GLRenderer::process_g (double delta_t)
{
	_delta_t = delta_t;
    if(!_ready) {
        ofGLFWWindowSettings settings;
        settings.width = 600;
        settings.height = 600;
        settings.setPosition(ofVec2f(300,0));
        settings.resizable = true;
        settings.title = _window_title;
        
        _main_window = ofCreateWindow(settings);
        shared_ptr<GLApp> app(new GLApp(this));
        _main_app = app;
        ofRunApp(_main_window, _main_app);
        //_main_app->setup();
        _ready=true;
    }
    
    //TODO glewContext = _gl_widget->getGlewContext();
    //_main_app->draw();
}


void GLRenderer::really_process_g (double delta_t)
{
    delta_t = _delta_t;
    //_main_window->makeCurrent();

	if (!_ready)
		return;

	// TODO find a better place for this
	if (_max_feedback_frames == (SCGRAPH_QT_GL_RENDERER_MAXMAX_FEEDBACK_FRAMES + 1))
		change_feedback_frames();

	// first thing to do 
	// _gl_widget->makeCurrent ();
/*
#ifndef HAVE_SHADERS
	// deactivate shaders
    glUseProgramObjectARB(0);
#endif
*/
	//_gl_widget->makeOverlayCurrent ()

	if (_up_key_down && !_shift_key_down)
		_forward += delta_t;

	if (_down_key_down && !_shift_key_down)
		_forward -= delta_t;

	if (_up_key_down && _shift_key_down)
		_upward -= delta_t;

	if (_down_key_down && _shift_key_down)
		_upward += delta_t ;

	if (_right_key_down)
		_sideward -= delta_t;

	if (_left_key_down)
		_sideward += delta_t;

#if 0
	_forward -= delta_t *  (_forward);
	_sideward -= delta_t *	(_sideward);
	_upward -= delta_t *  (_upward);
#endif

	_transformation_matrix.makeIdentityMatrix();
	_rotation_matrix.makeIdentityMatrix();

	ofMatrix4x4 m;
	m.makeIdentityMatrix();
	m.translate(_sideward, _upward, _forward);

	if (_mouse_down)
	{
		_rot_y += (_ren_mouse_x - _cur_mouse_x)*0.1*delta_t;
		_rot_x += (_ren_mouse_y - _cur_mouse_y)*0.1*delta_t;
	}

	ofMatrix4x4 r;

	r.rotateRad(_rot_x, 1, 0, 0);
    
	_rotation_matrix *= r;

    r.rotateRad(_rot_y, 0, 1, 0);
	_rotation_matrix = _rotation_matrix * r;

	_rotation_matrix.makeOrthoNormalOf(_rotation_matrix);

	_transformation_matrix	*= _rotation_matrix;
    _transformation_matrix *= m;

	_ren_mouse_x = _cur_mouse_x;
	_ren_mouse_y = _cur_mouse_y;


	glDisable (GL_BLEND);


	glClearColor
	(
		*_control_ins[CLEAR_COLOR + 0],
		*_control_ins[CLEAR_COLOR + 1],
		*_control_ins[CLEAR_COLOR + 2],
		*_control_ins[CLEAR_COLOR + 3]
	);

	if (*_control_ins[CLEAR_MODE] > 0.5)
	{
		glClear (GL_DEPTH_BUFFER_BIT);
		glEnable (GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glShadeModel(GL_FLAT);

		glMatrixMode(GL_PROJECTION);
	
		glLoadIdentity();
	
		glMatrixMode (GL_MODELVIEW);
	
		glLoadIdentity ();

		glBegin (GL_QUADS);

		glColor4f
		(
			*_control_ins[CLEAR_COLOR + 0],
			*_control_ins[CLEAR_COLOR + 1],
			*_control_ins[CLEAR_COLOR + 2],
			*_control_ins[CLEAR_COLOR + 3]
		);

		glVertex3f(-1, -1, 0);
		glVertex3f( 1, -1, 0);
		glVertex3f( 1,	1, 0);
		glVertex3f(-1,	1, 0);

		glEnd ();

		glDisable (GL_BLEND);
		glClear (GL_DEPTH_BUFFER_BIT);

	}
	else
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glEnable (GL_BLEND);
	// glDisable (GL_BLEND);


	if (*_control_ins[FOG] > 0.5)
	{
		glEnable (GL_FOG);

		switch ((int)*_control_ins[FOG_MODE])
		{
			case 0:
				glFogi (GL_FOG_MODE, GL_LINEAR);
			break;

			case 1:
				glFogi (GL_FOG_MODE, GL_EXP);
			break;

			case 2:
				glFogi (GL_FOG_MODE, GL_EXP2);
			break;
		}

		float fog_color [4];
		fog_color [0] = *_control_ins[FOG_COLOR + 0];
		fog_color [1] = *_control_ins[FOG_COLOR + 1];
		fog_color [2] = *_control_ins[FOG_COLOR + 2];
		fog_color [3] = *_control_ins[FOG_COLOR + 3];


		glFogfv (GL_FOG_COLOR, fog_color);
		glFogf (GL_FOG_DENSITY, *_control_ins[FOG_DENSITY]);

		glFogf (GL_FOG_START, *_control_ins[FOG_START]);
		glFogf (GL_FOG_END, *_control_ins[FOG_END]);

		// TODO: implement speed hint
		switch ((int)*_control_ins[FOG_NICENESS])
		{
			case 0:
				glHint (GL_FOG_HINT, GL_DONT_CARE);
			break;

			case 1:
				glHint (GL_FOG_HINT, GL_FASTEST);
			break;

			case 2:
				glHint (GL_FOG_HINT, GL_NICEST);
			break;
		}
		glHint (GL_FOG_HINT, GL_DONT_CARE);
	}
	else
		glDisable (GL_FOG);

	// set up camera position and view direction
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	if (*_control_ins[PERSPECTIVE] > 0.5)
	{
		/*gluPerspective (*_control_ins[FOV],
                        // TODO: right measurements?
						(GLfloat)_main_window->getWidth()
						/ (GLfloat)_main_window->getHeight(),
						*_control_ins[NEAR_PLANE], 
						*_control_ins[FAR_PLANE]);
         */
        ofSetupScreenPerspective(_main_window->getWidth(),
                                 _main_window->getHeight(),
                                 *_control_ins[FOV],
                                 *_control_ins[NEAR_PLANE],
                                 *_control_ins[FAR_PLANE]);
        _camera.disableOrtho();
                                 
	}
	else
	{
        // TODO: right measurements?
		float ratio = (GLfloat)_main_window->getWidth()
			/ (GLfloat)_main_window->getHeight();

		glOrtho (-1.0 * ratio, 1.0 * ratio, 
				 -1.0, 1.0,
				 *_control_ins[NEAR_PLANE], 
				 *_control_ins[FAR_PLANE]);
        _camera.enableOrtho();
	}


    _look_at_matrix.makeLookAtMatrix(ofVec3f(*_control_ins[EYE + 0],
                                    *_control_ins[EYE + 1],
                                    *_control_ins[EYE + 2]),
                            ofVec3f(*_control_ins[CENTER + 0],
                                    *_control_ins[CENTER + 1],
                                    *_control_ins[CENTER + 2]),
                            ofVec3f(*_control_ins[UP + 0],
                                    *_control_ins[UP + 1],
                                    *_control_ins[UP + 2]));
    _look_at_matrix *= _transformation_matrix;
    _camera.setTransformMatrix(_look_at_matrix);
    _camera.begin();
    
	// turn off all lights first so they can be reenabled on demand later
	if (*_control_ins[LIGHTING] > 0.5)
	{
		glEnable (GL_NORMALIZE);
		glEnable (GL_LIGHTING);
		for (int i = 0; i < 8; ++i)
			glDisable (_gl_light_indexes[i]);
	}
	else
		glDisable (GL_LIGHTING);

	if (*_control_ins[CULLING] > 0.5)
	{
		glEnable (GL_CULL_FACE);
		glCullFace (GL_BACK);
	}
	else
		glDisable (GL_CULL_FACE);


	glMatrixMode (GL_MODELVIEW);

	// this does the gruntwork)
	if (_graphics_ins[0])
	{
		// std::cout << "iterating over graphics ins" << std::endl;
		for (size_t i = 0; i < _graphics_ins[0]->_graphics.size (); ++i)
		{
			if (_graphics_ins[0]->_graphics[i].inspect())
			{
				// std::cout << "graphics in!!" << std::endl;
				_graphics_ins[0]->_graphics[i]->acceptConst(this);
			}
		}
	}

	// glAccum (GL_MULT, 0.5);
	// glAccum (GL_RETURN, 1.0);

	if (_feedback > 0) {
							/*glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 
							  0, 0,
							  (int)pow(2,(int)ceil(log2(_gl_widget->width()))),
							  (int)pow(2,(int)ceil(log2(_gl_widget->height()))), 
							  0);*/
		glActiveTexture(_past_frame_handles[_fbcounter]);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 0, 0,
                         _main_window->getWidth(), _main_window->getHeight(), 0);
		//std::cout << "GL error message:" << glGetError() << std::endl; 
		_feedback = 0;
	}
	_fbcounter = (_fbcounter + 1) % _max_feedback_frames;

	glDisable (GL_LIGHTING);
    
    _camera.end();
    
	if (_show_help)
	{
		int y_offset = 20;

		glColor3f (1, 1, 1);
        ofSetColor(scgColor::white);
        _main_window->renderer()->drawString(helptext, 10, y_offset, 0);
/* TODO
		QStringList::const_iterator constIterator;
		for (constIterator = helptexts.constBegin(); 
			 _show_help && (constIterator != helptexts.constEnd());
			 ++constIterator)
			{
				_gl_widget->renderText (10, y_offset, *constIterator, font);
				y_offset += 13;
			}
 */
	}

	if (_show_info)
	{
		_show_help = false;

		glColor3f (1, 1, 1);
/* TODO
		for(unsigned char dir = 0; _show_info && (dir < 3); dir++) {
			for(unsigned char axis = 0; _show_info && (axis < 3); axis++) {
				if(_show_info)
					_gl_widget->renderText(10, 
									   ((dir * 3) + (axis + 1)) * 13, 
									   QString("%1_%2: %3")
									   .arg(directions.at(dir))
									   .arg(axisnames.at(axis))
									   .arg(*_control_ins[offsets.at(dir) + axis]),
									   font);
			}
 
		}
 */
	}

}


void GLRenderer::mousePressed(int x, int y, int button)
{
    _cur_mouse_x = _ren_mouse_x = x;
    _cur_mouse_y = _ren_mouse_y = y;
	_mouse_down = true;
}


void GLRenderer::mouseReleased(int x, int y, int button)
{
	_mouse_down = false;
}


void GLRenderer::mouseDragged(int x, int y, int button)
{
	_cur_mouse_x = x;
	_cur_mouse_y = y;
}
/* TODO
void GLRenderer::mouseDoubleClickEvent (QMouseEvent *event)
{
	_full_screen = !_full_screen;
	if (_full_screen)
		_main_window->showFullScreen ();
	else
		_main_window->showNormal ();
}
*/

void GLRenderer::keyPressed(int key)
{
	switch (key)
	{
		case OF_KEY_UP:
			_up_key_down = true;
			return;
		break;

		case OF_KEY_DOWN:
			_down_key_down = true;
			return;
		break;

		case OF_KEY_RIGHT:
			_right_key_down = true;
			return;
		break;

		case OF_KEY_LEFT:
			_left_key_down = true;
			return;
		break;

		case OF_KEY_SHIFT:
			_shift_key_down = true;
			return;
		break;

		case (int)'r':
			_transformation_matrix.makeIdentityMatrix();
			_rotation_matrix.makeIdentityMatrix();
			_rot_y = 0;
			_rot_x = 0;
			_forward = 0;
			_sideward = 0;
			_upward = 0;
			return;
		break;

		case (int)'s':
			/* TODO if(event->isAutoRepeat() == false)
				_gl_widget->makeScreenshot();			*/

			return;
		break;

		case (int)'m':
            /* TODO
			if(event->isAutoRepeat() == false) 
				_gl_widget->toggleRecording();*/

			return;
		break;
	
		case (int)'i':
			_show_info = !_show_info;

			if (_show_info)
				_show_help = false;

			return;
		break;

		case OF_KEY_F1:
		case (int)'h':
			_show_help = !_show_help;

			if (_show_help)
				_show_info = false;

			return;
		break;

		case (int)'f':
			_full_screen = !_full_screen;
			if (_full_screen)
				_main_window->setFullscreen(true);
			else
				_main_window->setFullscreen(false);
	
			return;
		break;

		case OF_KEY_ESC:
			if (_full_screen)
				{
					_full_screen = !_full_screen;
					_main_window->setFullscreen(false);
				}
			return;
		break;

	
	}
}


void GLRenderer::keyReleased(int key)
{
	switch (key)
	{
		case OF_KEY_UP:
			_up_key_down = false;
			return;
		break;

		case OF_KEY_DOWN:
			_down_key_down = false;
			return;
		break;

		case OF_KEY_RIGHT:
			_right_key_down = false;
			return;
		break;

		case OF_KEY_LEFT:
			_left_key_down = false;
			return;
		break;

		case OF_KEY_SHIFT:
			_shift_key_down = false;
			return;
		break;
	}
}


void GLRenderer::set_done_action (int done_action)
{
	_done_action = done_action;
}

void GLRenderer::appendToWindowTitle (string toAppend)
{
	_main_window->setWindowTitle (_window_title + toAppend);
}

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		GLRenderer *tmp = new GLRenderer ();
		return (GUnit*) tmp; 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GGLRenderer";
		else
			return 0;
	}

	void *thread_func (void *arg);
}


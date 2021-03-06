#include "binary_op.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		if (index == 0)
			return (GUnit*) new BinaryOp (special_index); 
		if (index == 1)
			return (GUnit*) new MulAdd (); 

		return 0;
	}

	size_t get_num_of_units ()
	{
		return 2;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "BinaryOpUGen";
		if (index == 1)
			return "MulAdd";

		else
			return 0;
	}
}


BinaryOp::BinaryOp (int special_index) :
	_factor (1.f),
	_special_index (special_index)
{
}

BinaryOp::~BinaryOp ()
{
	// std::cout << "[BinaryOp]: Destructor" << std::endl;
}

void BinaryOp::visitGeometry (Geometry *g) {
	for (size_t i = 0; i < g->_faces.size (); ++i) {
		if (g->_faces[i]->_colors.empty()) {
			g->_faces[i].touch()->_face_color.scale_alpha(_factor);
		}
		else {
			for (size_t j = 0; j < g->_faces[i]->_colors.size (); ++j) {
				g->_faces[i].touch()->_colors[j].scale_alpha(_factor);
			}
		}
	}
}

void BinaryOp::visitText (Text *t) {
	t->_color.scale_alpha(_factor);
}


void BinaryOp::process_g (double delta_t)
{
	switch (_special_index)
	{
		case 0:
			{
				// throw away olde output
			  _graphics_outs[0]._graphics.clear ();

			  // and copy the inputs to da outputs
				for (size_t i = 0; i < _graphics_ins[0]->_graphics.size (); ++i)
				{
					_graphics_outs[0]._graphics.push_back (_graphics_ins[0]->_graphics[i]);
				}
				// [of both inputs :)]
				for (size_t i = 0; i < _graphics_ins[1]->_graphics.size (); ++i)
				{
					_graphics_outs[0]._graphics.push_back (_graphics_ins[1]->_graphics[i]);
				}
			}
		break;

	case 2:
		{
			_graphics_outs[0]._graphics.clear();
			int side = -1;
			if((_graphics_ins[0] != NULL) && (_graphics_ins[1] == NULL)) {
				side = 0;
				_factor = (float) *_control_ins[1];
			}
			else if((_graphics_ins[0] == NULL) && (_graphics_ins[1] != NULL)) {
				side = 1;
				_factor = (float) *_control_ins[0];
			}
			if(side >= 0) {
				for (size_t i = 0; i < _graphics_ins[side]->_graphics.size (); ++i) {
					_graphics_outs[0]
						._graphics.push_back (_graphics_ins[side]->_graphics[i]);
					_graphics_outs[0]._graphics[i].touch()->accept(this);
				}
			}
			else {
				//std::cout << (float) *_control_ins[0] << "b: " <<
				//(float) *_control_ins[1] << std::endl;
				// pretend it's adding + scaling
				// and copy the inputs to da outputs
				for (size_t i = 0; i < _graphics_ins[0]->_graphics.size (); ++i)
				{
					_graphics_outs[0]._graphics.push_back (_graphics_ins[0]->_graphics[i]);
				}
				// [of both inputs :)]
				for (size_t i = 0; i < _graphics_ins[1]->_graphics.size (); ++i)
					{
						_graphics_outs[0]._graphics.push_back (_graphics_ins[1]->_graphics[i]);
					}
				// and scale by the second control in
				_factor = (float) *_control_ins[1];
				for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
				{
					_graphics_outs[0]._graphics[i].touch()->accept(this);
				}
				//std::cout << "[BinaryOp]: Can't multiply gr with gr!" << std::endl;
			}
		}
		break;

		default:
			//std::cout << "[BinaryOp]: BinaryOp ignored. Only + is supported for graphics rate right now. Special index: " << _special_index << std::endl;
		break;
	}
}

void BinaryOp::process_c (double delta_t)
{
	switch (_special_index)
	{
		case 0:
			_control_outs[0] = *_control_ins[0] + *_control_ins[1];
		break;

		case 1:
			_control_outs[0] = *_control_ins[0] - *_control_ins[1];
		break;

		case 2:
			_control_outs[0] = *_control_ins[0] * *_control_ins[1];
		break;

		case 3:
			_control_outs[0] = *_control_ins[0] / *_control_ins[1];
		break;

		default:
		break;
	}
}


MulAdd::MulAdd ()
{
}

MulAdd::~MulAdd ()
{
	// std::cout << "[MulAdd]: Destructor" << std::endl;
}

void MulAdd::visitGeometry (Geometry *g) {
	for (size_t i = 0; i < g->_faces.size (); ++i) {
		if (g->_faces[i]->_colors.empty()) {
			g->_faces[i].touch()->_face_color.scale_alpha((float) *_control_ins[1]);
		}
		else {
			for (size_t j = 0; j < g->_faces[i]->_colors.size (); ++j) {
				g->_faces[i].touch()->_colors[j].scale_alpha((float) *_control_ins[1]);
			}
		}
	}
}

void MulAdd::visitText (Text *t) {
	t->_color.scale_alpha((float) *_control_ins[1]);
}


void MulAdd::process_g (double delta_t)
{
	// in, mul, add
	_graphics_outs[0]._graphics.clear ();

	// and copy the inputs to da outputs
	if(_graphics_ins[0] != NULL) {
		for (size_t i = 0; i < _graphics_ins[0]->_graphics.size (); ++i) {
			_graphics_outs[0]._graphics.push_back (_graphics_ins[0]->_graphics[i]);
		}
	}
	// of the in & add inputs
	if(_graphics_ins[2] != NULL) {
		for (size_t i = 0; i < _graphics_ins[2]->_graphics.size (); ++i) {
			_graphics_outs[0]._graphics.push_back (_graphics_ins[2]->_graphics[i]);
		}
	}
	if(_graphics_ins[1] != NULL) {
		if(_graphics_ins[1]->_graphics.size() > 0) {
			std::cout << "[MulAdd] can't multiply 2 graphics rate ugens!"  
					  << std::endl;
		}
	}

	for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i) {
		_graphics_outs[0]._graphics[i].touch()->accept(this);
	}
}


void MulAdd::process_c (double delta_t)
{
	_control_outs[0] = (*_control_ins[0] * *_control_ins[1]) + *_control_ins[2];
}

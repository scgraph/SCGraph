GeometryUGen : GraphicsUGen {
	checkInputs {
		^this.checkAllInputs(0)
	}
}

GBorderedRectangle : GeometryUGen {
	*gr { 
		arg 
			w = 1, h = 1,
			border_thickness = 0.1,
			fill_color = [0, 0, 0, 0],
			border_color = [1, 1, 1, 1];

		^this.multiNew
		(
			'audio',
			w,
			h,
			border_thickness,
			fill_color[0],
			fill_color[1],
			fill_color[2],
			fill_color[3],
			border_color[0],
			border_color[1],
			border_color[2],
			border_color[3]
		);
	}
}

GCircle : GeometryUGen {
	*gr { arg numSegments = 32;
		^this.multiNew(
			'audio', 
			numSegments
		);
	}
}

GCube : GeometryUGen {
	*gr {
		arg size = 1.0;
		^this.multiNew('audio', size);
	}
}

GFBTexQuad : GeometryUGen {
	*gr 
	{ 
		arg 
			vector1 = [-0.5, -0.5, 0], 
			vector2 = [0.5, -0.5, 0], 
			vector3 = [0.5, 0.5, 0], 
			vector4 = [-0.5, 0.5, 0], 
			tex_coord1 = [0, 0], 
			tex_coord2 = [1, 0], 
			tex_coord3 = [1,1], 
			tex_coord4 = [0,1], 
			past_frame = 1,
			normal = [0, 0, 1];

		^this.multiNew(
			'audio', 
			vector1[0], vector1[1], vector1[2], 
			vector2[0], vector2[1], vector2[2], 
			vector3[0], vector3[1], vector3[2], 
			vector4[0], vector4[1], vector4[2], 
			tex_coord1[0], tex_coord1[1], 
			tex_coord2[0], tex_coord2[1], 
			tex_coord3[0], tex_coord3[1], 
			tex_coord4[0], tex_coord4[1], 
			past_frame, 
			normal[0], normal[0], normal[0]
		);
	}
}

GLine : GeometryUGen {
	*gr { arg strength = 1.0, vector1 = [0, 0, 0], vector2 = [1, 1, 0];
		^this.multiNew('audio', strength,  vector1[0], vector1[1], vector1[2], vector2[0], vector2[1], vector2[2]);
	}
}

GModel : GeometryUGen {
	*gr {
		arg size = 1.0;
		^this.multiNew('audio', size);
	}
}

GPoints : GeometryUGen {
	*gr { arg size = 1.0, vectors = [[0, 0, 0]];
		if (((vectors.flat.size / vectors.size) == 3), {
			^this.multiNewList(['audio'] ++ size ++ vectors.size ++ vectors.flat);
		}, {
			^this.multiNew('audio', size, 'vectors not Array of Arrays of size 3');
		})
	}
}
GPolygon : GeometryUGen {
	*gr { arg vectors = [[0,0,0]], normal = [0, 0, 1];
		// we need to pass the number of vectors, so the unit knows what
		// control inputs are valid
		if (((vectors.flat.size / vectors.size) == 3), {
			^this.multiNewList(['audio'] ++ vectors.size ++ vectors.flat ++ normal);
		}, {
			^this.multiNew('audio', 'vector not array of arrays of size 3');
		})
	}
}

GQuad : GeometryUGen {
	*gr { arg vector1 = [-0.5, -0.5, 0], vector2 = [0.5, -0.5, 0], vector3 = [0.5, 0.5, 0], vector4 = [-0.5, 0.5, 0], normal = [0, 0, 1];
		^this.multiNew(
			'audio', 
			vector1[0], vector1[1], vector1[2],
			vector2[0], vector2[1], vector2[2],
			vector3[0], vector3[1], vector3[2],
			vector4[0], vector4[1], vector4[2],
			normal[0], normal[1], normal[2]
		);
	}
}

GRectangle : GeometryUGen {
	*gr { arg width = 1, height = 1;
		^this.multiNew('audio', width, height);
	}
}

GTexQuad : GeometryUGen {
	*gr 
	{ 
		arg 
			vector1 = [-0.5, -0.5, 0], 
			vector2 = [0.5, -0.5, 0], 
			vector3 = [0.5, 0.5, 0], 
			vector4 = [-0.5, 0.5, 0], 
			tex_coord1 = [0, 0], 
			tex_coord2 = [1, 0], 
			tex_coord3 = [1,1], 
			tex_coord4 = [0,1], 
			tex_index = 0,
		frame_index = 0,
			normal = [0, 0, 1];

		^this.multiNew(
			'audio', 
			vector1[0], vector1[1], vector1[2], 
			vector2[0], vector2[1], vector2[2], 
			vector3[0], vector3[1], vector3[2], 
			vector4[0], vector4[1], vector4[2], 
			tex_coord1[0], tex_coord1[1], 
			tex_coord2[0], tex_coord2[1], 
			tex_coord3[0], tex_coord3[1], 
			tex_coord4[0], tex_coord4[1], 
			tex_index, 
			frame_index,
			normal[0], normal[0], normal[0]
		);
	}
}

GText : GeometryUGen {
	*gr { arg fontSize = 1, stringID=0;
		^this.multiNew(
			'audio', 
			fontSize,
			stringID);
	}
}

GTube : GeometryUGen {
	*gr {
		arg size = 1.0, numSegments = 32;
		^this.multiNew('audio', size, numSegments);
	}
}


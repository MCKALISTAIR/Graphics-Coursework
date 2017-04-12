#version 440

// Incoming frame data
uniform sampler2D tex;
// screen width
uniform float screen_width;
// screen height
uniform float screen_height;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

float checker(in float limitOne, in float limitTwo , in float val)
{
	if (val < limitOne)
	{
		return 0.0; // Set the color component to black
	}
	if (val > limitTwo)
	{
		return 1.0; // Set the colour component to white
	}
	return val;
}

// Averaged pixel intensity from 3 colour channels
float average_intensity(in vec4 fragment) 
{
	return (fragment.r + fragment.g + fragment.b)/3.0;
}

vec4 get_fragment(in vec2 coords, in float dx, in float dy)
{
	return texture2D(tex,coords + vec2(dx, dy));
}

// returns fragment color
float detectEdge(in vec2 coords)
{
	float dxtex = 1.0 / screen_width /*inverted image width*/;
	float dytex = 1.0 / screen_height /*inverted image height*/;
	float fragment[9];
	int k = 0;
	float delta;

	// read neighboring fragment intensities
	for (int i=-1; i<2; i++) 
	{
		for(int j=-1; j<2; j++) 
		{
			fragment[k] = average_intensity(get_fragment(coords,float(i)*dxtex, float(j)*dytex));
			k++;
		}
	}

	// average color differences around neighboring fragment
	delta = (abs(fragment[1]-fragment[7]) + abs(fragment[5]-fragment[3]) + abs(fragment[0]-fragment[8]) + abs(fragment[2]-fragment[6])) / 4.0;

	return checker(0.1,0.4,clamp(1.8*delta,0.0,1.0));
}

void main()
{
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	color.b = detectEdge(tex_coord);
	colour = color;
}
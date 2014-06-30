#version 130

uniform float var;
in vec4 s_vPosition;

out vec4 s_vColor;

void main () {
	vec4 pos = s_vPosition;
	//uncomment the lines below to animate the grid
	//pos.y = pos.x + pos.y*sin(var);
	//pos.x = sin(10*var + pos.x)*sin(10*var + pos.x);
	gl_Position = pos;	
	s_vColor = vec4(1-var,var,var,1.0);

}

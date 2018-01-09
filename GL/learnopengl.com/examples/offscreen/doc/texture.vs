#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 scale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float width;
uniform float height;

uniform int front;

void main()
{
	if(front == 1){
	    gl_Position = scale* projection * view * model * vec4(aPos.x*width, aPos.y*height, aPos.z, 1.0);
	    //gl_Position = scale* projection * view * model * vec4(aPos, 1.0);

	}
	else{
		//gl_Position = scale* projection * view * model * vec4(aPos, 1.0);
		gl_Position = scale* vec4(aPos, 1.0);
	}
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
} 
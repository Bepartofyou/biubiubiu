#ifndef _OFF_SCREEN_H_
#define _OFF_SCREEN_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>

#include <linmath.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if USE_NATIVE_OSMESA
#define GLFW_EXPOSE_NATIVE_OSMESA
#include <GLFW/glfw3native.h>
#endif


static float pos_base[] = {
	-1.0f, 1.0f,
	-1.0f, -1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f,
};

static float f_tex1[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
};

static float f_tex2[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
};

static const char* vertex_shader_text =
	"attribute vec4 a_Position1;     \n"
	"attribute vec4 a_Position2;     \n"
	"attribute vec2 a_TextureCoordinates;     \n"
	"attribute vec2 a_TextureCoordinates2;     \n"
	"uniform  mat4 u_Matrix;     \n"
	"uniform  float u_type;     \n"
	"\n"
	"varying vec2 v_TextureCoordinates;     \n"
	"varying float v_type;     \n"
	"          \n"
	"\n"
	"void main()                      \n"
	"{                                \n"
	"	 if(u_type == 2.0)\n"
	"	 {\n"
	"	 	v_TextureCoordinates = a_TextureCoordinates2;\n"
	"	 	gl_Position = u_Matrix * a_Position2;\n"
	"	 }\n"
	"	 else\n"
	"	 {\n"
	"	  	v_TextureCoordinates = a_TextureCoordinates;\n"
	"	  	gl_Position = a_Position1;\n"
	"	 }\n"
	"	 \n"
	"	 v_type = u_type;\n"
	"}";

static const char* fragment_shader_text =
//        "precision mediump float;   \n"
        "                                          \n"
        "uniform sampler2D  u_TextureUnit;                                 \n"
        "uniform sampler2D  u_TextureUnit2;                                 \n"
        "varying vec2 v_TextureCoordinates;                                 \n"
		"varying float v_type;     \n"
		"    \n"
        "void main()                           \n"
        "{                                 \n"
        "    if(v_type == 1.0){"
		"		 gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates);"
        "	 }else{"
		"		 gl_FragColor = texture2D(u_TextureUnit2, v_TextureCoordinates);    "
        "	 }                                           \n"
//        "    gl_FragColor = texture2D(u_TextureUnit, v_TextureCoordinates);                                           \n"
        "}";


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

class CoffScreen
{
public:
	CoffScreen(const char* backpic, const char* frontpic);
	~CoffScreen();

public:
	int InitWin(const int winW, const int winH);
	int GenPic(const char* outpath, int fps, int duration, bool clock, int startnum);
	void DestoryWin();
private:
	GLFWwindow* m_window;
	std::string m_back;
	std::string m_front;
	int m_winW;
	int m_winH;

	float m_f_pos1_base[8];
	float m_f_pos2_base[8];
	float m_f_pos1[8];
	float m_f_pos2[8];
};

#endif /* _OFF_SCREEN_H_ */
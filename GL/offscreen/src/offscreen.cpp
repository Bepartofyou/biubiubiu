#include <iostream>
#include <sstream>  

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "pngdecoder.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include "offscreen.h"

CoffScreen::CoffScreen(const char* backpic, const char* frontpic){
	m_back = backpic;
	m_front = frontpic;

	int j;
	for (j = 0; j < sizeof(pos_base) / sizeof(float); ++j) {
		m_f_pos1[j] = m_f_pos1_base[j] = pos_base[j];
		m_f_pos2[j] = m_f_pos2_base[j] = pos_base[j] * 0.7;
		//m_f_pos2[j] = m_f_pos2_base[j] = pos_base[j];
	}
}

CoffScreen::~CoffScreen(){
}

int CoffScreen::InitWin(const int winW, const int winH) {

	m_winW = winW;
	m_winH = winH;

	glfwSetErrorCallback(error_callback);

	glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

	if (!glfwInit())
		return EXIT_FAILURE;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 20);

	printf("windows size: %d x %d\n", m_winW, m_winH);
	m_window = glfwCreateWindow(m_winW, m_winH, "Simple example", NULL, NULL);
	if (!m_window){
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	return EXIT_SUCCESS;
}

void CoffScreen::DestoryWin() {

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

int CoffScreen::GenPic(const char* outpath, int fps, int duration, bool clock, int startnum) {

	GLuint vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vpos2_location, vtex_location, vtex2_location;
	GLuint texture[2];
	GLint tex1_location, tex2_location, type_location;
	float ratio;
	int width, height;
	mat4x4 m, p, mvp;
	char* buffer;
	int i;

	GLint compile_status = -20;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (GL_TRUE != compile_status) {
		GLsizei length = 0;
		GLchar infoLog[1024];
		glGetShaderInfoLog(vertex_shader, 1024, &length, infoLog);
		printf("OpenGLHelper : compile vertexShader failed : error msg = %s", infoLog);

		return -1;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (GL_TRUE != compile_status) {
		GLsizei length = 0;
		GLchar infoLog[1024];
		glGetShaderInfoLog(vertex_shader, 1024, &length, infoLog);
		printf("OpenGLHelper : compile fragment_shader failed : error msg = %s", infoLog);

		return -1;
	}

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "u_Matrix");
	vpos_location = glGetAttribLocation(program, "a_Position1");
	vpos2_location = glGetAttribLocation(program, "a_Position2");
	vtex_location = glGetAttribLocation(program, "a_TextureCoordinates");
	vtex2_location = glGetAttribLocation(program, "a_TextureCoordinates2");
	tex1_location = glGetUniformLocation(program, "u_TextureUnit");
	tex2_location = glGetUniformLocation(program, "u_TextureUnit2");
	type_location = glGetUniformLocation(program, "u_type");

	// png decode
	PNGHandle* handles[2];
	handles[0] = mallocPngHandle();
	handles[1] = mallocPngHandle();
	decodePng(m_back.c_str(), handles[0]);
	decodePng(m_front.c_str(), handles[1]);

	for (i = 0; i < sizeof(texture) / sizeof(GLuint); i++) {
		PNGHandle* handle = handles[i];
		glGenTextures(1, texture + i);
		// Bind to the texture in OpenGL
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		// Set filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Load the bitmap into the bound texture.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, handle->width, handle->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, handle->data);
	}

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 0, m_f_pos1);
	glEnableVertexAttribArray(vpos2_location);
	glVertexAttribPointer(vpos2_location, 2, GL_FLOAT, GL_FALSE, 0, m_f_pos2);
	glEnableVertexAttribArray(vtex_location);
	glVertexAttribPointer(vtex_location, 2, GL_FLOAT, GL_FALSE, 0, f_tex1);
	glEnableVertexAttribArray(vtex2_location);
	glVertexAttribPointer(vtex2_location, 2, GL_FLOAT, GL_FALSE, 0, f_tex2);

	i = 0;
	while (!glfwWindowShouldClose(m_window))
	{
		if (i >= fps*duration)
			break;

		glfwGetFramebufferSize(m_window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_MULTISAMPLE);

		mat4x4_identity(m);
		mat4x4_identity(p);
		
		// rotate direction
		if (!clock)
			mat4x4_rotate_Z(m, m, -M_PI / 20 + i*M_PI / (20 * fps*duration));
		else
			mat4x4_rotate_Z(m, m, M_PI / 20 - i*M_PI / (20 * fps*duration));

		//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		mat4x4_mul(mvp, p, m);

		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)m);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(tex1_location, 0);
		glUniform1f(type_location, 1.0f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		///
		int width_r = handles[1]->width;
		int height_r = handles[1]->height;
		if (handles[1]->width > m_winW || handles[1]->height > m_winH){

			//update front pos
			if ((float)m_winW / (float)m_winH >= (float)handles[1]->width / (float)handles[1]->height) {

				height_r = m_winH;
				width_r = m_winH *((float)handles[1]->width / (float)handles[1]->height);
			}
			else {

				width_r = m_winW;
				height_r = m_winW *((float)handles[1]->height / (float)handles[1]->width);
			}
		}	

		glViewport((handles[0]->width - width_r)/2, (handles[0]->height - height_r)/2, width_r, height_r);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(tex2_location, 1);
		glUniform1f(type_location, 2.0f);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisable(GL_MULTISAMPLE);

		glfwSwapBuffers(m_window);
		glfwPollEvents();

		int j;
		for (j = 0; j < sizeof(m_f_pos1) / sizeof(float); ++j) {
			m_f_pos1[j] = m_f_pos1_base[j] * (float)(1 + (float)i*(float)0.3 / (float)(fps*duration));
			m_f_pos2[j] = m_f_pos2_base[j] * (float)(1 + (float)i*(float)0.3 / (float)(fps*duration));
			//m_f_pos1[j] = m_f_pos1_base[j]  ;
			//m_f_pos2[j] = m_f_pos2_base[j] ;
		}
		printf("%d, %f\n", i, m_f_pos1[0]);
		glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 0, m_f_pos1);
		glVertexAttribPointer(vpos2_location, 2, GL_FLOAT, GL_FALSE,0, m_f_pos2);

#if USE_NATIVE_OSMESA
		glfwGetOSMesaColorBuffer(m_window, &width, &height, NULL, (void**)&buffer);
#else
		buffer = (char*)calloc(4, width * height);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
#endif
		// Write image Y-flipped because OpenGL
		std::ostringstream ss;
		ss << i;
		std::string outstr = outpath + ss.str() + ".png";

		stbi_write_png(outstr.c_str(), width, height, 4, buffer + (width * 4 * (height - 1)), -width * 4);

#if USE_NATIVE_OSMESA
		// Here is where there's nothing
#else
		free(buffer);
#endif

		i++;
	}

	freePngHandle(handles[0]);
	freePngHandle(handles[1]);

	return 1;
}

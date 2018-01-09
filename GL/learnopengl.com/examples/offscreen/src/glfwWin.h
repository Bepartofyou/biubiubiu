#ifndef _GLFW_WINDOW_H_
#define _GLFW_WINDOW_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include <iostream>
#include <string>

#define  RATIO 0.3
#define  DEGREE 10

class CglfwWin
{
public:
	CglfwWin(int width, int height, const char* back, const char* front);
	~CglfwWin();

public:
	int Create();
	void Destory();
	void Generate(const char* outpath, int fps, int duration, bool clock, int startnum);

	Shader m_shader;

public:
	
private: 
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	int readImg(unsigned int& texture, const char* image, int& width, int& height, int &nrChannels);
	void processInput(GLFWwindow *window);

private:
	GLFWwindow* m_window;
	int m_Wwidth;
	int m_Wheight;
	std::string m_back;
	std::string m_front;

	float m_ratio_w;
	float m_ratio_h;

};

#endif // _GLFW_WINDOW_H_
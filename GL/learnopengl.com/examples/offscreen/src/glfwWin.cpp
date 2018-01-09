#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef __cplusplus
}
#endif // __cplusplus

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glfwWin.h"

CglfwWin::CglfwWin(int width, int height, const char* back, const char* front)
{
	m_Wwidth = width;
	m_Wheight = height;
	m_back = back;
	m_front = front;

	m_ratio_w = 1.0f;
	m_ratio_h = 1.0f;
}

CglfwWin::~CglfwWin()
{

}

void CglfwWin::Destory() {
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

int CglfwWin::Create(){
	
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 20);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(m_Wwidth, m_Wheight, "LearnOpenGL", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window, Maybe you not setting `export DISPLAY` in xshell" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, CglfwWin::framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 0;
}

void CglfwWin::Generate(const char* outpath, int fps, int duration, bool clock, int startnum) {
	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("texture.vs", "texture.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // texture coords
		1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	int width, height, nrChannels;
	// texture 1
	// ---------
	readImg(texture1, m_back.c_str(), width, height, nrChannels);
	// texture 2
	// ---------
	readImg(texture2, m_front.c_str(), width, height, nrChannels);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	if (width > height) {
		m_ratio_h = (float)height / (float)width;
	}
	else if (width < height) {
		m_ratio_w = (float)width / (float)height;
	}
	ourShader.setFloat("width", m_ratio_w);
	ourShader.setFloat("height", m_ratio_h);

	float ratio = 1.0f;
	float degree = DEGREE;
	int count = 0;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(m_window))
	{

		if (count >= fps*duration)
			break;
		// input
		// -----
		processInput(m_window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// get matrix's uniform location and set matrix
		ourShader.use();

		ratio += RATIO / (float)(fps*duration);
		degree -= DEGREE / (float)(fps*duration);
		glm::mat4 scale = glm::mat4(1.0f);
		scale = glm::scale(scale, glm::vec3(ratio, ratio, 1));
		ourShader.setMat4("scale", scale);

		ourShader.setInt("front", 0);
		// render container
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// create transformations

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		//model = glm::rotate(model, (float)1 * glm::radians(-3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		if (clock)
			model = glm::rotate(model, (float)1 * glm::radians(degree), glm::vec3(0.0f, 0.0f, 1.0f));
		else
			model = glm::rotate(model, (float)1 * glm::radians(-degree), glm::vec3(0.0f, 0.0f, 1.0f));
	
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)m_Wwidth / (float)m_Wheight, 0.1f, 100.0f);
		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		ourShader.setInt("front", 1);
		// render container
		//glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		std::ostringstream ss;
		ss << count + startnum;
		std::string outstr = outpath + ss.str() + ".png";

		char* buffer = new char[m_Wwidth*m_Wheight * 4];
		glReadPixels(0, 0, m_Wwidth, m_Wheight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		// Write image Y-flipped because OpenGL
		stbi_write_png(outstr.c_str(), m_Wwidth, m_Wheight, 4, buffer + (m_Wwidth * 4 * (m_Wheight - 1)), -m_Wwidth * 4);
		delete[]buffer;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(m_window);
		glfwPollEvents();

		//Sleep(40);
		count++;
	}

//	Sleep(40000000);
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void CglfwWin::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void CglfwWin::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

int CglfwWin::readImg(unsigned int& texture, const char* image, int& width, int& height, int &nrChannels) {
	// texture 
	// ---------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(image, &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the **.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return 0;
}

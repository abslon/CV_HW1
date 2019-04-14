#include "System.h"
#include "Shader.h"
#include "objloader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

// all callback functions must be declared in 'C' style.
// therefore, we cannot use class methods as callback.
// whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

class MainApplication
{
public:
	MainApplication() 
	{

	}

	~MainApplication()
	{
		Terminate();
	}

	// Main function for application
	void Start()
	{
		// Initialization
		InitGL();
		InitCV();

		// Creating scene
		CreateBackground();
		CreateTeapot();

		// Render loop
		RenderLoop();
	}

private:
	// initialize OpenGL with GLAD and GLFW. 
	// returns false if it fail to initialize.
	bool InitGL()
	{
		// initialize glfw
		// OpenGL version : 3.3
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// create main window
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW1", NULL, NULL);
		if (window == NULL)
		{
			cout << "Failed to create GLFW window" << endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		
		// add callback function
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		// glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			cout << "Failed to initialize GLAD" << endl;
			return false;
		}

		// configure global opengl state
		// glEnable(GL_DEPTH_TEST);

		return true;
	}

	void InitCV()
	{
		string videoPath;
		videoPath = "res\\video\\test.mp4";
		video.open(videoPath);
	}

	void CVTest()
	{
		cv::Mat testImg = cv::Mat::zeros(cv::Size(300, 400), CV_8U);
		cv::imshow("test", testImg);
	}

	void CreateBackground()
	{
		backShader = new Shader("res\\shader\\back.vert", "res\\shader\\back.frag");

		float vertices[] = 
		{
			// positions             // texture coords
			 1.0f,  1.0f, 0.0f,      1.0f, 0.0f, // top right
			 1.0f, -1.0f, 0.0f,      1.0f, 1.0f, // bottom right
			-1.0f, -1.0f, 0.0f,      0.0f, 1.0f, // bottom left
			-1.0f,  1.0f, 0.0f,      0.0f, 0.0f  // top left 
		};

		unsigned int indices[] = 
		{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// create GL buffers
		glGenVertexArrays(1, &backVAO);
		glGenBuffers(1, &backVBO);
		glGenBuffers(1, &backEBO);

		glBindVertexArray(backVAO);

		glBindBuffer(GL_ARRAY_BUFFER, backVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// load and create a texture 
		glGenTextures(1, &backTexture);
		glBindTexture(GL_TEXTURE_2D, backTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// load image, create texture and generate mipmaps
	void GrabVideo()
	{
		cv::Mat img;
		video.read(img);
		if (img.empty()) {
			cout << "failed to grab video!" << endl;
			return;
		}

		cv::cvtColor(img, videoImg, CV_BGR2RGB);

		glBindTexture(GL_TEXTURE_2D, backTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoImg.cols, videoImg.rows,
			0, GL_RGB, GL_UNSIGNED_BYTE, videoImg.ptr());
	}

	void CreateTeapot()
	{
		teapotShader = new Shader("res\\shader\\teapot.vert", "res\\shader\\teapot.frag");

		bool success = loadOBJ("res\\model\\teapot.obj", TPverts, TPuvs, TPnorms);
		if (!success) {
			cout << "failed to load teapot!" << endl;
			return;
		}

		// create GL buffers
		glGenVertexArrays(1, &teapotVAO);
		glGenBuffers(1, &teapotVBO);
		glGenBuffers(1, &teapotUBO);

		glBindVertexArray(teapotVAO);

		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO);
		glBufferData(GL_ARRAY_BUFFER, TPverts.size() * sizeof(glm::vec3), &TPverts[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, teapotUBO);
		glBufferData(GL_ARRAY_BUFFER, TPuvs.size() * sizeof(glm::vec2), &TPuvs[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);

		// load and create a texture 
		glGenTextures(1, &teapotTexture);
		glBindTexture(GL_TEXTURE_2D, teapotTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		unsigned char *data = stbi_load("res\\image\\tile.jpg", &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	// GLFW rendering loop function
	void RenderLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			// process input
			processInput(window);

			// clear
			glClear(GL_COLOR_BUFFER_BIT);

			// render background
			GrabVideo();
			backShader->use();
			glBindVertexArray(backVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			
			glUseProgram(0);

			// render teapot
			glBindTexture(GL_TEXTURE_2D, teapotTexture);
			teapotShader->use();
			glBindVertexArray(teapotVAO);
			glDrawArrays(GL_TRIANGLES, 0, TPverts.size());

			// swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	// terminate application
	void Terminate()
	{
		glfwTerminate();
	}

private:
	// GLFW
	GLFWwindow* window;

	// OpenGL
	Shader* backShader;
	unsigned int backTexture;
	unsigned int backVBO, backVAO, backEBO;

	Shader* teapotShader;
	unsigned int teapotTexture;
	unsigned int teapotVBO, teapotUBO, teapotVAO;
	std::vector< glm::vec3 > TPverts;
	std::vector< glm::vec2 > TPuvs;
	std::vector< glm::vec3 > TPnorms;

	// OpenCV
	cv::VideoCapture video;
	cv::Mat videoImg;
};

int main()
{
	cout << "HW1 started" << endl;

	MainApplication app;
	app.Start();

	return 0;
}







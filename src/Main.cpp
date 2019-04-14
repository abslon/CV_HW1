#include "System.h"
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
		InitGL();
		CVTest();
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
		glEnable(GL_DEPTH_TEST);

		return true;
	}


	void CVTest()
	{
		cv::Mat testImg = cv::Mat::zeros(cv::Size(300, 400), CV_8U);
		cv::imshow("test", testImg);
	}

	// GLFW rendering loop function
	void RenderLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			// process input
			processInput(window);

			// render (only clearcolor for now...)
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

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
	GLFWwindow* window;
	cv::VideoCapture video;
};

int main()
{
	cout << "HW1 started" << endl;

	MainApplication app;
	app.Start();

	return 0;
}







#include "System.h"
#include <GLFW/glfw3.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
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
		
		cv::Mat testImg = cv::Mat::zeros(cv::Size(400, 400), CV_8UC3);
		//draw circle
		cv::circle(testImg, cvPoint(200, 200), 25, CV_RGB(0, 100, 100),10);
		//draw filled circle
		cv::circle(testImg, cvPoint(350, 200), 25, CV_RGB(0, 300, 100), CV_FILLED,10);
		// or
		cv::circle(testImg, cvPoint(280, 200), 25, CV_RGB(0, 100, 100), -1);//thickness set to -1
		//draw line
		cv::line(testImg, cvPoint(10, 0), cvPoint(10, 100), CV_RGB(110, 220, 0), 2, 8);
		//draw arrowed line
		cv::arrowedLine(testImg, cvPoint(20, 200), cvPoint(200, 200), CV_RGB(100, 100, 100), 5);
		//draw ellipse
		cv::ellipse(testImg, cvPoint(200, 150), cvSize(100, 150), 45, 0, 360, CV_RGB(255, 0, 0),  3, 8);
		//draw filled rectangle
		cv::rectangle(testImg, cvPoint(15, 20), cvPoint(70, 50), CV_RGB(0, 55, 255), CV_FILLED);
		//draw polygon
		DrawPolygon(testImg);
		//draw contour
		DrawContour(testImg);
		//draw text
		cv::putText(testImg, "Computer Vision", cvPoint(150, testImg.rows/2), CV_FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0,143,143), 2);
		
		cv::imshow("test", testImg);
	}
	void DrawContour(cv::Mat img) {
		cv::Point contours;
		cv::Vec4i heirarchy;
		cv::findContours()

		cv::findContours(img, contours, heirarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		const cv::Point* cpt[1] = { cont_points[0] };
		cv::drawContours(img, cpt, -1, CV_RGB(255, 255, 0));
	}
	void DrawPolygon(cv::Mat img) {
		int lineType = 8;
		int w = 200;
		cv::Point poly_points[1][5];
		poly_points[0][0] = cv::Point(w / 2.0+w, w+w);
		poly_points[0][1] = cv::Point(10 + w, 2 * w / 3.0 + w);
		poly_points[0][2] = cv::Point(1*w / 4.0 + w, 10 + w);
		poly_points[0][3] = cv::Point(3*w / 4.0 + w, 10 + w);
		poly_points[0][4] = cv::Point(w + w, 2 * w / 3.0 + w);


		const cv::Point* ppt[1] = { poly_points[0] }; //the vertices of the polygon are the set of points in here
		int npt[] = { 5 };//the total number of vertices to be drawn are npt, only one polygon will be drawn
		cv::fillPoly(img, ppt, npt, 1, CV_RGB(0, 0, 255), lineType);
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







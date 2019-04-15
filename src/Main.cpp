#include "System.h"
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

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
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		// set model matrix
		
		model = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(1, 0, 0));

		// set projection matrix
		proj[0][0] = 2.0 * fx / SCR_WIDTH;
		proj[0][1] = 0.0;
		proj[0][2] = 0.0;
		proj[0][3] = 0.0;

		proj[1][0] = 0.0;
		proj[1][1] = 2.0 * fy / SCR_HEIGHT;
		proj[1][2] = 0.0;
		proj[1][3] = 0.0;

		proj[2][0] = 1.0 - 2.0 * cx / SCR_WIDTH;
		proj[2][1] = 2.0 * cy / SCR_HEIGHT - 1.0;
		proj[2][2] = (zfar + znear) / (znear - zfar);
		proj[2][3] = -1.0;

		proj[3][0] = 0.0;
		proj[3][1] = 0.0;
		proj[3][2] = 2.0 * zfar * znear / (znear - zfar);
		proj[3][3] = 0.0;

		return true;
	}

	void InitCV()
	{
		// open video
		string videoPath;
		videoPath = "res\\video\\Sample2.mp4";
		video.open(videoPath);

		// set camera params
		camMat = cv::Mat::zeros(cv::Size(3, 3), CV_32F);
		camMat.at<float>(0, 0) = fx;
		camMat.at<float>(1, 1) = fy;
		camMat.at<float>(0, 2) = cx;
		camMat.at<float>(1, 2) = cy;
		camMat.at<float>(2, 2) = 1;
		distCoeffs.zeros();
		distCoeffs[0] = k1;
		distCoeffs[0] = k2;
		distCoeffs[0] = p1;
		distCoeffs[0] = p2;

		// create aruco params
		detectorParams = cv::aruco::DetectorParameters::create();
		dictionary = cv::aruco::getPredefinedDictionary(
			cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	}

	void CVTest()
	{
		cv::Mat testImg = cv::Mat::zeros(cv::Size(300, 400), CV_8U);
		cv::imshow("test", testImg);
	}

	void CreateBackground()
	{
		backShader = new Shader("res\\shader\\back.vert", "res\\shader\\back.frag");
		backShader->use();

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

		teapotShader->use();
		teapotShader->setVec3("lightPos", glm::vec3(0.3, 1, 0.2));
		teapotShader->setVec3("viewPos", glm::vec3(0, 0, 0));
		teapotShader->setVec3("objectColor", glm::vec3(0.25, 0.7, 0.4));
		teapotShader->setVec3("lightColor", glm::vec3(1, 1, 1));
		
		string inputfile = "res\\model\\teapot.objf";
		string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

		if (!err.empty()) { // `err` may contain warning message.
			cerr << err << endl;
		}

		if (!ret) {
			cout << "failed to open teapot" << endl;
			exit(1);
		}

		// create GL buffers
		glGenVertexArrays(1, &teapotVAO);
		teapotVBO = new unsigned int[2];
		glGenBuffers(2, teapotVBO);
		// glGenBuffers(1, &teapotUBO);
		int s = shapes.size();
		teapotEBO = new unsigned int[s];
		glGenBuffers(s, teapotEBO);

		glBindVertexArray(teapotVAO);

		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, attrib.vertices.size() * sizeof(float), &attrib.vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, attrib.normals.size() * sizeof(float), &attrib.normals[0], GL_STATIC_DRAW);

		indexCount = 0;
		for (int i = 0; i < s; i++)
		{
			vector<int> indices;
			for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
			{
				indices.push_back(shapes[i].mesh.indices[j].vertex_index);
				
			}
			indexCount += indices.size();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, teapotEBO[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
			 
		}
		// position attribute
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		// texture coord attribute
		// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// glEnableVertexAttribArray(1);

		// load and create a texture 
		// glGenTextures(1, &teapotTexture);
		// glBindTexture(GL_TEXTURE_2D, teapotTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*	int width, height, nrChannels;
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
		stbi_image_free(data);*/
	}

	void Vecs2Mat(cv::Vec3d &rvec, cv::Vec3d &tvec, glm::mat4 &view)
	{
		cv::Mat rot;
		cv::Rodrigues(rvec, rot);

		view[0][0] = rot.at<double>(0, 0);
		view[0][1] = rot.at<double>(1, 0);
		view[0][2] = rot.at<double>(2, 0);
		view[0][3] = 0.0;

		view[1][0] = rot.at<double>(0, 1);
		view[1][1] = rot.at<double>(1, 1);
		view[1][2] = rot.at<double>(2, 1);
		view[1][3] = 0.0;

		view[2][0] = rot.at<double>(0, 2);
		view[2][1] = rot.at<double>(1, 2);
		view[2][2] = rot.at<double>(2, 2);
		view[2][3] = 0.0;

		view[3][0] = tvec[0];
		view[3][1] = tvec[1];
		view[3][2] = tvec[2];
		view[3][3] = 1.0;

		static float changeCoordArray[16] = { 1,  0,  0, 0, 
											  0, -1,  0, 0,
											  0,  0, -1, 0, 
											  0,  0,  0, 1  };
		auto change_coord = glm::make_mat4(changeCoordArray);
		
		view = change_coord * view;
	}

	// GLFW rendering loop function
	void RenderLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			// start timer
			clock_t before;
			before = clock();

			// process input
			processInput(window);

			// clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			
			// render background
			glDisable(GL_DEPTH_TEST);
			GrabVideo();
			backShader->use();
			glBindVertexArray(backVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			
			// find marker
			vector< int > ids;
			vector< vector< cv::Point2f > > corners, rejected;
			vector< cv::Vec3d > rvecs, tvecs;
			cv::aruco::detectMarkers(videoImg, dictionary, corners, ids, detectorParams, rejected);
			if (ids.size() > 0)
			{
				cv::aruco::estimatePoseSingleMarkers(corners, 0.07, camMat, distCoeffs, rvecs, tvecs);
			}
			Vecs2Mat(rvecs[0], tvecs[0], view);
			mvp = proj * view * model;

			// render teapot
			//glBindTexture(GL_TEXTURE_2D, teapotTexture);
			glEnable(GL_DEPTH_TEST);
			teapotShader->use();
			teapotShader->setMat4("model", model);
			teapotShader->setMat4("view", view);
			teapotShader->setMat4("proj", proj);
			glBindVertexArray(teapotVAO);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

			// swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			glfwSwapBuffers(window);
			glfwPollEvents();

			double time = (double)(clock() - before) / CLOCKS_PER_SEC;
			if (1.0f / FPS > time)
			{
				float s = (1.0f / FPS - time) * 1000;
				Sleep(s);
			}
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
	/// GLFW
	GLFWwindow* window;

	/// OpenGL
	// background
	Shader* backShader;
	unsigned int backTexture;
	unsigned int backVBO, backVAO, backEBO;
	// teapot
	Shader* teapotShader;
	unsigned int teapotTexture;
	unsigned int teapotUBO, teapotVAO;
	unsigned int *teapotVBO, *teapotEBO;
	// obj loader
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	int indexCount;
	// matrix
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 mvp;

	/// OpenCV
	// video
	cv::VideoCapture video;
	cv::Mat videoImg;
	// camera
	cv::Mat camMat;
	cv::Vec4f distCoeffs;
	// marker
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	int dictionaryId = 10;
};

int main()
{
	cout << "HW1 started" << endl;

	MainApplication app;
	app.Start();

	return 0;
}







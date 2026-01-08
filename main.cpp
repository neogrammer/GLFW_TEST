//
//#include <iostream>
//#include <string>
//#include <memory>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include "Window.h"
//#include "Logger.h"
//
//#include <string>
//void error_callback(int error, const char* description)
//{
//	fprintf(stderr, "Error: %s\n", description);
//}
//
//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//}
//
//void updateGame(double dt)
//{
//	// Update game logic here
//}
//void renderGame()
//{
//	// Render game here
//}
//
//int main()
//{
//	glfwSetErrorCallback(error_callback);
//	if (!glfwInit())
//	{
//		// Initialization failed
//		std::cout << "GLFW Initialization failed!" << std::endl;
//		return -1;
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	GLFWwindow* window = glfwCreateWindow(640, 480, "Perfectly Timed Simulation - Calculating Actual FPS..", NULL, NULL);
//	if (!window)
//	{
//		// Window or OpenGL context creation failed
//		std::cout << "GLFW Window creation failed!" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSwapInterval(1);
//
//	if (!gladLoadGL(glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//
//	glfwSetKeyCallback(window, key_callback);
//
//	int width, height;
//	glfwGetFramebufferSize(window, &width, &height);
//	glViewport(0, 0, width, height);
//	glClearColor(0.06f, 0.46f, 0.95f, 1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//
//	double startTime = glfwGetTime();
//	double accum = 0.0;
//	const double fps60 = 1.0 / 60.0;
//	int numFrames = 0;
//	double frameTimer = 0.0;
//
//	while (!glfwWindowShouldClose(window))
//	{
//		// Keep running
//		glfwPollEvents();
//
//		double dt = glfwGetTime() - startTime;
//		startTime = glfwGetTime();
//		accum += dt;
//		frameTimer += dt;
//
//		while (accum >= fps60)
//		{
//			if (numFrames < 60)
//			{
//				updateGame(fps60);
//				numFrames++;
//				accum -= fps60;
//			}
//			else
//			{
//				while (accum >= fps60)
//				{
//					// wind down the accumulator, leaving only the fractional frame for perfect timing
//					accum -= fps60;
//				}
//				// use the remaining time to update once more
//				updateGame(accum);
//
//				// reset accumulator
//				accum = 0.0;
//			}
//		}
//
//		if (frameTimer >= 1.0)
//		{
//			std::string title = "Perfectly Timed Simulation - " + std::to_string(numFrames) + " FPS";
//			glfwSetWindowTitle(window, title.c_str());
//			numFrames = 0;
//			frameTimer -= 1.0;
//			accum += frameTimer;
//			frameTimer = 0.0;
//		}
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		renderGame();
//		glfwSwapBuffers(window);
//	}
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//
//	return 0;
//}


///---------------------------------------------------------------------
///---------------------------------------------------------------------
///---------------------------------------------------------------------
///---------------------------------------------------------------------
/// 
#include <memory>
#include <string>

#include "Window.h"
#include "Logger.h"

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char* argv[]) {

    std::unique_ptr<Window> w = std::make_unique<Window>();

    glfwSetErrorCallback(error_callback);
    if (!w->init(640, 480, "OpenGL Renderer")) {
        Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
        return -1;
    }

    w->mainLoop();

    w->cleanup();

    return 0;
}






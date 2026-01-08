#pragma once
#include <string>
#include <memory>
#include <vulkan/vulkan.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OGLRenderer.h"
#include "Model.h"

class Window {
  public:
    bool init(unsigned int width, unsigned int height, std::string title);
    bool initVulkan();
    void mainLoop();
    void cleanup();

  private:
      bool vulkanIsLoaded = false;
    GLFWwindow *mWindow = nullptr;
    std::string mApplicationName;

    VkInstance mInstance{};
    VkSurfaceKHR mSurface{};

    std::unique_ptr<OGLRenderer> mRenderer;
    std::unique_ptr<Model> mModel;

    unsigned int mScreenWidth = 640;
    unsigned int mScreenHeight = 480;

    void handleWindowMoveEvents(int xpos, int ypos);
    void handleWindowMinimizedEvents(int minimized);
    void handleWindowMaximizedEvents(int maximized);
    void handleWindowCloseEvents();

    void handleKeyEvents(int key, int scancode, int action, int mods);
    void handleMouseButtonEvents(int button, int action, int mods);
    void handleMousePositionEvents(double xpos, double ypos);
    void handleMouseEnterLeaveEvents(int enter);
};

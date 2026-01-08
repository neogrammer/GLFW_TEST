#include <vector>

#include "Window.h"
#include "Logger.h"

bool Window::init(unsigned int width, unsigned int height, std::string title) {
  if (!glfwInit()) {
    Logger::log(1, "%s: glfwInit() error\n", __FUNCTION__);
    return false;
  }


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  mWindow = glfwCreateWindow(width, height, mApplicationName.c_str(), nullptr, nullptr);
  if (!mWindow) {
      if (!glfwVulkanSupported()) {
          glfwTerminate();
          Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
          return false;
      }
      /* set a "hint" for the NEXT window created*/
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      mApplicationName = title;
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      mWindow = glfwCreateWindow(width, height, mApplicationName.c_str(), nullptr, nullptr);

      if (!mWindow)
      {
          glfwTerminate();
          Logger::log(1, "%s: GLFW failed both OpenGL and Vulkan\n", __FUNCTION__);
          return false;
      }

      if (!initVulkan()) {
          Logger::log(1, "%s: Could not init Vulkan\n", __FUNCTION__);
          glfwDestroyWindow(mWindow);
          glfwTerminate();
          return false;
      }

      this->vulkanIsLoaded = true;
  }
  else
  {
      glfwMakeContextCurrent(mWindow);
  }
 
  glfwSetWindowUserPointer(mWindow, this);

  /* 2) use a lambda to get the pointer and call the member function */
  glfwSetWindowPosCallback(mWindow, [](GLFWwindow* win, int xpos, int ypos) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleWindowMoveEvents(xpos, ypos);
      }
  );

  glfwSetWindowIconifyCallback(mWindow, [](GLFWwindow* win, int minimized) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleWindowMinimizedEvents(minimized);
      }
  );

  glfwSetWindowMaximizeCallback(mWindow, [](GLFWwindow* win, int maximized) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleWindowMaximizedEvents(maximized);
      }
  );

  glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* win) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleWindowCloseEvents();
      }
  );

  glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleKeyEvents(key, scancode, action, mods);
      }
  );

  glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleMouseButtonEvents(button, action, mods);
      }
  );

  glfwSetCursorPosCallback(mWindow, [](GLFWwindow* win, double xpos, double ypos) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleMousePositionEvents(xpos, ypos);
      }
  );

  glfwSetCursorEnterCallback(mWindow, [](GLFWwindow* win, int enter) {
      auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
      thisWindow->handleMouseEnterLeaveEvents(enter);
      }
  );



  mRenderer = std::make_unique<OGLRenderer>();


  if (!mRenderer->init(width, height)) {
    glfwTerminate();
    Logger::log(1, "%s error: Could not init OpenGL\n", __FUNCTION__);
    return false;
  }

  mModel = std::make_unique<Model>();
  mModel->init();
  Logger::log(1, "%s: mockup model data loaded\n", __FUNCTION__);

  Logger::log(1, "%s: Window with OpenGL 4.6 successfully initialized\n", __FUNCTION__);
  return true;
}

bool Window::initVulkan() {
    VkResult result = VK_ERROR_UNKNOWN;

    VkApplicationInfo mAppInfo{};
    mAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    mAppInfo.pNext = nullptr;
    mAppInfo.pApplicationName = mApplicationName.c_str();
    mAppInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    mAppInfo.pEngineName = "Game Animations Programming";
    mAppInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    mAppInfo.apiVersion = VK_API_VERSION_1_4;

    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    if (extensionCount == 0) {
        Logger::log(1, "%s error: no Vulkan extensions found, need at least 'VK_KHR_surface'\n", __FUNCTION__);
        return false;
    }

    Logger::log(1, "%s: Found %u Vulkan extensions\n", __FUNCTION__, extensionCount);
    for (uint32_t i = 0; i < extensionCount; ++i) {
        Logger::log(1, "%s: %s\n", __FUNCTION__, std::string(extensions[(size_t)i]).c_str());
    }

    VkInstanceCreateInfo mCreateInfo{};
    mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    mCreateInfo.pNext = nullptr;
    mCreateInfo.pApplicationInfo = &mAppInfo;
    mCreateInfo.enabledExtensionCount = extensionCount;
    mCreateInfo.ppEnabledExtensionNames = extensions;
    mCreateInfo.enabledLayerCount = 0;

    result = vkCreateInstance(&mCreateInfo, nullptr, &mInstance);
    if (result != VK_SUCCESS) {
        Logger::log(1, "%s: Could not create Vulkan instance (%i)\n", __FUNCTION__, result);
        return false;
    }

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0) {
        Logger::log(1, "%s: No Vulkan capable GPU found\n", __FUNCTION__);
        return false;
    }

    std::vector<VkPhysicalDevice> devices;
    devices.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, devices.data());

    Logger::log(1, "%s: Found %u physical device(s)\n", __FUNCTION__, physicalDeviceCount);

    result = glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface);
    if (result != VK_SUCCESS) {
        Logger::log(1, "%s: Could not create Vulkan surface\n", __FUNCTION__);
        return false;
    }

    return true;
}



void Window::handleWindowMoveEvents(int xpos, int ypos) {
    Logger::log(1, "%s: Window has been moved to %i/%i\n", __FUNCTION__, xpos, ypos);
}

void Window::handleWindowMinimizedEvents(int minimized) {
    if (minimized) {
        Logger::log(1, "%s: Window has been minimized\n", __FUNCTION__);
    }
    else {
        Logger::log(1, "%s: Window has been restored\n", __FUNCTION__);
    }
}

void Window::handleWindowMaximizedEvents(int maximized) {
    if (maximized) {
        Logger::log(1, "%s: Window has been maximized\n", __FUNCTION__);
    }
    else {
        Logger::log(1, "%s: Window has been restored\n", __FUNCTION__);
    }
}

void Window::handleWindowCloseEvents() {
    Logger::log(1, "%s: Window got close event... bye!\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods) {
    std::string actionName;
    switch (action) {
    case GLFW_PRESS:
        actionName = "pressed";
        break;
    case GLFW_RELEASE:
        actionName = "released";
        break;
    case GLFW_REPEAT:
        actionName = "repeated";
        break;
    default:
        actionName = "invalid";
        break;
    }

    const char* keyName = glfwGetKeyName(key, 0);
    Logger::log(1, "%s: key %s (key %i, scancode %i) %s\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
    }
}

void Window::handleMouseButtonEvents(int button, int action, int mods) {
    std::string actionName;
    switch (action) {
    case GLFW_PRESS:
        actionName = "pressed";
        break;
    case GLFW_RELEASE:
        actionName = "released";
        break;
    case GLFW_REPEAT:
        actionName = "repeated";
        break;
    default:
        actionName = "invalid";
        break;
    }

    std::string mouseButtonName;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        mouseButtonName = "left";
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        mouseButtonName = "middle";
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        mouseButtonName = "right";
        break;
    default:
        mouseButtonName = "other";
        break;
    }

    Logger::log(1, "%s: %s mouse button (%i) %s\n", __FUNCTION__, mouseButtonName.c_str(), button, actionName.c_str());
}

void Window::handleMousePositionEvents(double xpos, double ypos) {
    Logger::log(1, "%s: Mouse is at position %lf/%lf\n", __FUNCTION__, xpos, ypos);
}

void Window::handleMouseEnterLeaveEvents(int enter) {
    if (enter) {
        Logger::log(1, "%s: Mouse entered window\n", __FUNCTION__);
    }
    else {
        Logger::log(1, "%s: Mouse left window\n", __FUNCTION__);
    }
}


void Window::mainLoop() {
  /* force VSYNC */
  glfwSwapInterval(1);

  /* upload only once for now */
  mRenderer->uploadData(mModel->getVertexData());

  while (!glfwWindowShouldClose(mWindow)) {
    mRenderer->draw();

    /* swap buffers */
    glfwSwapBuffers(mWindow);

    /* poll events in a loop */
    glfwPollEvents();

  }
}

void Window::cleanup() {
  mRenderer->cleanup();

  if (vulkanIsLoaded)
  {
      vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
      vkDestroyInstance(mInstance, nullptr);
  }
  if (mWindow)
      glfwDestroyWindow(mWindow);
  glfwTerminate();
  Logger::log(1, "%s: Terminating Window\n", __FUNCTION__);
}

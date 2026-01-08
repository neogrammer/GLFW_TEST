#include "OGLRenderer.h"
#include "Logger.h"

bool OGLRenderer::init(unsigned int width, unsigned int height) {

  /* initalize GLAD */
  if (!gladLoadGL(glfwGetProcAddress)) {
    Logger::log(1, "%s error: failed to initialize GLAD\n", __FUNCTION__);
    return false;
  }

  if (!GLAD_GL_VERSION_4_6) {
    Logger::log(1, "%s error: failed to get at least OpenGL 4.6\n", __FUNCTION__);
    return false;
  }

  GLint majorVersion, minorVersion;
  glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
  glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
  Logger::log(1, "%s: OpenGL %d.%d initializeed\n", __FUNCTION__, majorVersion, minorVersion);

  if (!mFramebuffer.init(width, height)) {
    Logger::log(1, "%s error: could not init Framebuffer\n", __FUNCTION__);
    return false;
  }
  Logger::log(1, "%s: framebuffer succesfully initialized\n", __FUNCTION__);

  if (!mTex.loadTexture("textures/crate.png")) {
    Logger::log(1, "%s: texture loading failed\n", __FUNCTION__);
    return false;
  }
  Logger::log(1, "%s: texture successfully loaded\n", __FUNCTION__);

  mVertexBuffer.init();
  Logger::log(1, "%s: vertex buffer successfully created\n", __FUNCTION__);

  if (!mShader.loadShaders("shader/basic.vert", "shader/basic.frag")) {
    Logger::log(1, "%s: shader loading failed\n", __FUNCTION__);
    return false;
  }
  Logger::log(1, "%s: shaders succesfully loaded\n", __FUNCTION__);

  return true;
}

void OGLRenderer::setSize(unsigned int width, unsigned int height) {
  /* handle minimize */
  if (width == 0 || height == 0) {
    return;
  }

  mFramebuffer.resize(width, height);
  glViewport(0, 0, width, height);

  Logger::log(1, "%s: resized window to %dx%d\n", __FUNCTION__, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData) {
  mTriangleCount = (int)vertexData.vertices.size() / 3;
  mVertexBuffer.uploadData(vertexData);
}

void OGLRenderer::draw() {
  /* draw to framebuffer */
  mFramebuffer.bind();

 
  glClearColor(0.06f, 0.46f, 0.95f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  //static double startTime = glfwGetTime();
  //static double accum = 0.0;
  //static const double fps60 = 1.0 / 60.0;
  //static int numFrames = 0;
  //static double frameTimer = 0.0;

  //double dt = glfwGetTime() - startTime;
  //startTime = glfwGetTime();
  //accum += dt;
  //frameTimer += dt;

  //while (accum >= fps60)
  //{
  //    if (numFrames < 60)
  //    {
  //       // updateGame(fps60);
  //        numFrames++;
  //        accum -= fps60;
  //    }
  //    else
  //    {
  //        while (accum >= fps60)
  //        {
  //            // wind down the accumulator, leaving only the fractional frame for perfect timing
  //            accum -= fps60;
  //        }
  //        // use the remaining time to update once more
  //        //updateGame(accum);

  //        // reset accumulator
  //        accum = 0.0;
  //    }
  //}

  //if (frameTimer >= 1.0)
  //{
  //    std::string title = "Perfectly Timed Simulation - " + std::to_string(numFrames) + " FPS";
  //    glfwSetWindowTitle(, title.c_str());
  //    numFrames = 0;
  //    frameTimer -= 1.0;
  //    accum += frameTimer;
  //    frameTimer = 0.0;
  //}

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //renderGame();
  //glfwSwapBuffers(window);
  mShader.use();
  mTex.bind();
  mVertexBuffer.bind();

  mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount * 3);
  mVertexBuffer.unbind();
  mTex.unbind();

  mFramebuffer.unbind();

  /* blit color buffer to screen */
  mFramebuffer.drawToScreen();
}

void OGLRenderer::cleanup() {
  mShader.cleanup();
  mTex.cleanup();
  mVertexBuffer.cleanup();
  mFramebuffer.cleanup();
}

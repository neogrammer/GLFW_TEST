#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "../vulkan/VkRenderData.h"

class Model {
  public:
    VkMesh getVertexData();

  private:
    void init();
    VkMesh mVertexData;
};

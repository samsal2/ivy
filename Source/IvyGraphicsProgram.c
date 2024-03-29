#include "IvyGraphicsProgram.h"

#include <stdio.h>

#include "IvyLog.h"
#include "IvyRenderer.h"
#include "IvyVulkanUtilities.h"

IVY_INTERNAL char *ivyLoadFileIntoByteBuffer(IvyAnyMemoryAllocator allocator,
    char const *path, uint64_t *size) {
  FILE *file = NULL;
  char *buffer = NULL;
  int bufferSize;

  file = fopen(path, "r");
  if (!file) {
    goto error;
  }

  if (0 != fseek(file, 0, SEEK_END)) {
    goto error;
  }

  if (-1 == (bufferSize = ftell(file))) {
    goto error;
  }

  if (0 != fseek(file, 0, SEEK_SET)) {
    goto error;
  }

  buffer = ivyAllocateMemory(allocator, bufferSize * sizeof(*buffer));
  if (!buffer) {
    goto error;
  }

  // FIXME: check for errors
  IVY_UNUSED(fread(buffer, bufferSize, 1, file));
  *size = (uint64_t)bufferSize;

  IVY_UNUSED(fclose(file));
  return buffer;

error:
  if (buffer) {
    ivyFreeMemory(allocator, buffer);
  }

  if (file) {
    IVY_UNUSED(fclose(file));
  }

  return NULL;
}

IVY_API VkResult ivyCreateVulkanShader(IvyAnyMemoryAllocator allocator,
    VkDevice device, char const *path, VkShaderModule *shader) {
  uint64_t shaderCodeSizeInBytes;
  char *shaderCode;
  VkResult vulkanResult;
  VkShaderModuleCreateInfo shaderCreateInfo;

  shaderCode =
      ivyLoadFileIntoByteBuffer(allocator, path, &shaderCodeSizeInBytes);
  if (!shaderCode) {
    return VK_ERROR_UNKNOWN;
  }

  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.pNext = NULL;
  shaderCreateInfo.flags = 0;
  shaderCreateInfo.codeSize = shaderCodeSizeInBytes;
  shaderCreateInfo.pCode = (uint32_t *)shaderCode;

  vulkanResult = vkCreateShaderModule(device, &shaderCreateInfo, NULL, shader);
  ivyFreeMemory(allocator, shaderCode);
  return vulkanResult;
}

// FIXME(samuel): validate flags
IVY_API VkResult ivyCreateVulkanPipeline(IvyAnyMemoryAllocator allocator,
    VkDevice device, int32_t width, int32_t height,
    IvyGraphicsProgramPropertyFlags flags, VkSampleCountFlagBits sampleCounts,
    VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
    VkShaderModule vertexShader, VkShaderModule fragmentShader,
    VkPipeline *pipeline) {
  VkResult vulkanResult;
  VkVertexInputBindingDescription vertexInputBindingDescription;
  uint32_t vertexInputAttributesDescriptionCount = 0;
  VkVertexInputAttributeDescription *vertexInputAttributesDescriptions = NULL;
  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
  VkViewport viewport;
  VkRect2D scissor;
  VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
  VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2];
  VkGraphicsPipelineCreateInfo pipelineCreateInfo;

  if (IVY_VERTEX_3_ENABLE & flags) {
    vertexInputAttributesDescriptionCount = 1;
    vertexInputAttributesDescriptions = ivyAllocateMemory(allocator,
        vertexInputAttributesDescriptionCount *
            sizeof(*vertexInputAttributesDescriptions));
  } else if (IVY_VERTEX_332_ENABLE & flags) {
    vertexInputAttributesDescriptionCount = 3;
    vertexInputAttributesDescriptions = ivyAllocateMemory(allocator,
        vertexInputAttributesDescriptionCount *
            sizeof(*vertexInputAttributesDescriptions));
  } else if (IVY_VERTEX_3322444_ENABLE & flags) {
    vertexInputAttributesDescriptionCount = 7;
    vertexInputAttributesDescriptions = ivyAllocateMemory(allocator,
        vertexInputAttributesDescriptionCount *
            sizeof(*vertexInputAttributesDescriptions));
  }

  if (IVY_VERTEX_ENABLE_MASK & flags) {
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.stride = sizeof(IvyGraphicsVertex332);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  }

  if (IVY_VERTEX_3_ENABLE & flags) {
    vertexInputAttributesDescriptions[0].binding = 0;
    vertexInputAttributesDescriptions[0].location = 0;
    vertexInputAttributesDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributesDescriptions[0].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3, position);
  } else if (IVY_VERTEX_332_ENABLE & flags) {
    vertexInputAttributesDescriptions[0].binding = 0;
    vertexInputAttributesDescriptions[0].location = 0;
    vertexInputAttributesDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributesDescriptions[0].offset =
        IVY_OFFSETOF(IvyGraphicsVertex332, position);

    vertexInputAttributesDescriptions[1].binding = 0;
    vertexInputAttributesDescriptions[1].location = 1;
    vertexInputAttributesDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributesDescriptions[1].offset =
        IVY_OFFSETOF(IvyGraphicsVertex332, color);

    vertexInputAttributesDescriptions[2].binding = 0;
    vertexInputAttributesDescriptions[2].location = 2;
    vertexInputAttributesDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributesDescriptions[2].offset =
        IVY_OFFSETOF(IvyGraphicsVertex332, uv);
  } else if (IVY_VERTEX_3322444_ENABLE & flags) {
    vertexInputAttributesDescriptions[0].binding = 0;
    vertexInputAttributesDescriptions[0].location = 0;
    vertexInputAttributesDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributesDescriptions[0].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, position);

    vertexInputAttributesDescriptions[1].binding = 0;
    vertexInputAttributesDescriptions[1].location = 1;
    vertexInputAttributesDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributesDescriptions[1].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, normal);

    vertexInputAttributesDescriptions[2].binding = 0;
    vertexInputAttributesDescriptions[2].location = 2;
    vertexInputAttributesDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributesDescriptions[2].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, uv0);

    vertexInputAttributesDescriptions[3].binding = 0;
    vertexInputAttributesDescriptions[3].location = 2;
    vertexInputAttributesDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributesDescriptions[3].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, uv1);

    vertexInputAttributesDescriptions[4].binding = 0;
    vertexInputAttributesDescriptions[4].location = 1;
    vertexInputAttributesDescriptions[4].format =
        VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributesDescriptions[4].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, joint0);

    vertexInputAttributesDescriptions[5].binding = 0;
    vertexInputAttributesDescriptions[5].location = 2;
    vertexInputAttributesDescriptions[5].format =
        VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributesDescriptions[5].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, weight0);

    vertexInputAttributesDescriptions[6].binding = 0;
    vertexInputAttributesDescriptions[6].location = 2;
    vertexInputAttributesDescriptions[6].format =
        VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributesDescriptions[6].offset =
        IVY_OFFSETOF(IvyGraphicsVertex3322444, color0);
  }

  vertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.pNext = NULL;
  vertexInputStateCreateInfo.flags = 0;
  if (IVY_VERTEX_ENABLE_MASK & flags) {
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions =
        &vertexInputBindingDescription;
  } else {
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = NULL;
  }
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      vertexInputAttributesDescriptionCount;
  vertexInputStateCreateInfo.pVertexAttributeDescriptions =
      vertexInputAttributesDescriptions;

  inputAssemblyStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.pNext = NULL;
  inputAssemblyStateCreateInfo.flags = 0;
  inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = width;
  viewport.height = height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = width;
  scissor.extent.height = height;

  viewportStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.pNext = NULL;
  viewportStateCreateInfo.flags = 0;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissor;

  rasterizationStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCreateInfo.pNext = NULL;
  rasterizationStateCreateInfo.flags = 0;
  if (IVY_DEPTH_ENABLE & flags) {
    rasterizationStateCreateInfo.depthClampEnable = VK_TRUE;
  } else {
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
  }

  rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;

  if (IVY_POLYGON_MODE_FILL & flags) {
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  } else if (IVY_POLYGON_MODE_LINE & flags) {
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
  } else {
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  }

  if (IVY_CULL_BACK & flags && IVY_CULL_FRONT & flags) {
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
  } else if (IVY_CULL_FRONT & flags) {
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
  } else if (IVY_CULL_BACK & flags) {
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  } else {
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
  }

  if (IVY_FRONT_FACE_COUNTER_CLOCKWISE & flags) {
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  } else if (IVY_FRONT_FACE_COUNTER_CLOCKWISE & flags) {
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  } else {
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  }

  rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0F;
  rasterizationStateCreateInfo.depthBiasClamp = 0.0F;
  rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0F;
  rasterizationStateCreateInfo.lineWidth = 1.0F;

  multisampleStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCreateInfo.pNext = NULL;
  multisampleStateCreateInfo.flags = 0;
  multisampleStateCreateInfo.rasterizationSamples = sampleCounts;
  multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
  multisampleStateCreateInfo.minSampleShading = 1.0F;
  multisampleStateCreateInfo.pSampleMask = NULL;
  multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

  if (IVY_BLEND_ENABLE & flags) {
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  } else {
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  }
  colorBlendAttachmentState.colorWriteMask = 0;
  colorBlendAttachmentState.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
  colorBlendAttachmentState.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
  colorBlendAttachmentState.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
  colorBlendAttachmentState.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;

  colorBlendStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.pNext = NULL;
  colorBlendStateCreateInfo.flags = 0;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendStateCreateInfo.blendConstants[0] = 0.0F;
  colorBlendStateCreateInfo.blendConstants[1] = 0.0F;
  colorBlendStateCreateInfo.blendConstants[2] = 0.0F;
  colorBlendStateCreateInfo.blendConstants[3] = 0.0F;

  depthStencilStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCreateInfo.pNext = NULL;
  depthStencilStateCreateInfo.flags = 0;
  if (IVY_DEPTH_ENABLE & flags) {
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
  } else {
    depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
  }
  depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.front.failOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.front.passOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.front.depthFailOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_NEVER;
  depthStencilStateCreateInfo.front.compareMask = 0;
  depthStencilStateCreateInfo.front.writeMask = 0;
  depthStencilStateCreateInfo.front.reference = 0;
  depthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_ZERO;
  depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_NEVER;
  depthStencilStateCreateInfo.back.compareMask = 0;
  depthStencilStateCreateInfo.back.writeMask = 0;
  depthStencilStateCreateInfo.back.reference = 0;
  depthStencilStateCreateInfo.minDepthBounds = 0.0F;
  depthStencilStateCreateInfo.maxDepthBounds = 1.0F;

  shaderStageCreateInfos[0].sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfos[0].pNext = NULL;
  shaderStageCreateInfos[0].flags = 0;
  shaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStageCreateInfos[0].module = vertexShader;
  shaderStageCreateInfos[0].pName = "main";
  shaderStageCreateInfos[0].pSpecializationInfo = NULL;

  shaderStageCreateInfos[1].sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageCreateInfos[1].pNext = NULL;
  shaderStageCreateInfos[1].flags = 0;
  shaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStageCreateInfos[1].module = fragmentShader;
  shaderStageCreateInfos[1].pName = "main";
  shaderStageCreateInfos[1].pSpecializationInfo = NULL;

  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.pNext = NULL;
  pipelineCreateInfo.flags = 0;
  pipelineCreateInfo.stageCount = IVY_ARRAY_LENGTH(shaderStageCreateInfos);
  pipelineCreateInfo.pStages = shaderStageCreateInfos;
  pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
  pipelineCreateInfo.pTessellationState = NULL;
  pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
  pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
  pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
  pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
  pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
  pipelineCreateInfo.pDynamicState = NULL;
  pipelineCreateInfo.layout = pipelineLayout;
  pipelineCreateInfo.renderPass = renderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;

  vulkanResult = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
      &pipelineCreateInfo, NULL, pipeline);

  ivyFreeMemory(allocator, vertexInputAttributesDescriptions);

  return vulkanResult;
}

IVY_API IvyCode ivyCreateGraphicsProgram(IvyAnyMemoryAllocator allocator,
    IvyGraphicsDevice *device, VkSampleCountFlagBits samples,
    VkRenderPass renderPass, VkPipelineLayout pipelineLayout,
    int32_t viewportWidth, int32_t viewportHeight,
    char const *vertexShaderPath, char const *fragmentShaderPath,
    IvyGraphicsProgramPropertyFlags flags, IvyGraphicsProgram *program) {
  VkResult vulkanResult;
  IvyCode ivyCode;
  VkShaderModule vertexShader = VK_NULL_HANDLE;
  VkShaderModule fragmentShader = VK_NULL_HANDLE;

  IVY_ASSERT(program);
  IVY_ASSERT(vertexShaderPath);
  IVY_ASSERT(fragmentShaderPath);

  IVY_MEMSET(program, 0, sizeof(*program));

  vulkanResult = ivyCreateVulkanShader(allocator, device->logicalDevice,
      vertexShaderPath, &vertexShader);
  if (vulkanResult) {
    ivyCode = ivyVulkanResultAsIvyCode(vulkanResult);
    goto error;
  }

  vulkanResult = ivyCreateVulkanShader(allocator, device->logicalDevice,
      fragmentShaderPath, &fragmentShader);
  if (vulkanResult) {
    ivyCode = ivyVulkanResultAsIvyCode(vulkanResult);
    goto error;
  }

  vulkanResult = ivyCreateVulkanPipeline(allocator, device->logicalDevice,
      viewportWidth, viewportHeight, flags, samples, renderPass,
      pipelineLayout, vertexShader, fragmentShader, &program->pipeline);
  if (vulkanResult) {
    ivyCode = ivyVulkanResultAsIvyCode(vulkanResult);
    goto error;
  }

  vkDestroyShaderModule(device->logicalDevice, fragmentShader, NULL);
  vkDestroyShaderModule(device->logicalDevice, vertexShader, NULL);

  return IVY_OK;

error:
  ivyDestroyGraphicsProgram(device, program);

  if (fragmentShader) {
    vkDestroyShaderModule(device->logicalDevice, fragmentShader, NULL);
  }

  if (vertexShader) {
    vkDestroyShaderModule(device->logicalDevice, vertexShader, NULL);
  }

  // FIXME: check for when the path was not found
  return ivyCode;
}

IVY_API void ivyDestroyGraphicsProgram(IvyGraphicsDevice *device,
    IvyGraphicsProgram *program) {
  if (program->pipeline) {
    vkDestroyPipeline(device->logicalDevice, program->pipeline, NULL);
    program->pipeline = VK_NULL_HANDLE;
  }
}

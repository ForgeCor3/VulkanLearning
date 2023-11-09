#ifndef MODELOBJ_H
#define MODELOBJ_H

#include "Vertex.h"

struct ModelOBJ
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};


#endif // MODELOBJ_H
#ifndef TRIANGLEDATA_H
#define TRIANGLEDATA_H

#include "Vertex.h"
#include <vector>

namespace triangleData
{
    const std::vector<Vertex> triangle = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
}//namespace triangleData

#endif //TRIANGLEDATA_H
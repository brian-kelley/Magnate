#ifndef __GLM_HEADERS__
#define __GLM_HEADERS__

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/string_cast.hpp"

std::ostream& operator<<(std::ostream& os, glm::vec2& vec);
std::ostream& operator<<(std::ostream& os, glm::vec3& vec);

#endif
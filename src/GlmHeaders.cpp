#include "GlmHeaders.h"

using namespace std;
using namespace glm;

ostream& operator<<(ostream& os, vec2& vec)
{
    os << "<" << vec.x << ", " << vec.y << ">";
    return os;
}

ostream& operator<<(ostream& os, vec3& vec)
{
    os << "<" << vec.x << ", " << vec.y << ", " << vec.z << ">";
    return os;
}
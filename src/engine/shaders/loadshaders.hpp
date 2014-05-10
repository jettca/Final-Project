#include <string>
#include <vector>
#include "includes/gl_include.h"

namespace engine
{
    GLuint loadshaders(std::string vertfile, std::string fragfile,
            std::vector<std::string> in_attributes,
            std::vector<std::string> out_attributes);
}

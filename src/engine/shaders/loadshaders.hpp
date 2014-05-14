#include <string>
#include <vector>
#include "includes/gl_include.h"

/* Function for loading shaders into program.
 * Attribute locations are determined by ordering in vector.
 */
namespace engine
{
    GLuint loadshaders(std::string vertfile, std::string fragfile,
            std::vector<std::string> in_attributes,
            std::vector<std::string> out_attributes);
}


/* Fragment shader for evaluating blinn phong
 * shading with a polar coordinate shadowmap
 */

#version 150

// Values for blinn-phong shading
in vec3 fragNormal;
in vec3 lightDir;
in vec3 halfViewDir;
in vec3 shadowPos;

// Light and shadow data
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 shadowmapSize;
uniform sampler2D shadowmap;

out vec3 color;

void main()
{
    float diffuse = dot(lightDir, fragNormal);
    float specular = pow(dot(fragNormal, halfViewDir), 100.0f);

    float shadowScale = 0;
    int size = 4;
    int hfsize = size/2;
    float xshift = 1.0f/shadowmapSize.x;
    float yshift = 1.0f/shadowmapSize.y;
    float offset = .00005;
    
    // Evaluate filter
    vec2 centerUV = (shadowPos.xy + 1.0f)/2.0f;
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            vec2 UV;
            UV.x = centerUV.x + xshift*(i - hfsize);
            UV.y = centerUV.y + yshift*(i - hfsize);
            if(pow(texture(shadowmap, UV).r, 2) + offset > shadowPos.z)
            {
                shadowScale = shadowScale + 1;
            }
        }
    }
    shadowScale = shadowScale/(size*size);

    // Apply scaled blinn phong shading
    vec3 ambient = vec3(0.2f);
    color = shadowScale*(lightDiffuse*diffuse + lightSpecular*specular + ambient);
}

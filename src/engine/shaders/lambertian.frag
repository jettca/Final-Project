#version 150

in vec3 fragNormal;
in vec3 lightDir;
in vec3 halfViewDir;
in vec3 shadowPos;

uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 shadowmapSize;
uniform sampler2D shadowmap;

out vec3 color;

void main()
{
    float diffuse = dot(lightDir, fragNormal);
    float specular = pow(dot(fragNormal, halfViewDir), 100.0f);

    float scalar = 0;
    int size = 4;
    int hfsize = size/2;
    float xshift = 1.0f/shadowmapSize.x;
    float yshift = 1.0f/shadowmapSize.y;
    float offset = .00005;
    
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
                scalar = scalar + 1;
            }
        }
    }
    scalar = scalar/(size*size);
    vec3 ambient = vec3(0.2f);
    color = scalar*(lightDiffuse*diffuse + lightSpecular*specular + ambient);
}

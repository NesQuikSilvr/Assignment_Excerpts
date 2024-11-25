#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        // HW3: You will compute the lighting here.

        vec3 viewLightPos; //Light position in camera coord frame
        vec3 n = normalize(transpose(inverse(mat3(modelview))) * normal); //Normal in camera coord frame

        vec3 v; //Vector pointing towards the viewer
        vec3 h; //Halfway direction between direction to viewer and direction to light
        vec4 sum;

        for(int i = 0; i < nlights; i++) {
            viewLightPos = normalize( position.w * mat3(view) * lightpositions[i].xyz - lightpositions[i].w * mat3(modelview) * position.xyz );
            v = normalize( position.w * mat3(view) * vec3(inverse(view)[3][0], inverse(view)[3][1], inverse(view)[3][2]) - mat3(modelview) * position.xyz );
            h = normalize( v + viewLightPos );

            sum = sum + ( ambient + diffuse * max(dot(n, viewLightPos), 0) + specular * pow(max(dot(n, h), 0), shininess) ) * lightcolors[i];
        }

        fragColor = emision + sum;
    }
}

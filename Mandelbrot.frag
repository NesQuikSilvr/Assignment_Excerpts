#version 330 core

// Inputs to the fragment shader are outputs
// of the same name of the vertex shader
in vec2 canvas_coord; // range [-1,1]x[-1,1]

uniform vec2 center;
uniform float zoom;
uniform int maxiter;

// Output the frag color
out vec4 fragColor;

// HW1: You can define customized functions here,
// e.g. complex multiplications, helper functions
// for colormap etc.

vec2 cprod(vec2 z1, vec2 z2);
int mandelIter(vec2 c, int maxiter);
vec4 calcColor(int count, int maxiter);

void main (void){
    
    vec2 c = center + zoom * canvas_coord;
    // HW1: Your implementation goes here. Compute
    // the value of the Mandelbrot fractal at
    // complex number c.  Then map the value to
    // some color.
    
    int count = mandelIter(c, maxiter);
    fragColor = calcColor(count, maxiter);
}

vec2 cprod(vec2 z1, vec2 z2) {
    float v1 = z1.x * z2.x - z1.y * z2.y;
    float v2 = z1.y * z2.x + z1.x * z2.y;
    return vec2(v1, v2);
}

int mandelIter(vec2 c, int maxiter) {
   int count = 0;
    vec2 z = vec2(0.0, 0.0);
    for(int k = 0; k < maxiter; k++) {
        z = cprod(z, z) + c;
        if(length(z) > 2 || (length(z)) < -2) {
            count = k;
            break;
        }
    }
    return count;
}

vec4 calcColor(int count, int maxiter) {
    float a = float(count);
    float b = float(maxiter);
    return vec4(a/b, a/b, a/b, 1.0);
}
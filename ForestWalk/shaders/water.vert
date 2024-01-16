
#version 330

//  Variabile de intrare (dinspre programul principal);
layout (location = 0) in vec4 in_Position;     //  Se preia din buffer de pe prima pozitie (0) atributul care contine coordonatele;

//  Variabile de iesire;
out vec4 gl_Position;   //  Transmite pozitia actualizata spre programul principal;
out vec3 Normal;
out vec3 Position;
//  Variabile uniforme;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float gametime;

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
    mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
    mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

vec3 getPointAt(vec2 xz){
    float circular = gametime - int (gametime/33)*33;
    float height = noise(xz/8000 * circular) + 100;
    return vec3(xz.x, height, xz.y);
}

void main(void)
{
    vec2 a = in_Position.xz;
    vec2 b = a + vec2(0.1, 0);
    vec2 c = a + vec2(0, 0.1);

    vec3 a_triangle = getPointAt(a);
    vec3 b_triangle = getPointAt(b);
    vec3 c_triangle = getPointAt(c);
    Position = a_triangle;

    vec3 v1 = b_triangle - a_triangle;
    vec3 v2 = c_triangle - a_triangle;
    Normal = normalize(cross(v2, v1));


    vec4 poz = vec4(a_triangle, 1);
    gl_Position = projection*view*model*poz;

//    vec4 low_blue = vec4(0.00, 0.291, 0.970, 0.2);
//    vec4 high_blue = vec4(0.376, 0.560, 0.990, 1);
//
//
//    FragColor = low_blue;
}

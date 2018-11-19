
in vec3 position;
in vec3 color;
in vec3 ver_normal;

varying vec3 frag_normal;
varying vec3 frag_pos;
varying vec3 objectColor;

uniform mat4 model;        //模型矩阵
uniform mat4 view;         //观察矩阵
uniform mat4 projection;   //投影矩阵

uniform mat3 normal_model;  //法向量变换矩阵

void main(void)
{
    frag_normal = normal_model *ver_normal  ;     //将法向量赋予frag_normal，并传到片段着色器
    frag_pos = vec3(model* vec4(position, 1.0));  //将位置传递至片段着色器
    objectColor = color;

    gl_Position = projection * view * model * vec4(position, 1.0);

}

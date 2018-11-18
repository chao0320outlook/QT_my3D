//不能使用layout（location=） 原因不明  两个着色器之间只能使用 varying 传递数据 in out 不行   原因不明
in vec3 position;
in vec2 aTexCoords;

varying vec2 TexCoords;

uniform mat4 model;        //模型矩阵
uniform mat4 view;         //观察矩阵
uniform mat4 projection;   //投影矩阵

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0);
  TexCoords = aTexCoords;
}


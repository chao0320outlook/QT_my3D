
struct Material
{
    vec3 ambient;        //环境光照
    vec3 diffuse;        //漫反射光照
    vec3 specular;       //镜面光照

    float shininess;     //反光度
};
struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

varying vec3 frag_normal;       //法向量
varying vec3 frag_pos;          //位置信息
varying vec3 objectColor;       //物体颜色

uniform vec3 viewpos;          //相机位置

vec3 light_pos_1=vec3( 17.5, 0.0 ,0.0);
vec3 light_pos_2=vec3(-17.5, 0.0 ,0.0);
vec3 light_pos_3=vec3(0.0, 0.0 , 17.5);
vec3 light_pos_4=vec3(0.0, 0.0 ,-17.5);

const float  specularStrength = 0.5;                     //镜面光照因子
const float  ambientStrength = 0.5;                      //环境光照因子

float Diff(vec3 lightpos);
float Spec(vec3 lightpos);

vec3 norm = normalize(frag_normal);                       //法线标准化

Material material;         //材质
Light light_mine;          //光照权重

void main(void)
{
    material.ambient=objectColor;
    material.diffuse=objectColor;
    material.specular=vec3(0.5,0.5,0.5);
    material.shininess=64.0;

    light_mine.ambient=vec3(0.2,0.2,0.2);
    light_mine.diffuse=vec3(0.5,0.5,0.5);
    light_mine.specular=vec3(1.0,1.0,1.0);

    vec3 ambient = light_mine.ambient * material.ambient;                 //环境光照

    float diffu=(Diff(light_pos_1)+Diff(light_pos_2)+ Diff(light_pos_3)+Diff(light_pos_4))*0.7;
    vec3 diffuse = light_mine.diffuse * (diffu * material.diffuse);      //最终漫反射

    float specu=(Spec(light_pos_1) + Spec(light_pos_2) + Spec(light_pos_3)+Spec(light_pos_4))*0.4;
    vec3 specular= light_mine.specular * (specu * material.specular);

    vec3 result = ambient + diffuse + specu;
    gl_FragColor = vec4(result, 1.0);

}

//计算漫反射因子
 float Diff(vec3 lightpos)
 {
     vec3 lightDir = normalize(lightpos - frag_pos);      //光源——物体 方向向量标准化
     return max( dot( lightDir,norm),0.0);                //计算漫反射参数
 }

 //计算镜面反射因子
float Spec(vec3 lightpos)
 {
     vec3 lightDir = normalize(lightpos- frag_pos);
     vec3 viewDir = normalize(viewpos - frag_pos);              //相机——物体 方向向量标准化
     vec3 reflectDir = reflect(-lightDir, norm);                //沿着法线轴的反射向量
     float spec= max(dot(viewDir, reflectDir), 0.0);            //计算镜面反射参数
     return pow(spec , material.shininess);
 }


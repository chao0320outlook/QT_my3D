#ifndef MY_SAMPLE_MODEL_H
#define MY_SAMPLE_MODEL_H

#include "model.h"
#include "transform_3d.h"
//cube                           顶点坐标             纹理
// 前
#define VERTEX_F_1 Vertex(  1.0f,  1.0f,  1.0f, 1.0f, 1.0f  )
#define VERTEX_F_2 Vertex( -1.0f,  1.0f,  1.0f, 0.0f, 1.0f  )
#define VERTEX_F_3 Vertex( -1.0f, -1.0f,  1.0f, 0.0f, 0.0f  )
#define VERTEX_F_4 Vertex(  1.0f, -1.0f,  1.0f, 1.0f, 0.0f  )
// 后
#define VERTEX_B_1 Vertex(  1.0f,  1.0f, -1.0f, 0.0f, 1.0f  )
#define VERTEX_B_2 Vertex( -1.0f,  1.0f, -1.0f, 1.0f, 1.0f  )
#define VERTEX_B_3 Vertex( -1.0f, -1.0f, -1.0f, 1.0f, 0.0f  )
#define VERTEX_B_4 Vertex(  1.0f, -1.0f, -1.0f, 0.0f, 0.0f  )

//左
#define VERTEX_L_1 Vertex( -1.0f,  1.0f,  1.0f, 1.0f, 1.0f  )  //左上前
#define VERTEX_L_2 Vertex( -1.0f,  1.0f, -1.0f, 0.0f, 1.0f  ) //左上后
#define VERTEX_L_3 Vertex( -1.0f, -1.0f, -1.0f, 0.0f, 0.0f  )  //左下后
#define VERTEX_L_4 Vertex( -1.0f, -1.0f,  1.0f, 1.0f, 0.0f  )  //左下前

//右
#define VERTEX_R_1 Vertex(  1.0f,  1.0f,  1.0f,  0.0f, 1.0f  )  //右上前
#define VERTEX_R_2 Vertex(  1.0f,  1.0f, -1.0f,  1.0f, 1.0f  )  //右上后
#define VERTEX_R_3 Vertex(  1.0f, -1.0f, -1.0f,  1.0f, 0.0f  )  //右下后
#define VERTEX_R_4 Vertex(  1.0f, -1.0f,  1.0f,  0.0f, 0.0f  )  //右下前

//上
#define VERTEX_U_1 Vertex( -1.0f,  1.0f,  1.0f,  0.0f, 0.0f  )  //上左前
#define VERTEX_U_2 Vertex( -1.0f,  1.0f, -1.0f,  0.0f, 1.0f  )  //上左后
#define VERTEX_U_3 Vertex(  1.0f,  1.0f, -1.0f,  1.0f, 1.0f  )  //上右后
#define VERTEX_U_4 Vertex(  1.0f,  1.0f,  1.0f,  1.0f, 0.0f  )  //上右前

//下
#define VERTEX_D_1 Vertex( -1.0f, -1.0f,  1.0f,  0.0f, 0.0f  )  //下左前
#define VERTEX_D_2 Vertex( -1.0f, -1.0f, -1.0f,  1.0f, 0.0f  )  //下左后
#define VERTEX_D_3 Vertex(  1.0f, -1.0f, -1.0f,  1.0f, 1.0f  )  //下右后
#define VERTEX_D_4 Vertex(  1.0f, -1.0f,  1.0f,  0.0f, 1.0f  )  //下右前

static const Vertex static_my_box[36] =
{
    // Face 1 (Front)
    VERTEX_F_1, VERTEX_F_2, VERTEX_F_3,
    VERTEX_F_3, VERTEX_F_4, VERTEX_F_1,
    // Face 2 (Back)
    VERTEX_B_4, VERTEX_B_2, VERTEX_B_1,
    VERTEX_B_2, VERTEX_B_4, VERTEX_B_3,
    // Face 3 (Top)
    VERTEX_U_4, VERTEX_U_2, VERTEX_U_1,
    VERTEX_U_2, VERTEX_U_4, VERTEX_U_3,
    // Face 4 (Bottom)
    VERTEX_D_1, VERTEX_D_2, VERTEX_D_3,
    VERTEX_D_3, VERTEX_D_4, VERTEX_D_1,
    // Face 5 (Left)
    VERTEX_L_1, VERTEX_L_2, VERTEX_L_3,
    VERTEX_L_3, VERTEX_L_4, VERTEX_L_1,
    // Face 6 (Right)
    VERTEX_R_1, VERTEX_R_4, VERTEX_R_3,
    VERTEX_R_1, VERTEX_R_3, VERTEX_R_2
};



#endif // MY_SAMPLE_MODEL_H

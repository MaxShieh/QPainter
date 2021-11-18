#ifndef WALL_H
#define WALL_H

#include <stdlib.h>
#include <cstring>

typedef unsigned int uint ;

struct Brick
{
    float x;    //x坐标
    float y;    //y坐标
    int width;  //砖长
    int height; //砖高
    int rank;   //行序号
    int column; //列序号
    bool full;  //是否整砖
};

/**
 * @brief 墙体计算输入参数
 */
struct WallParameterIn_st
{
    uint uiWallLength;	//墙长
    uint uiWallHeight;	//墙高
    uint uiJointThickness; //灰缝厚度
    uint uiTopJointThickness;	//顶缝高度
    uint uiBrickLength;	//砖长
    uint uiBrickHeight;	//砖高
    //uint uiBrickThickness;	//砖厚
};

/**
 * @brief 墙体计算输出参数
 */     
struct WallParameterOut_st
{
    uint uiFullBrickLayer;	//整砖层数
    uint uiTotalBrickLayer; //包括顶层非整砖在内的所有砖的层数
    uint uiTopBrickHeight;	//顶砖高度
    uint uiFullNum_Layer1;	//第一层整砖数量
    uint uiBrokenNum_Layer1;   //第一层非整砖数量
    uint uiBrokenLenOfLayer1;	//第一层非整砖长度
    uint uiFullNum_Layer2;	//第二层整砖数量
    uint uiBrokenNum_Layer2;	//第二层非整砖数量
    uint uiBrokenLen1OfLayer2;	//第二层第一块非整砖长度
    uint uiBrokenLen2OfLayer2;	//第二层第二块非整砖长度
    Brick *bricks;  //所有砖的数据
};

/**
 * @brief 
 * 墙体参数计算函数
 * @param in const WallParameterIn_st& 墙体输入参数
 * @param out WallParameterOut_st& 墙体输出参数
 * @return void
 */
void WallCalculate(const WallParameterIn_st& in, WallParameterOut_st& out)
{
    //整砖层数计算(需要保证顶部灰缝)
    out.uiFullBrickLayer = (in.uiWallHeight - in.uiTopJointThickness) / (in.uiBrickHeight + in.uiJointThickness);

    //最顶部砌体高度
    out.uiTopBrickHeight = (in.uiWallHeight - in.uiTopJointThickness) % (in.uiBrickHeight + in.uiJointThickness) 
                                    - in.uiJointThickness;

    out.uiTotalBrickLayer = out.uiFullBrickLayer + (out.uiTopBrickHeight == 0 ? 0 : 1);
    out.uiTopBrickHeight = (out.uiTopBrickHeight > 0) ? out.uiTopBrickHeight : 0;   //如果小于0，设置为0
        
    //第一排整砖数量计算
    out.uiFullNum_Layer1 = in.uiWallLength / (in.uiBrickLength + in.uiJointThickness);

    //第一排非整砖数量
    out.uiBrokenNum_Layer1 = 1;

    //第一排非整砖长度
    out.uiBrokenLenOfLayer1 = in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * out.uiFullNum_Layer1
                                 - 2 * in.uiJointThickness;
    out.uiBrokenLenOfLayer1 = (out.uiBrokenLenOfLayer1 > 0) ? out.uiBrokenLenOfLayer1 : 0;   //如果小于0，设置为0

    //第二排整砖、非整砖数量
    if (out.uiBrokenLenOfLayer1 < in.uiBrickLength / 3.0)
    {
        //
        out.uiFullNum_Layer2 = out.uiFullNum_Layer1 - 1;
        out.uiBrokenNum_Layer2 = 2;
        out.uiBrokenLen1OfLayer2 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
    }
    else if( in.uiBrickLength / 3.0 * 2.0> out.uiBrokenLenOfLayer1 && out.uiBrokenLenOfLayer1 > in.uiBrickLength / 3.0 )
    {
        out.uiFullNum_Layer2 = out.uiFullNum_Layer1;
        out.uiBrokenNum_Layer2 = 1;
        out.uiBrokenLen1OfLayer2 = out.uiBrokenLenOfLayer1;    //第二排第一块非整砖长度
    }
    else
    {
        out.uiFullNum_Layer2 = out.uiFullNum_Layer1 - 1;
        out.uiBrokenNum_Layer2 = 2;
        out.uiBrokenLen1OfLayer2 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
    }

    //第二排第二块非整砖长度
    long temp = (long)(in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * out.uiFullNum_Layer2 
                    - (out.uiBrokenLen1OfLayer2 + in.uiJointThickness)
                    - 2 * in.uiJointThickness);
    if (temp < 0)
        out.uiBrokenLen2OfLayer2 = 0;
    else
        out.uiBrokenLen2OfLayer2 = temp;

////-----------------考虑最后一块砖小于80的情况---------------
//    uint& len = out.uiBrokenLenOfLayer1;   //不想写这么长的名字，定义一个引用
//    if (len < 80)
//    {
//        out.uiFullNum_Layer1 -= 1;
//        out.uiBrokenNum_Layer1 = 2;
//        len = (len + in.uiBrickLength) / 2.0;
//    }

    //-------------------记录所有砖块的信息-----------------------
    int totalBricks_Layer1 = out.uiFullNum_Layer1 + out.uiBrokenNum_Layer1;
    int totalBricks_Layer2 = out.uiFullNum_Layer2 + out.uiBrokenNum_Layer2;
    //如果有顶层砖，也需包含上 
    int totalBricks = out.uiTotalBrickLayer / 2 * (totalBricks_Layer1 + totalBricks_Layer2)
                                + out.uiTotalBrickLayer % 2 * totalBricks_Layer1;                           

    //创建动态内存空间-------------------------------记住以后要delete[]
    //out.bricks = new Brick[totalBricks];
    out.bricks = (Brick*)realloc(out.bricks, totalBricks * sizeof(Brick));
    memset(out.bricks, 0, totalBricks * sizeof(Brick));

    float x(in.uiJointThickness), y(in.uiJointThickness);
    int rank(1), column(1);
    int width(in.uiBrickLength);
    const int height(in.uiBrickHeight);
    int brickIndex = 0;

    while(rank <= out.uiTotalBrickLayer)
    {
        width = (rank % 2 == 0 && column == 1 ? out.uiBrokenLen1OfLayer2 : in.uiBrickLength);

        while (column <= (rank % 2 == 0 ? totalBricks_Layer2 : totalBricks_Layer1) )
        {
            Brick *pBrick = &out.bricks[brickIndex];
            pBrick->x = x;
            pBrick->y = y;
            pBrick->width = width;
            pBrick->height = (rank == out.uiFullBrickLayer + 1) ? (int)out.uiTopBrickHeight : (int)height;
            pBrick->rank = rank;
            pBrick->column = column;

            x += width + in.uiJointThickness;
            column += 1;    //列计数加1
            brickIndex += 1;    //下标加1

            width = in.uiBrickLength;   //从第二块砖开始（砖的长度为整砖长度）
            if (rank % 2 == 0)
            {
                width = (column > out.uiFullNum_Layer2 + 1) ? out.uiBrokenLen2OfLayer2 : width;
            }
            else
            {
                width = column > out.uiFullNum_Layer1 ? out.uiBrokenLenOfLayer1 : width;
            }
        }

        rank ++;    //轮询完一行后，行计数+1
        column = 1; //轮询完一行后，列计数重置为1
        x = in.uiJointThickness; //轮询完一行后，第一块砖的坐标重置
        y += ( height + in.uiJointThickness);
    }
}
#endif // WALL_H

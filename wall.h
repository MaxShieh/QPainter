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
    uint uiTotalBrickNum;
    Brick *bricks;  //所有砖的数据
};



    //整砖层数计算(需要保证顶部灰缝)
    inline static uint calFullBrickLayer(const WallParameterIn_st& in)
    {
        uint layer = (in.uiWallHeight - in.uiTopJointThickness) / (in.uiBrickHeight + in.uiJointThickness);
        return layer;
    }

     //最顶部砌体高度
    inline static uint calTopBrickHeight(const WallParameterIn_st& in)
    {

        uint uiTopBrickHeight = (in.uiWallHeight - in.uiTopJointThickness) % (in.uiBrickHeight + in.uiJointThickness)
                                    - in.uiJointThickness;
        uiTopBrickHeight = (uiTopBrickHeight > 0) ? uiTopBrickHeight : 0;//如果小于0，设置为0
        return uiTopBrickHeight;
    }

    //
    inline static uint calTotalBrickLayer(uint uiFullBrickLayer, uint uiTopBrickHeight)
    {
        uint uiTotalBrickLayer = uiFullBrickLayer + (uiTopBrickHeight == 0 ? 0 : 1);
        return uiTotalBrickLayer;
    }

    inline static uint calFullNum_Layer1(const WallParameterIn_st& in)
    {
        uint uiFullNum_Layer1 = in.uiWallLength / (in.uiBrickLength + in.uiJointThickness);
        return uiFullNum_Layer1;
    }

    inline static uint calBrokenLen_Layer1(const WallParameterIn_st& in, uint  uiFullNum_Layer1)
    {
        uint uiBrokenLenOfLayer1;
        uiBrokenLenOfLayer1 = in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * uiFullNum_Layer1
                                     - 2 * in.uiJointThickness;
        uiBrokenLenOfLayer1 = (uiBrokenLenOfLayer1 > 0) ? uiBrokenLenOfLayer1 : 0;   //如果小于0，设置为0
        return uiBrokenLenOfLayer1;
    }

    //
    inline static uint calBrokenNum_Layer1(uint uiBrokenLenOfLayer1)
    {
        uint uiBrokenNum = (uiBrokenLenOfLayer1 > 0) ? 1 : 0;
        return uiBrokenNum;
    }

    //
    inline static void calBrokenLenLayer2(const WallParameterIn_st& in, uint uiBrokenLenOfLayer1,  uint& uiFullNum, uint& uiBrokenNum, uint& uiBrokenLen1, uint& uiBrokenLen2)
    {

        //第二排整砖、非整砖数量
        if (uiBrokenLenOfLayer1 < in.uiBrickLength / 3.0)
        {
            uiFullNum= calFullNum_Layer1(in) - 1;
            uiBrokenLen1 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
            uiBrokenNum = 2;

        }
        else if( in.uiBrickLength / 3.0 * 2.0> uiBrokenLenOfLayer1 && uiBrokenLenOfLayer1 > in.uiBrickLength / 3.0 )
        {
            uiFullNum = calFullNum_Layer1(in);
            uiBrokenLen1 = uiBrokenLenOfLayer1;    //第二排第一块非整砖长度
            uiBrokenNum = 1;
        }
        else
        {
            uiFullNum = calFullNum_Layer1(in) - 1;
            uiBrokenLen1 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
            uiBrokenNum = 2;
        }

        //第二排第二块非整砖长度
        long temp = (long)(in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * uiFullNum
                        - (uiBrokenLen1+ in.uiJointThickness)
                        - 2 * in.uiJointThickness);
        if (temp < 0)
            uiBrokenLen2 = 0;
        else
            uiBrokenLen2 = temp;
    }


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
    out.uiFullBrickLayer = calFullBrickLayer(in);

    //最顶部砌体高度
    out.uiTopBrickHeight = calTopBrickHeight(in);

    out.uiTotalBrickLayer = calTotalBrickLayer(out.uiFullBrickLayer, out.uiTopBrickHeight);
    
    //第一排整砖数量计算
    out.uiFullNum_Layer1 = calFullNum_Layer1(in);

    //第一排非整砖长度
    out.uiBrokenLenOfLayer1 = calBrokenLen_Layer1(in, out.uiFullBrickLayer);

    //第一排非整砖数量
    out.uiBrokenNum_Layer1 = calBrokenNum_Layer1(out.uiBrokenLenOfLayer1);

     //第二排整砖、非整砖数量
    calBrokenLenLayer2(in, out.uiBrokenLenOfLayer1, out.uiFullNum_Layer2, out.uiBrokenNum_Layer2, out.uiBrokenLen1OfLayer2, out.uiBrokenLen2OfLayer2);

////-----------------考虑最后一块砖小于80的情况---------------
//    uint& len = out.uiBrokenLenOfLayer1;   //不想写这么长的名字，定义一个引用
//    if (len < 80)
//    {
//        out.uiFullNum_Layer1 -= 1;
//        out.uiBrokenNum_Layer1 = 2;
//        len = (len + in.uiBrickLength) / 2.0;
//    }


    int totalBricks_Layer1 = out.uiFullNum_Layer1 + out.uiBrokenNum_Layer1;
    int totalBricks_Layer2 = out.uiFullNum_Layer2 + out.uiBrokenNum_Layer2;
    //如果有顶层砖，也需包含上
    out.uiTotalBrickNum = out.uiTotalBrickLayer / 2 * (totalBricks_Layer1 + totalBricks_Layer2)
                                + out.uiTotalBrickLayer % 2 * totalBricks_Layer1;


    //创建动态内存空间-------------------------------记住以后要delete[]
    //out.bricks = new Brick[out.uiTotalBrickNum];
    out.bricks = (Brick*)realloc(out.bricks, out.uiTotalBrickNum* sizeof(Brick));
    memset(out.bricks, 0, out.uiTotalBrickNum * sizeof(Brick));

    float x(in.uiJointThickness), y(in.uiJointThickness);
    int rank(1), column(1);
    int width(in.uiBrickLength);
    const int height(in.uiBrickHeight);
    int brickIndex = 0;

    //-------------------记录所有砖块的信息-----------------------
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

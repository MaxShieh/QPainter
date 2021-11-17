#ifndef WALL_H
#define WALL_H

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
    uint uiTopBrickHeight;	//顶砖高度
    uint uiFullBrickNumOfLayer1;	//第一层整砖数量
    uint uiBrokenNumOfLayer1;   //第一层非整砖数量
    uint uiBrokenLenOfLayer1;	//第一层非整砖长度
    uint uiFullBrickNumOfLayer2;	//第二层整砖数量
    uint uiBrokenBrickNumOfLayer2;	//第二层非整砖数量
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
    out.uiTopBrickHeight = (out.uiTopBrickHeight > 0) ? out.uiTopBrickHeight : 0;   //如果小于0，设置为0
        
    //第一排整砖数量计算
    out.uiFullBrickNumOfLayer1 = in.uiWallLength / (in.uiBrickLength + in.uiJointThickness);

    //第一排非整砖数量
    out.uiBrokenNumOfLayer1 = 1;

    //第一排非整砖长度
    out.uiBrokenLenOfLayer1 = in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * out.uiFullBrickNumOfLayer1
                                 - 2 * in.uiJointThickness;
    out.uiBrokenLenOfLayer1 = (out.uiBrokenLenOfLayer1 > 0) ? out.uiBrokenLenOfLayer1 : 0;   //如果小于0，设置为0
    uint& len = out.uiBrokenLenOfLayer1;   //不想写这么长的名字，定义一个引用
    if (len < 80)
    {
        out.uiFullBrickNumOfLayer1 -= 1;
        out.uiBrokenNumOfLayer1 = 2;
        len = (len + in.uiBrickLength) / 2.0;
    }

    //第二排整砖、非整砖数量
    if (out.uiBrokenLenOfLayer1 < in.uiBrickLength / 3.0)
    {
        //
        out.uiFullBrickNumOfLayer2 = (out.uiBrokenNumOfLayer1 == 2) ? out.uiFullBrickNumOfLayer1 : out.uiFullBrickNumOfLayer1 - 1;
        out.uiBrokenBrickNumOfLayer2 = 2;
        out.uiBrokenLen1OfLayer2 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
    }
    else if( in.uiBrickLength / 3.0 * 2.0> out.uiBrokenLenOfLayer1 && out.uiBrokenLenOfLayer1 > in.uiBrickLength / 3.0 )
    {
        out.uiFullBrickNumOfLayer2 = out.uiFullBrickNumOfLayer1;
        out.uiBrokenBrickNumOfLayer2 = 1;
        out.uiBrokenLen1OfLayer2 = out.uiBrokenLenOfLayer1;    //第二排第一块非整砖长度
    }
    else
    {
        out.uiFullBrickNumOfLayer2 = out.uiFullBrickNumOfLayer1 - 1;
        out.uiBrokenBrickNumOfLayer2 = 2;
        out.uiBrokenLen1OfLayer2 = in.uiBrickLength / 2.0; //第二排第一块非整砖长度
    }


    //第二排第二块非整砖长度
    long temp = in.uiWallLength - (in.uiBrickLength + in.uiJointThickness) * out.uiFullBrickNumOfLayer2 
                    - (out.uiBrokenLen1OfLayer2 + in.uiJointThickness)
                    - 2 * in.uiJointThickness;
    if (len < 0)
        out.uiBrokenLen2OfLayer2 = 0;
    else
        out.uiBrokenLen2OfLayer2 = len;
}
#endif // WALL_H

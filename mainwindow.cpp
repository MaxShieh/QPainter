#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wall.h"
#include <iostream>
#include <QtMath>
#include <QPen>
#include "QtGui/QPainter"
#include <QPaintEvent>
#include <QDebug>

WallParameterIn_st wallParamIn_st;
WallParameterOut_st wallParamOut_st;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //设置输入参数
    wallParamIn_st.uiWallHeight = 3000;
    wallParamIn_st.uiWallLength = 2500;
    wallParamIn_st.uiBrickHeight = 200;
    wallParamIn_st.uiBrickLength = 600;
    wallParamIn_st.uiTopJointThickness = 20;
    wallParamIn_st.uiJointThickness = 5;

    //调用墙体计算函数
    WallCalculate(wallParamIn_st, wallParamOut_st);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //释放动态内存
    //delete[] wallParamOut_st.bricks;
    free(wallParamOut_st.bricks);
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)   //QPaintEvent *event
{
    //Q_UNUSED(event);

    struct aa
    {
        int x;
        int y;
        int width;
        int height;
    }a;

    QPainter painter(this);
    // 设置画笔颜色
    painter.setPen(QColor(0, 160, 230));

    //this->resize(this->width(),this->width() * ((float)3000 / 2500));

    //计算绘图比例
    float widthRatio = (float)(this->width() - 20) / wallParamIn_st.uiWallLength;
    float heightRatio = (float)(this->height() - 20) / wallParamIn_st.uiWallHeight;

    QPainterPath path;
//-------------用黑线画出墙体的区域----------------------
    path.clear();
    path.addRect(10, this->height() - 10, wallParamIn_st.uiWallLength * widthRatio, -1 * (int)wallParamIn_st.uiWallHeight * heightRatio);
    QPen pen (QColor(0, 0, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawPath(path);

//------------------------------------------
    path.clear();
    int rank(1);
    int column(1);
    int x(0);
    int y(0);
    int width(0);
    int height(0);
    int brickIndex = 0;
    bool isLayer2 = false;
    while(rank <= wallParamOut_st.uiTotalBrickLayer)
    {
        isLayer2 = (rank % 2 == 0) ? true : false;
        column = 1;
        while (column <= (isLayer2 ? wallParamOut_st.uiFullNum_Layer2 +  wallParamOut_st.uiBrokenNum_Layer2 :
                                    wallParamOut_st.uiFullNum_Layer1 +  wallParamOut_st.uiBrokenNum_Layer1) )
        {
            Brick *pBrick = &wallParamOut_st.bricks[brickIndex];
            
            //注意：无符号数必须先强制转化为带符号数才能得到负数
            x = 10 + pBrick->x * widthRatio;
            y = this->height() - 10 - pBrick->y * heightRatio;
            width = pBrick->width * widthRatio;
            height = -1 * pBrick->height * heightRatio;
            path.setFillRule(Qt::WindingFill);
            path.addRect(x, y, width, height);
//            std::cout<<10 + pBrick->x * widthRatio <<"," << this->height() - 10 - pBrick->y * widthRatio <<"," <<
//                            pBrick->width * widthRatio <<"," <<-pBrick->height * heightRatio << std::endl;
            if (rank == 1 && column == 3)
            {
                a.x = pBrick->x;
                a.y = pBrick->y;
                a.width = pBrick->width;
                a.height = pBrick->height;
            }
            column += 1;    //列计数加1
            brickIndex += 1;
        }
        rank ++;
        column = 1;
    }

//    Brick bricks[75];
//    for (int i = 0; i < 75; i++)
//    {
//        bricks[i] = wallParamOut_st.bricks[i];
//    }
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::red);
    painter.setBrush(QBrush(Qt::Dense5Pattern));
    painter.drawPath(path);

    //高亮一块砖
    path.clear();
    path.moveTo(a.x, a.y);
    path.addRect(10 + a.x * widthRatio, this->height() - 10 - a.y * heightRatio, a.width * widthRatio, -1 * (int)a.height *heightRatio);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::red);
    painter.setBrush(QBrush(QColor(0,0,255,80),Qt::SolidPattern));
    painter.drawPath(path);

}

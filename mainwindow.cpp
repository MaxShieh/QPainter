#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wall.h"
#include <QtMath>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)   //QPaintEvent *event
{
    //Q_UNUSED(event);

    QEvent::Type type = event->type();
    qDebug() << QString(type);

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

    // 设置字体：微软雅黑、点大小50、斜体
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(50);
    font.setItalic(true);
    painter.setFont(font);

    // 绘制文本
    //painter.drawText(rect(), Qt::AlignCenter, "Qt");

    this->resize(this->width(),this->width() * ((float)3000 / 2500));
    WallParameterIn_st wallParamIn_st;
    WallParameterOut_st wallParamOut_st;
    //设置输入参数
    wallParamIn_st.uiWallHeight = 3000;
    wallParamIn_st.uiWallLength = 2500;
    wallParamIn_st.uiBrickHeight = 200;
    wallParamIn_st.uiBrickLength = 600;
    wallParamIn_st.uiTopJointThickness = 20;
    wallParamIn_st.uiJointThickness = 5;

    //计算绘图比例
    float widthRatio = (float)(this->width() - 20) / wallParamIn_st.uiWallLength;
    float heightRatio = (float)(this->height() - 20) / wallParamIn_st.uiWallHeight;
    //调用墙体计算函数
    WallCalculate(wallParamIn_st, wallParamOut_st);

    float x(10 + wallParamIn_st.uiJointThickness * widthRatio), y(this->height() - 10 - wallParamIn_st.uiJointThickness * heightRatio);
    int rank(1);
    int column(1);
    int width(wallParamIn_st.uiBrickLength);
    const int height(wallParamIn_st.uiBrickHeight);

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
    while(rank <= wallParamOut_st.uiFullBrickLayer + (wallParamOut_st.uiTopBrickHeight == 0 ? 0 : 1))
    {
        width = (rank % 2 == 0 && column == 1 ? wallParamOut_st.uiBrokenLen1OfLayer2 : wallParamIn_st.uiBrickLength);

        while (column <= (rank % 2 == 0 ?
                         wallParamOut_st.uiFullBrickNumOfLayer2 + wallParamOut_st.uiBrokenBrickNumOfLayer2 +1
                         : wallParamOut_st.uiFullBrickNumOfLayer1 + 1 + (wallParamOut_st.uiBrokenLenOfLayer1 == 0?0:1)) )
        {
            path.moveTo(x,y);
            path.setFillRule(Qt::WindingFill);

            if(rank == wallParamOut_st.uiFullBrickLayer + 1)
                //注意：无符号数必须先强制转化为带符号数才能得到负数
                path.addRect(x,y,width * widthRatio, -1 *(int)wallParamOut_st.uiTopBrickHeight* heightRatio);
            else
                path.addRect(x,y,width * widthRatio, -height * heightRatio);

            if (rank == 1 && column == 3)
            {
                a.x = x;
                a.y = y;
                a.width = width;
                a.height = height;
            }

            x += (width + wallParamIn_st.uiJointThickness) * widthRatio;
            column += 1;//列计数加1

            width = wallParamIn_st.uiBrickLength;
            if(column == (rank % 2 == 0 ? wallParamOut_st.uiFullBrickNumOfLayer2 + wallParamOut_st.uiBrokenBrickNumOfLayer2 + 1
                          : wallParamOut_st.uiFullBrickNumOfLayer1 + (wallParamOut_st.uiBrokenLenOfLayer1 == 0?0:1) ))
            {
                width = (rank % 2 == 0 ? wallParamOut_st.uiBrokenLen2OfLayer2 : wallParamOut_st.uiBrokenLenOfLayer1);
            }
            int a =20;
        }
        rank ++;
        column = 1;
        x = 10 + wallParamIn_st.uiJointThickness * widthRatio;
        y -= ( height + wallParamIn_st.uiJointThickness) * heightRatio;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::red);
    painter.setBrush(QBrush(Qt::Dense5Pattern));
    painter.drawPath(path);


   path.clear();
   path.moveTo(a.x, a.y);
   path.addRect(a.x, a.y, a.width * widthRatio, -1 * (int)a.height *heightRatio);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setPen(Qt::red);
   painter.setBrush(QBrush(QColor(0,0,255,80),Qt::SolidPattern));
   painter.drawPath(path);

}

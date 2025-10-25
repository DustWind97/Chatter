#include "bubbleframe.h"
#include <QPaintEvent>
#include <QPainter>

const int WIDTH_TRIANGLE = 8;//三角宽，斜三角横向的那条边长度

BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent)
    :QFrame(parent), m_role(role), m_margin(3)
{
    m_pHLayout = new QHBoxLayout();

    if( m_role == ChatRole::Self)//角色为自己
    {
        m_pHLayout->setContentsMargins( m_margin, m_margin, WIDTH_TRIANGLE + m_margin , m_margin );//左上右下
    }
    else//角色为其他人
    {
        m_pHLayout->setContentsMargins( WIDTH_TRIANGLE + m_margin, m_margin, m_margin , m_margin );//左上右下
    }

    this->setLayout(m_pHLayout);
}

BubbleFrame::~BubbleFrame()
{

}

void BubbleFrame::setMargin(int margin)
{
    Q_UNUSED(margin);
    // m_margin = margin;
}

void BubbleFrame::setWidget(QWidget *wid)
{
    if(m_pHLayout->count() > 0)
    {
        return;
    }
    else
    {
        m_pHLayout->addWidget(wid);
    }
}

//重写绘制事件绘制气泡框 在Widget中绘制
void BubbleFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);//无画笔，绘制出来的东西是没有轨迹的

    //角色为自己
    if(m_role == ChatRole::Self)
    {
        QColor bk_color(158,234,106);
        painter.setBrush(QBrush(bk_color));
        //画气泡
        QRect bk_rect = QRect(0, 0, this->width() - WIDTH_TRIANGLE, this->height());//矩形框左上坐标和矩形框右下坐标
        painter.drawRoundedRect(bk_rect,5,5);//倒角
        //画三角
        QPointF points[3] = {
            QPointF(bk_rect.x() + bk_rect.width(), 12),//直角顶点
            QPointF(bk_rect.x() + bk_rect.width(), 12 + WIDTH_TRIANGLE + 2),//右边顶点
            QPointF(bk_rect.x() + bk_rect.width() + WIDTH_TRIANGLE, 10 + WIDTH_TRIANGLE - WIDTH_TRIANGLE / 2),//下方顶点
        };
        painter.drawPolygon(points, 3);//将几个点连接
    }
    else//角色为他人
    {
        //QColor bk_color(Qt::white); //设置颜色
        QColor bk_color(176,224,230);//粉蓝色
        painter.setBrush(QBrush(bk_color));//设置笔刷颜色
        //画气泡
        QRect bk_rect = QRect( WIDTH_TRIANGLE, 0, this->width() - WIDTH_TRIANGLE, this->height());
        painter.drawRoundedRect(bk_rect,5,5);//倒角
        //画小三角
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),//左边顶点
            QPointF(bk_rect.x(), 10 + WIDTH_TRIANGLE + 2),//直角顶点
            QPointF(bk_rect.x() - WIDTH_TRIANGLE, 10 + WIDTH_TRIANGLE - WIDTH_TRIANGLE / 2),//下方顶点
        };
        painter.drawPolygon(points, 3);//将几个点连接
    }

    return QFrame::paintEvent(event);
}

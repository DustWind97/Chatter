#include "chatview.h"
#include <QScrollBar>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>

//构造
ChatView::ChatView(QWidget *parent):QWidget(parent)
    ,isAppended(false)
{
    /*chatview内部层级结构
     *
     * chatview->pMainLayout->m_pScrollArea->wid->pHLayout_1->上面为很多消息体new Widget()为比例
     *
     */

    //创建一个主布局铺满整个widget
    QVBoxLayout *pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    pMainLayout->setContentsMargins(0, 0, 0, 0); //参数顺序：左, 上, 右, 下

    //创建一个滚动区域并放置到pMainLayout中
    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainLayout->addWidget(m_pScrollArea);

    //创建单条聊天记录的Widget
    QWidget *wid = new QWidget(this);//此处this可写可不写
    wid->setObjectName("chat_bg");
    wid->setAutoFillBackground(true);//允许子控件自动填充背景

    //创建子布局并放入滚动区域中
    QVBoxLayout *pVLayout_1 = new QVBoxLayout();
    pVLayout_1->addWidget(new QWidget(),100000);//十万为伸缩因子添加以后动态撑住
    //这个new Widget就相当于一个弹簧，在这个上面加的才是消息体，弹簧迫使消息体一条一条规则分布，而不会消息体一下子布满整个画面
    wid->setLayout(pVLayout_1);
    m_pScrollArea->setWidget(wid);

    //关掉垂直滚动条显示，但是垂直滚动条滚动时候让它做一些事情
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();//将滚动区域的垂直滚动条存下来方便使用
    connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);//滚动条范围变化

    //将垂直的ScrollBar放置到上边的横向放置，而不是纵向放置
    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout_2->setContentsMargins(0, 0, 0, 0); //参数顺序：左, 上, 右, 下
    m_pScrollArea->setLayout(pHLayout_2);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);//滑动区变化时Widget大小也可重新设置
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();

}

//析构函数
ChatView::~ChatView()
{

}

//头插
void ChatView::prependChatItem(QWidget *item)
{
    Q_UNUSED(item);
}

//中间插
void ChatView::insertChatItem(QWidget *before, QWidget *item)
{
    Q_UNUSED(before);
    Q_UNUSED(item);
}

//尾插
void ChatView::appendChatItem(QWidget *item)
{
    QVBoxLayout *vl = (qobject_cast<QVBoxLayout *>)(m_pScrollArea->widget()->layout());//将滑动区的布局复制
    vl->insertWidget(vl->count()-1, item);//为啥减一，因为new Widget在下面
    isAppended = true;
}

void ChatView::removeAllItem()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

    int count = layout->count();
    for (int i = 0; i < count - 1; ++i)//最下方一个widget为占位的，因此不需要删除最下方那一个
    {
        QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
        if (item) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    }
}

//初始化样式表
void ChatView::initStyleSheet()
{
   // QScrollBar *scrollBar = m_pScrollArea->verticalScrollBar();
   // scrollBar->setStyleSheet("QScrollBar{background:transparent;}"
   //                          "QScrollBar:vertical{background:transparent;width:8px;}"
   //                          "QScrollBar::handle:vertical{background:red; border-radius:4px;min-height:20px;}"
   //                          "QScrollBar::add-line:vertical{height:0px}"
   //                          "QScrollBar::sub-line:vertical{height:0px}"
   //                          "QScrollBar::add-page:vertical {background:transparent;}"
   //                          "QScrollBar::sub-page:vertical {background:transparent;}");
}

//事件过滤器
bool ChatView::eventFilter(QObject *obj, QEvent *event)
{
    /*if(e->type() == QEvent::Resize && o == )
    {

    }
    else */
    if(event->type() == QEvent::Enter && obj == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(event->type() == QEvent::Leave && obj == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(obj, event);
}

//绘制事件
void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    return QWidget::paintEvent(event);//???是否需要重绘
}

//滚动条移动的槽函数
void ChatView::onVScrollBarMoved(int min, int max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);

    if(isAppended)//添加Item可能要调用多次
    {
        QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();//将滚动区域的垂直滚动条存下来方便使用
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒内多次调用
        QTimer::singleShot(500,[this](){
            isAppended = false;
        });
    }
}

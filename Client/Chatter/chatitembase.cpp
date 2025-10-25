#include "chatitembase.h"

//有参构造
ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent)
    :QWidget(parent)
    ,m_role(role)
{
    //用户名Label
    m_pNameLabel    = new QLabel();
    m_pNameLabel->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");//设置字体
    font.setPointSize(9);//设置字号
    m_pNameLabel->setFont(font);
    m_pNameLabel->setFixedHeight(20);//设置高度固定为20
    //头像Label
    m_pIconLabel    = new QLabel();
    m_pIconLabel->setScaledContents(true);//允许伸缩
    m_pIconLabel->setFixedSize(42, 42);
    //气泡框
    m_pBubble       = new QWidget();

    //创建栅格布局
    QGridLayout *pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);//垂直控件间距为3
    pGLayout->setHorizontalSpacing(3);//水平控件间距为3
    pGLayout->setContentsMargins(3,3,3,3);//源码中为pGLayout->setMargin(3);
    //弹簧控件，最小40*20，可伸缩
    QSpacerItem *pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


    /*
    1.角色为自己的栅格布局：
        |--------------------------------------|
        |            |  NameLabel  | IconLabel |
        |--------------------------------------|
        |   spacer   |    Bubble   |           |
        |--------------------------------------|

    2.角色为他人的栅格布局
        |--------------------------------------|
        | IconLabel |  NameLabel  |            |
        |--------------------------------------|
        |           |    Bubble   |   spacer   |
        |--------------------------------------|
    */
    if(m_role == ChatRole::Self)//角色为自己
    {
        // NameLabel 坐标(0,1)
        m_pNameLabel->setContentsMargins(0, 0, 8, 0);//右边距为8
        m_pNameLabel->setAlignment(Qt::AlignRight);//右对齐
        pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);//0行1列，占1行1列
        // IconLabel 坐标(0,2)
        pGLayout->addWidget(m_pIconLabel, 0, 2, 2, 1, Qt::AlignTop);//0行2列，占2行1列，上对齐
        // spacer 坐标(1,0)
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);//1行0列，占1行1列
        // Bubble 坐标(1,1)
        pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);//1行1列，占1行1列

        //设置列的拉伸因子，第0列为2（0.4）,第1列为3（0.6）,第2列不变
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
    }
    else//角色为他人
    {
        m_pNameLabel->setContentsMargins(8, 0, 0, 0);//左边距为8
        m_pNameLabel->setAlignment(Qt::AlignLeft);//左对齐
        pGLayout->addWidget(m_pIconLabel, 0, 0, 2, 1, Qt::AlignTop);//上对齐
        pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
        pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
        pGLayout->addItem(pSpacer, 2, 2, 1, 1);

        //设置列的拉伸因子，第0列不变,第1列为3（0.6）,第2列为2（0.4）
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
    }
    this->setLayout(pGLayout);//设置布局
}

//析构
ChatItemBase::~ChatItemBase()
{

}

//设置用户名
void ChatItemBase::SetUserName(const QString &name)
{
    m_pNameLabel->setText(name);
}

//设置用户头像
void ChatItemBase::SetUserIcon(const QPixmap &icon)
{
    m_pIconLabel->setPixmap(icon);
}

//设置页面，气泡框
void ChatItemBase::SetWidget(QWidget *wid)
{
    QGridLayout *pGLayout = (qobject_cast<QGridLayout *>)(this->layout());
    pGLayout->replaceWidget(m_pBubble, wid);//将新的Bubble显示出来
    delete m_pBubble;//删除原来的Bubble  注意此处如果不删除可能会造成野指针进而导致内存泄漏
    m_pBubble = wid;//将新的Bubble赋值给此成员变量
}

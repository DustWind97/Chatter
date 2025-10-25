#include "textbubble.h"
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <QFontMetricsF>

//有参构造
TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent)
    :BubbleFrame(role, parent)
{
    //创建一个TextEdit
    m_pTextEdit = new QTextEdit();
    m_pTextEdit->setReadOnly(true);//设置只读
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//关闭垂直滚动策略
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//关闭水平滚动策略
    m_pTextEdit->installEventFilter(this);//在此界面部署一个事件过滤器
    //设置微软雅黑字体
    QFont font("MicroSoft YaHei");
    m_pTextEdit->setFont(font);
    //插入文本内容
    setPlainText(text);
    //设置Widget(TextEdit)
    setWidget(m_pTextEdit);
    //初始化样式
    initStyleSheet();
}

//析构
TextBubble::~TextBubble()
{

}

bool TextBubble::eventFilter(QObject *obj, QEvent *event)
{
    if(m_pTextEdit == obj && event->type() == QEvent::Paint)
    {
        adjustTextHeight();//在PaintEvent中设置
    }
    return BubbleFrame::eventFilter(obj, event);
}

//调整文本高度
void TextBubble::adjustTextHeight()
{
    qreal doc_margin = m_pTextEdit->document()->documentMargin();//字体到边框的宽度默认为
    QTextDocument *doc = m_pTextEdit->document();
    qreal text_height = 0;//用于存储统计文本高度,每段文本高度相加得到总的高度

    for(QTextBlock it = doc->begin(); it != doc->end(); it=it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();
        text_height += text_rect.height();
    }

    int vMargin = this->layout()->contentsMargins().top();

    //这个气泡的总高度=文本累计高度+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin * 2 + vMargin * 2);
}

//设置文本
void TextBubble::setPlainText(const QString &text)
{
    m_pTextEdit->setPlainText(text);

    //找出段落最大宽度
    qreal doc_margin = m_pTextEdit->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();

    QFontMetricsF fm(m_pTextEdit->font());
    QTextDocument *doc = m_pTextEdit->document();
    int max_width = 0;
    //遍历文本，查找到最大宽度
    for(QTextBlock it = doc->begin(); it != doc->end(); it=it.next())
    {
        int txtWidth = int(fm.horizontalAdvance(it.text()));//统计单行宽度
        max_width = max_width < txtWidth ? txtWidth : max_width;//对比此单行宽度和最大宽度并赋值给最大宽度
    }
    //设置此气泡的最大宽度（仅需要设置一次）
    setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right));
}

//设置初始化样式表
void TextBubble::initStyleSheet()
{
    m_pTextEdit->setStyleSheet("QTextEdit{background:transparent;border:none}");
}

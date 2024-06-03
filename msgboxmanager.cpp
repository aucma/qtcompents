#include "msgboxmanager.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEvent>
#include <QMoveEvent>
MsgBoxManager::MsgBoxManager(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    m_suitable_width = qMin(300, parentWidget()->width());
    setFixedWidth(m_suitable_width);

    parent->installEventFilter(this);
}

MsgBoxCard* MsgBoxManager::createMsgBoxCard(MsgBoxInfo* info)
{
    MsgBoxCard* card = new MsgBoxCard(info, this);
    connect(card, &MsgBoxCard::cardClose, this, &MsgBoxManager::slotCardClosed);

    addMsgBox(card);
    return card;
}

bool MsgBoxManager::eventFilter(QObject *target, QEvent *event)
{
    if (target == parentWidget())
    {
        switch (event->type())
        {
        case QEvent::Resize:
            break;
        case QEvent::Move:
        {
            QMoveEvent* moveEvent = static_cast<QMoveEvent*>(event);
            int diffx = moveEvent->pos().x() - moveEvent->oldPos().x();
            int diffy = moveEvent->pos().y() - moveEvent->oldPos().y();
            setGeometry(pos().x()+diffx, pos().y()+diffy, width(), height());
        }
        break;
        case QEvent::Close:
            close();
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(target, event);
}

void MsgBoxManager::slotCardClosed(MsgBoxCard *card)
{
    card->close();
    int index = m_listMsgBox.indexOf(card);

    QRect rcCard = card->geometry();

    for(int i = index+1; i<m_listMsgBox.size(); i++)
    {
        QRect rc = m_listMsgBox[i]->geometry();
        m_listMsgBox[i]->show();
        m_listMsgBox[i]->setGeometry(rc.left(), rc.top()-rcCard.height()-BOX_INTERVAL, rc.width(), rc.height());

        QPropertyAnimation* box_ani = new QPropertyAnimation(m_listMsgBox[i], "geometry");
        box_ani->setStartValue(rc);
        box_ani->setEndValue(QRect(rc.left(), rc.top()-rcCard.height()-BOX_INTERVAL, rc.width(), rc.height()));
        box_ani->setDuration(300);
        box_ani->start();
        connect(box_ani, SIGNAL(finished()), box_ani, SLOT(deleteLater()));
    }

    m_listMsgBox.removeAt(index);
    card->deleteLater();
}

void MsgBoxManager::addMsgBox(MsgBoxCard *card)
{
    QSize card_size= card->size();
    QRect rcParent = parentWidget()->geometry();

    int height = 0;
    for(int i = 0; i<m_listMsgBox.size(); i++)
    {
        height += m_listMsgBox[i]->height();
        height += BOX_INTERVAL;
    }

    m_listMsgBox.append(card);
    QRect rc(rcParent.left() +(rcParent.width()-width())/2, rcParent.top()+10, rcParent.width(), height+card->height()+BOX_INTERVAL);
    setGeometry(rc);

    card->show();
    card->setGeometry((this->width()-card_size.width())/2, height, card_size.width(), card_size.height());
    QRect rcCard = card->geometry();

    QPropertyAnimation* box_ani = new QPropertyAnimation(card, "geometry");
    box_ani->setStartValue(QRect(rcCard.left(), height-rcCard.height(), rcCard.width(), rcCard.height()));
    box_ani->setEndValue(rcCard);
    box_ani->setDuration(300);
    box_ani->start();
    connect(box_ani, SIGNAL(finished()), box_ani, SLOT(deleteLater()));
}

//////////////////////////////////////////////////////////
template<typename T>
int MsgBoxCard::getWidgetHeight(T *w)
{
    QStringList strs = w->text().split("\n");
    int w_width = w->width();
    int height = 0;
    QFontMetrics fm = w->fontMetrics();
    double mPixelPerCentimer = 1.0;
    int linesCount = 0;
    foreach (QString str, strs)
    {
        double tempWidth = fm.horizontalAdvance(str) / mPixelPerCentimer; //字数转行数
        linesCount += (tempWidth+w_width-1) / w_width;
    }
    height = fm.lineSpacing() * linesCount;
    return height;
}

MsgBoxCard::MsgBoxCard(MsgBoxInfo* info, QWidget *parent): QWidget(parent)
{
    m_msgBoxInfo = info;

    m_icon = new QLabel(this);
    m_icon->setFixedSize(24, 24);

    if(info->m_MsgType == MsgBoxInfo::error)
        m_icon->setStyleSheet("background-image:url(:/resource/error.png)");
    else if(info->m_MsgType == MsgBoxInfo::warn)
        m_icon->setStyleSheet("background-image:url(:/resource/warn.png)");
    else if(info->m_MsgType == MsgBoxInfo::success)
        m_icon->setStyleSheet("background-image:url(:/resource/success.png)");
    else
         m_icon->setStyleSheet("background-image:url(:/resource/info.png)");

    m_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_content_label  = new QLabel(info->m_Msg, this);
    m_content_label->setAlignment(Qt::AlignTop);
    m_content_label->setStyleSheet("font-family:\"微软雅黑\"; font-size:12px;");

    QHBoxLayout* hlayout = new QHBoxLayout(this);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);

    hlayout->addWidget(m_icon);
    hlayout->addSpacing(2);
    hlayout->addWidget(m_content_label);

    setLayout(hlayout);

    m_content_label->adjustSize();

    QFontMetricsF fontMetrics(m_content_label->font());
    QRectF rcText = fontMetrics.boundingRect(QRect(0, 0, parentWidget()->width() - BOX_CONTENT_MARGIN*2 - m_icon->width(), 999999), Qt::AlignTop | Qt::TextWordWrap, m_content_label->text());

    m_content_label->setFixedWidth(rcText.width());
    m_content_label->setFixedHeight(getWidgetHeight(m_content_label));
    m_content_label->setWordWrap(true);

    // 控件大小
    resize(m_content_label->width()+m_icon->width()+2+20, m_content_label->height()+20);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(1000*5);
    m_timer->start();
    connect(m_timer, &QTimer::timeout, this, [=](){emit cardClose(this);});
}

void MsgBoxCard::paintEvent(QPaintEvent *event)
{
    //白色背景和圆角弧度
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 4, 4);

    QWidget::paintEvent(event);
}

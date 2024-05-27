#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_msgboxmanager = new MsgBoxManager(this);
    m_msgboxmanager->show();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    static int i = 0;

    MsgBoxInfo* info = new MsgBoxInfo(QString("码错误用户名密码错误用户名码错误用户误 %1").arg(i++), (MsgBoxInfo::MsgType)(i % 3), this);
    m_msgboxmanager->createMsgBoxCard(info);
}


void Widget::on_pushButton_2_clicked()
{
    static int i = 0;

    MsgBoxInfo* info = new MsgBoxInfo(QString("码错误码错误用户名密码错误用户名码错误用户码错误码错误用户名密码错误用户名码错误用户误用户名密码错误用户名码错误用户误误用户名密码错误用户名码错误用户误 %1").arg(i++), (MsgBoxInfo::MsgType)(i % 3), this);
    m_msgboxmanager->createMsgBoxCard(info);
}


void Widget::on_pushButton_3_clicked()
{
    static int i = 0;

    MsgBoxInfo* info = new MsgBoxInfo(QString("%1").arg(i++), (MsgBoxInfo::MsgType)(i % 3), this);
    m_msgboxmanager->createMsgBoxCard(info);
}


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
    m_msgboxmanager->showInfo("提示信息");
}


void Widget::on_pushButton_2_clicked()
{
    m_msgboxmanager->showError("错误信息");
}


void Widget::on_pushButton_3_clicked()
{
    m_msgboxmanager->showSuccess("成功信息");
}


void Widget::on_pushButton_4_clicked()
{
    m_msgboxmanager->showWarn("警告信息");
}


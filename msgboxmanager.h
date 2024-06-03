#ifndef MSGBOXMANAGER_H
#define MSGBOXMANAGER_H

#include <QWidget>
#include <QLabel>
#include <QList>
class MsgBoxInfo : public QObject
{
    Q_OBJECT
public:
    enum MsgType {
        error,
        warn,
        info,
        success
    };

    MsgBoxInfo(QString content, MsgType type, QObject* parent= nullptr): QObject(parent)
    {
        m_Msg = content;
        m_MsgType = type;
    }

    MsgType m_MsgType = MsgType::info;
    QString m_Msg;
};

#define BOX_CONTENT_MARGIN 10
class MsgBoxCard : public QWidget
{
    Q_OBJECT
public:
    explicit MsgBoxCard(MsgBoxInfo* info, QWidget *parent = nullptr);
    ~MsgBoxCard()
    {
        delete m_msgBoxInfo;
    }

    void paintEvent(QPaintEvent *event) override;

private:
    template<typename T>
    int getWidgetHeight(T* w);

signals:
    void cardClose(MsgBoxCard*);

private:
    MsgBoxInfo* m_msgBoxInfo;
    QLabel* m_icon;
    QLabel* m_content_label;
    QTimer* m_timer;
};

#define BOX_WIDTH
#define BOX_INTERVAL 10
class MsgBoxManager : public QWidget
{
    Q_OBJECT
public:
    explicit MsgBoxManager(QWidget *parent = nullptr);
    ~MsgBoxManager()
    {

    }
    MsgBoxCard* createMsgBoxCard(MsgBoxInfo* info);

    void showError(QString content)
    {
        MsgBoxInfo* info = new MsgBoxInfo(content, MsgBoxInfo::error);
        createMsgBoxCard(info);
    }

    void showInfo(QString content)
    {
        MsgBoxInfo* info = new MsgBoxInfo(content, MsgBoxInfo::info);
        createMsgBoxCard(info);
    }

    void showWarn(QString content)
    {
        MsgBoxInfo* info = new MsgBoxInfo(content, MsgBoxInfo::warn);
        createMsgBoxCard(info);
    }

    void showSuccess(QString content)
    {
        MsgBoxInfo* info = new MsgBoxInfo(content, MsgBoxInfo::success);
        createMsgBoxCard(info);
    }

    bool eventFilter(QObject *target, QEvent *event) override;
signals:

public slots:
    void slotCardClosed(MsgBoxCard*);

private:
    void addMsgBox(MsgBoxCard *msgBox);

private:
    QList<MsgBoxCard*> m_listMsgBox;
    int m_suitable_width = 0;
};

#endif // MSGBOXMANAGER_H

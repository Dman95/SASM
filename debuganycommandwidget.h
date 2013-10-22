#ifndef DEBUGANYCOMMANDWIDGET_H
#define DEBUGANYCOMMANDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QKeyEvent>

class DebugAnyCommandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DebugAnyCommandWidget(QWidget *parent = 0);
    ~DebugAnyCommandWidget();
    void setFocusOnLineEdit();
    void showPreviousCommand();
    void showNextCommand();
    
signals:
    void performCommand(const QString command);
    
public slots:
    void processCommand();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QLabel *anyCommandLabel;
    QLineEdit *command;
    QPushButton *performButton;
    QHBoxLayout *layout;
    QStringList commands;
    int currentCommandPos;
};

#endif // DEBUGANYCOMMANDWIDGET_H

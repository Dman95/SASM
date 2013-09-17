#ifndef REGISTERSWINDOW_H
#define REGISTERSWINDOW_H

#include <QTableWidget>

class RegistersWindow : public QTableWidget
{
    Q_OBJECT
public:
    explicit RegistersWindow(int rows, int columns, QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *);

signals:
    void closeSignal();
    
public slots:
    
};

#endif // REGISTERSWINDOW_H

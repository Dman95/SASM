#include "registerswindow.h"

RegistersWindow::RegistersWindow(int rows, int columns, QWidget *parent) :
    QTableWidget(rows, columns, parent)
{
}

void RegistersWindow::closeEvent(QCloseEvent *) {
    emit closeSignal();
}

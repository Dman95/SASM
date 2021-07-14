/****************************************************************************
** SASM - simple IDE for assembler development
** Copyright (C) 2013 Dmitriy Manushin
** Contact: site: http://dman95.github.io/SASM/
**          e-mail: Dman1095@gmail.com
**
** This file is part of SASM.
**
** SASM is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** SASM is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with SASM.  If not, see <http://www.gnu.org/licenses/>.
**
** Этот файл — часть SASM.
**
** SASM - свободная программа: вы можете перераспространять ее и/или
** изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
** в каком она была опубликована Фондом свободного программного обеспечения;
** либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
** версии.
**
** SASM распространяется в надежде, что она будет полезной,
** но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
** или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
** общественной лицензии GNU.
**
** Вы должны были получить копию Стандартной общественной лицензии GNU
** вместе с этой программой. Если это не так, см.
** <http://www.gnu.org/licenses/>.)
**
****************************************************************************/

#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <QWidget>
#include <QtGui>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QComboBox>
#include <QtWidgets/QScrollArea>
#include <QElapsedTimer>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#ifdef Q_OS_WIN32
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/sem.h>
#endif
#define BLOCK_SIZE 3145728

class DisplayWindow : public QWidget
{
    Q_OBJECT
public:
    
    explicit DisplayWindow(QWidget *parent = 0);
    ~DisplayWindow();
    void changeDisplay(int msgid);
    void finish(int msgid);
    void updateDisplay();
    #ifdef Q_OS_WIN32
    HANDLE hCreateNamedPipe;
    #else
    sem_t* sem_producer;
    sem_t* sem_consumer;
    int sem_con_id, sem_pro_id;
    #endif

protected:
    void closeEvent(QCloseEvent *);

private:
    QVBoxLayout  *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QImage* displayPicture;
    QLabel* displayImageLabel;
    QComboBox *zoomComboBox;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    uint8_t buffer[BLOCK_SIZE];
    int zoom;
    int msgid, res_x, res_y, mode, display_size;
    std::atomic<bool> loop;
    int shared_block_id;
    uint8_t* block_values;
    
public slots:
void zoomSettingsChanged(int value);

signals:
    void displayChanged(void);
    void closeSignal();
    void closeDisplay();
    void printLog(QString msg, QColor color = QColor(Qt::black));
};

#endif

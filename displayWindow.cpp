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

#include "displayWindow.h"

#define IPC_RESULT_ERROR (-1)
#define BLOCK_SIZE 1048576
#define FILENAME "/tmp"


DisplayWindow::DisplayWindow(QWidget *parent) :
    QWidget(parent)
{
    zoom = 1;
    this->setFixedSize(QSize(512+50, 512+125));
    this->setStyleSheet("background-color:grey;");
    verticalLayout = new QVBoxLayout(this);
    zoomComboBox = new QComboBox(this);
    QStringList comboBoxList;
    comboBoxList << "1" << "2" << "4" << "8" << "16" << "32";
    zoomComboBox->insertItems(0, comboBoxList);
    connect(zoomComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomSettingsChanged(int)));

    verticalLayout->addWidget(zoomComboBox);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    //scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1218, 1218));
    horizontalLayout = new QHBoxLayout(scrollAreaWidgetContents);
    displayImageLabel = new QLabel(scrollAreaWidgetContents);
    displayImageLabel->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 85, 127);"));
    displayImageLabel->setMinimumSize(QSize(150, 150));

    horizontalLayout->addWidget(displayImageLabel);

    scrollArea->setWidget(scrollAreaWidgetContents);

    verticalLayout->addWidget(scrollArea);
}

void DisplayWindow::changeDisplay(int msgid){
    loop = true;
    displayPicture  = new QImage(512, 512, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    buffer.resize(512*512);
    memset(buffer.data(), 0xff, 512*512);
    scrollAreaWidgetContents->setFixedSize(512*zoom+26, 512*zoom+26);
    this->msgid = msgid;
    res_x = 512;
    res_y = 512;
    mode = 0;
    qint64 fps = 30;
    QElapsedTimer programExecutionTime;
    this->setFixedSize(QSize(512+60, 512+92));
    displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(512*zoom,512*zoom)));
    programExecutionTime.start();
    #ifdef Q_OS_WIN32
    if(!ConnectNamedPipe(hCreateNamedPipe, NULL))
        emit printLog(QString("Connection Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
    while(1){
        DWORD dwNoBytesRead;
        BOOL readSuccess = ReadFile(
                hCreateNamedPipe,
                message.mesg_text,
                8184,
                &dwNoBytesRead,
                NULL);
        if(!readSuccess){
                if(GetLastError()!=109)
                    emit printLog(QString("Read Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
            break;
        }
        if(message.mesg_text[0]==3){
            res_x = message.mesg_text[1];
            res_y = message.mesg_text[5];
            for(int i = 1; i < 4; i++){
                res_x += message.mesg_text[1+i] << (8*i);
                res_y += message.mesg_text[5+i]  << (8*i);
            }
            mode = message.mesg_text[9];
            fps = message.mesg_text[10];
            if(mode)
                buffer.resize(res_x*res_y*3);
            else
                buffer.resize(res_x*res_y);
            displayPicture  = new QImage(res_x, res_y, QImage::Format_RGB32);
            displayPicture->fill(qRgb(255, 255, 255));
            scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
            this->setFixedSize(QSize(res_x+60, res_y+92));
            displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
            continue;
        }
        // display the message and print on display
        int needed_bytes = (mode) ? res_x*res_y*3 : res_x*res_y;
        for(int i = 0; i < needed_bytes; i+=8184){
            dwNoBytesRead = 0;
            BOOL readSuccess = ReadFile(
            hCreateNamedPipe,
                message.mesg_text,
                8184,
                &dwNoBytesRead,
                NULL);
            if(!readSuccess){
                emit printLog(QString("Read Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
                break;
            }
            memcpy(buffer.data()+i, &message.mesg_text[0], std::min(8184, needed_bytes-i));
        }
        updateDisplay();
        qint64 elapsed_time = programExecutionTime.elapsed();
        if(elapsed_time < 1000/fps)
            usleep(1000/fps - elapsed_time);
        displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
        programExecutionTime.start();
    }
    CloseHandle(hCreateNamedPipe);
    #else
    sem_wait(sem_producer);
    if(loop){
        //setup the shared memory
        key_t key = ftok(FILENAME, 'f');
        if(key == IPC_RESULT_ERROR){}
        shared_block_id = shmget(key, BLOCK_SIZE, 0666 | IPC_CREAT);
        if(shared_block_id == IPC_RESULT_ERROR){
            emit printLog(QString("shmget failed\n"), Qt::red);
        }
        block_values = (uint8_t*)shmat(shared_block_id, NULL, 0);
        if(block_values == (uint8_t*)IPC_RESULT_ERROR){
            emit printLog(QString("shmat failed\n"), Qt::red);
        }
        /// read setup
        res_x = block_values[0];
        res_y = block_values[4];
        for(int i = 1; i < 4; i++){
            res_x += block_values[i] << (8*i);
            res_y += block_values[4+i]  << (8*i);
        }
        mode = block_values[8];
        fps = block_values[9];
        display_size = (mode) ? res_x*res_y*3 : res_x*res_y;
        displayPicture  = new QImage(res_x, res_y, QImage::Format_RGB32);
        displayPicture->fill(qRgb(255, 255, 255));
        scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
        this->setFixedSize(QSize(res_x+60, res_y+92));
        displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
        sem_post(sem_consumer);
    }
    ///
    
    while(loop){
        // receive message
        sem_wait(sem_producer);
        if(!loop)
            break;
        // display the message and print on display
        int currentcharx, currentchary;
        if (mode){
            for(int l = 0; l < display_size; l+=3) {
                currentcharx = (l/3)%res_x;
                currentchary = l/3/res_x;
                displayPicture->setPixel(currentcharx, currentchary, qRgb(block_values[l], block_values[l+1], block_values[l+2]));
            }
        } else {
            for(int l = 0; l < display_size; l++){
                currentcharx = l%res_x;
                currentchary = l/res_x;
                if(block_values[l]){
                    displayPicture->setPixel(currentcharx, currentchary, qRgb(255, 255, 255));
                } else {
                    displayPicture->setPixel(currentcharx, currentchary, qRgb(0, 0, 0));
                }
            }
        }
        
        
        qint64 elapsed_time = programExecutionTime.elapsed();
        //emit printLog("time: "+QString::number(elapsed_time)+"\n");
        if(elapsed_time < 1000/fps)
            usleep(1000/fps - elapsed_time);
        displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
        programExecutionTime.start();
        sem_post(sem_consumer);
    }
    //close sema
    sem_close(sem_consumer);
    sem_close(sem_producer);
    if(shmctl(shared_block_id, IPC_RMID, NULL) == IPC_RESULT_ERROR){
        emit printLog(QString("shmctl failed\n"), Qt::red);
    }
    #endif
    emit closeDisplay();
}

void DisplayWindow::finish(int msgid){
    this->msgid = msgid;
    #ifdef Q_OS_WIN32
    #else
    loop = false;
    sem_post(sem_producer);
    #endif
}

void DisplayWindow::zoomSettingsChanged(int value){
    #ifdef Q_OS_WIN32
    zoom = std::pow(2, value);
    scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
    this->setFixedSize(QSize(res_x+60, res_y+92));
    displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom, res_y*zoom)));
    #else
    sem_wait(sem_producer);
    zoom = std::pow(2, value);
    scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
    this->setFixedSize(QSize(res_x+60, res_y+92));
    displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom, res_y*zoom)));
    sem_post(sem_producer);
    #endif
}

void DisplayWindow::updateDisplay() {
    // display the message and print on display
    int currentcharx;
    int currentchary;
    int needed_pixel = (mode) ? res_x*res_y*3 : res_x*res_y;
    if (mode){
       for(int l = 0; l < needed_pixel; l+=3) {
            currentcharx = (l/3)%res_x;
            currentchary = l/3/res_x;
            displayPicture->setPixel(currentcharx, currentchary, qRgb(buffer[l], buffer[l+1], buffer[l+2]));
       }
    } else {
        for(int l = 0; l < needed_pixel; l++){
            currentcharx = l%res_x;
            currentchary = l/res_x;
            if(buffer[l]){
                displayPicture->setPixel(currentcharx, currentchary, qRgb(255, 255, 255));
            } else {
                displayPicture->setPixel(currentcharx, currentchary, qRgb(0, 0, 0));
            }
        }
    }
}

void DisplayWindow::closeEvent(QCloseEvent *) {
    emit closeSignal();
}

DisplayWindow::~DisplayWindow()
{
    delete displayImageLabel;
    //delete layout;
}

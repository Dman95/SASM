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


DisplayWindow::DisplayWindow(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("background-color:grey;");
    layout = new QVBoxLayout(this);
    zoom = 1;
    zoomComboBox = new QComboBox;
    layout->addWidget(zoomComboBox);
    QStringList comboBoxList;
    comboBoxList << "1" << "2" << "4" << "8" << "16" << "32";
    zoomComboBox->insertItems(0, comboBoxList);
    connect(zoomComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomSettingsChanged(int)));
    displayImageLabel = new QLabel("");
    displayPicture  = new QImage(537, 562, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));

    layout->addWidget(displayImageLabel, Qt::AlignTop);

    setLayout(layout); 
    this->resize(displayPicture->size());
    this->setFixedSize(500,525);
}

void DisplayWindow::changeDisplay(int msgid){
    this->setFixedSize(537,562);
    displayPicture  = new QImage(512, 512, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    buffer.resize(512*512);
    memset(buffer.data(), 0xff, 512*512);
    #ifdef Q_OS_WIN32
    #else
    this->msgid = msgid;
    res_x = 512;
    res_y = 512;
    mode = 0;
    qint64 fps = 30;
    QElapsedTimer programExecutionTime;
    memset(&message.mesg_text[0], 0xff, sizeof(message.mesg_text));
    displayPicture->fill(qRgb(0, 0, 0));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    programExecutionTime.start();
    while(1){
    	// msgrcv to receive message
    	msgrcv(msgid, &message, sizeof(message), 0, 0);

    	if (message.mesg_type == 2){  // type = 1 (default) -> normal message | -> 2 finish | -> 3 setup
    	    break;
    	}
    	if (message.mesg_type == 3){
    	    res_x = message.mesg_text[0];
     	    res_y = message.mesg_text[4];
    	    for(int i = 1; i < 4; i++){
    	        res_x += message.mesg_text[i] << (8*i);
     	        res_y += message.mesg_text[4+i]  << (8*i);
     	    }
     	    mode = message.mesg_text[8];
     	    fps = message.mesg_text[9];
     	    if(mode)
     	        buffer.resize(res_x*res_y*3);
     	    else
     	        buffer.resize(res_x*res_y);
     	    displayPicture  = new QImage(res_x*zoom, res_y*zoom, QImage::Format_RGB32);
    	    displayPicture->fill(qRgb(0, 0, 0));
    	    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    	    this->setFixedSize(displayPicture->size()+QSize(25,50));
    	    continue;
    	}
    	// display the message and print on display
    	int needed_bytes = (mode) ? res_x*res_y*3 : res_x*res_y;
    	for(int i = 0; i < needed_bytes; i+=8184){
    	    if(i>0)
    	        msgrcv(msgid, &message, sizeof(message), 0, 0);
    	    memcpy(buffer.data()+i, &message.mesg_text[0], std::min(8184, needed_bytes-i));
    	}
    	updateDisplay();
   	qint64 elapsed_time = programExecutionTime.elapsed();
    	if(elapsed_time < 1000/fps)
    	    usleep(1000/fps - elapsed_time);
        displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
        programExecutionTime.start();
    }
    msgctl(msgid, IPC_RMID, NULL);
    emit closeDisplay();
    #endif
}

void DisplayWindow::finish(int msgid){
    #ifdef Q_OS_WIN32
    #else
    mesg_buffer end;
    end.mesg_type = 2;
    //send default message type == 2 means end
    msgsnd(msgid, &end, sizeof(end), 0);
    #endif
}

void DisplayWindow::zoomSettingsChanged(int value){
    zoom = std::pow(2, value);
    displayPicture  = new QImage(res_x*zoom, res_y*zoom, QImage::Format_RGB32);
    this->setFixedSize(displayPicture->size()+QSize(25,50));
    displayPicture->fill(qRgb(0, 0, 0));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    updateDisplay();
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
}

void DisplayWindow::updateDisplay() {
    // display the message and print on display
    int currentcharx;
    int currentchary;
    int needed_pixel = (mode) ? res_x*res_y*3 : res_x*res_y;
    if (mode){
       for(int l = 0; l < needed_pixel; l+=3) {
            currentcharx = ((l/3)%res_x)*zoom;
            currentchary = (l/3/res_x)*zoom;
            for(int j = 0; j < zoom; j++){
             for(int k = 0; k < zoom; k++){
                displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(buffer[l], buffer[l+1], buffer[l+2]));
            }}
       }
    } else {
        for(int l = 0; l < needed_pixel; l++){
            currentcharx = (l%res_x)*zoom;
            currentchary = (l/res_x)*zoom;
            if(buffer[l]) {
               for(int j = 0; j < zoom; j++){
                for(int k = 0; k < zoom; k++){
                    displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(255, 255, 255));
                }}
            } else {
                for(int j = 0; j < zoom; j++){
                 for(int k = 0; k < zoom; k++){
                    displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(0, 0, 0));
                }}
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

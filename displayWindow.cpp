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
    displayImageLabel = new QLabel("");
    displayPicture  = new QImage(480, 480, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));

    layout->addWidget(displayImageLabel, Qt::AlignTop);

    setLayout(layout); 
    this->resize(displayPicture->size());
    this->setFixedSize(500,525);
}

void DisplayWindow::changeDisplay(int msgid, int msgidsnd){
    this->setFixedSize(500,525);
    displayPicture  = new QImage(480, 480, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    #ifdef Q_OS_WIN32
    #else
    char res_x = 8;
    char res_y = 8;
    char mode = 0;
    memset(&message.mesg_text[0], 0xff, sizeof(message.mesg_text));
    displayPicture->fill(qRgb(0, 0, 0));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    while(1){
    	// msgrcv to receive message
    	msgrcv(msgid, &message, sizeof(message), 0, 0);

    	if (message.mesg_type == 2){  // type = 1 (default) -> normal message | -> 2 finish | -> 3 setup
    	    break;
    	}
    	if (message.mesg_type == 3){
    	    res_x = message.mesg_text[0];
     	    res_y = message.mesg_text[1];
     	    mode = message.mesg_text[2];
     	    displayPicture  = new QImage(res_x*60, res_y*60, QImage::Format_RGB32);
    	    displayPicture->fill(qRgb(0, 0, 0));
    	    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    	    this->setFixedSize(displayPicture->size());
    	    continue;
    	}
    	
    	// display the message and print on display
    	int needed_pixel = res_x*res_y;
    	int currentcharx;
    	int currentchary;
    	if(mode) {
    	    needed_pixel *= 3;
    	}
    	for(int l = 0; l < needed_pixel; l+=768) {
    	    if(l>0)
    	        msgrcv(msgid, &message, sizeof(message), 0, 0);
            for(int i = 0; i < std::min(768, needed_pixel-l); i++){
                if(mode){
                    currentcharx = (((i+l)/3)%res_x)*60;
                    currentchary = ((i+l)/3/res_x)*60;
                } else {
                    currentcharx = ((i+l)%res_x)*60;
                    currentchary = ((i+l)/res_x)*60;
                }
                // pixel makieren
                for(int k = 0; k < 60; k++){
                    displayPicture->setPixel(currentcharx+k, currentchary, qRgb(255, 0, 0));
                    displayPicture->setPixel(currentcharx+k, currentchary+1, qRgb(255, 0, 0));
                }
                for(int k = 2; k < 57; k++){
                    displayPicture->setPixel(currentcharx, currentchary+k, qRgb(255, 0, 0));
                    displayPicture->setPixel(currentcharx+1, currentchary+k, qRgb(255, 0, 0));
                    displayPicture->setPixel(currentcharx+58, currentchary+k, qRgb(255, 0, 0));
                    displayPicture->setPixel(currentcharx+59, currentchary+k, qRgb(255, 0, 0));
                }
                for(int k = 0; k < 60; k++) {
                    displayPicture->setPixel(currentcharx+k, currentchary+58, qRgb(255, 0, 0));
                    displayPicture->setPixel(currentcharx+k, currentchary+59, qRgb(255, 0, 0));
                }
                displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
                usleep(90000);
                
                if (mode){
                    for(int j = 0; j < 60; j++){
                    for(int k = 0; k < 60; k++){
                        displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(message.mesg_text[i], message.mesg_text[i+1], message.mesg_text[i+2]));
                    }}
                    i+=2;
                } else {
                    if(message.mesg_text[i]) {
                        for(int j = 0; j < 60; j++){
                        for(int k = 0; k < 60; k++){
                            displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(255, 255, 255));
                        }}
                    } else {
                        for(int j = 0; j < 60; j++){
                        for(int k = 0; k < 60; k++){
                            displayPicture->setPixel(currentcharx+j, currentchary+k, qRgb(0, 0, 0));
                        }}
                    }
                }
            }
        }
        displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
        msgsnd(msgidsnd, &message, sizeof(message), 0);
    }
    msgctl(msgid, IPC_RMID, NULL);
    msgctl(msgidsnd, IPC_RMID, NULL);
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

void DisplayWindow::closeEvent(QCloseEvent *) {
    emit closeSignal();
}

DisplayWindow::~DisplayWindow()
{
    delete displayImageLabel;
    //delete layout;
}

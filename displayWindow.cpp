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

    // white = new QRgb(255, 255, 255);
    // black = new QRgb(0, 0, 0);

    displayImageLabel = new QLabel("");
    displayPicture  = new QImage(480, 480, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));

    layout->addWidget(displayImageLabel, Qt::AlignTop);

    setLayout(layout); 
    this->resize(displayPicture->size());
}

void DisplayWindow::changeDisplay(int msgid){
    while(1){
    	// msgrcv to receive message
    	msgrcv(msgid, &message, sizeof(message), 0, 0);

    	if (message.mesg_type == 2){
    	    break;
    	}
    	// display the message and print on display
        //std::cout << "Data Received in :";
        displayPicture->fill(qRgb(255, 255, 255));
        for(int i = 0; i < 256; i++){
            //std::cout << + message.mesg_text[i] << " ";
            if(message.mesg_text[i]) {
                for(int j = 0; j < 60; j++){
                for(int k = 0; k < 60; k++){
                    displayPicture->setPixel((i%8)*60+j, i/8*60+k, qRgb(0, 0, 0));
                }}
                //displayPicture->setPixel((i%32)*15, i/32*15, qRgb(0, 0, 0));
            }
        }
        //std::cout << "\n";
        displayImageLabel->setPixmap(QPixmap::fromImage(*displayPicture));
    }
    msgctl(msgid, IPC_RMID, NULL);
    emit closeDisplay();
}

void DisplayWindow::finish(int msgid){
    mesg_buffer end;
    end.mesg_type = 2;
    strcpy(end.mesg_text, "end");
    //send default message type == 2 means end
    if(msgsnd(msgid, &end, sizeof(end), 0) < 0)
        printf("error finish... ");
}

void DisplayWindow::closeEvent(QCloseEvent *) {
    emit closeSignal();
}

DisplayWindow::~DisplayWindow()
{
    delete displayImageLabel;
    delete layout;
}

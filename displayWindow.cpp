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

    layout->addWidget(displayImageLabel, Qt::AlignTop);

    setLayout(layout); 
    this->setFixedSize(500,525);
}

void DisplayWindow::changeDisplay(int msgid, HANDLE hCreateNamedPipe){
    displayPicture  = new QImage(512, 512, QImage::Format_RGB32);
    buffer.resize(512*512);
    memset(buffer.data(), 0xff, 512*512);
    this->msgid = msgid;
    res_x = 512;
    res_y = 512;
    mode = 0;
    qint64 fps = 30;
    QElapsedTimer programExecutionTime;
    displayPicture->fill(qRgb(255, 255, 255));
	this->setFixedSize(QSize(512*zoom+25, 512*zoom+75));
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
			this->setFixedSize(QSize(res_x*zoom+25, res_y*zoom+75));
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
     	    displayPicture  = new QImage(res_x, res_y, QImage::Format_RGB32);
    	    displayPicture->fill(qRgb(0, 0, 0));
    	    displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
    	    this->setFixedSize(QSize(res_x*zoom+25, res_x*zoom+75));
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
        displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom,res_y*zoom)));
        programExecutionTime.start();
    }
    msgctl(msgid, IPC_RMID, NULL);
	#endif
    emit closeDisplay();
}

void DisplayWindow::finish(int msgid){
	this->msgid = msgid;
    #ifdef Q_OS_WIN32
	/*char c[8184];
	c[0] = 2;
	HANDLE hFile = CreateFileW(
            L"\\\\.\\pipe\\SASMPIPE",
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		emit printLog(QString("Could not create file object (")+QString::number(GetLastError())+")\n", Qt::red);
		return;
	}
	DWORD dwNoBytesWrote = 0;
	BOOL writeSuccess = WriteFile(
			hFile,
			c,
			sizeof(c),
			&dwNoBytesWrote,
			NULL);
	if(!writeSuccess){
		emit printLog(QString("Could not write to file (")+QString::number(GetLastError())+")\n", Qt::red);
	}
	if(!FlushFileBuffers(hFile)){
		emit printLog(QString("Could not flush the file (")+QString::number(GetLastError())+")\n", Qt::red);
	}*/
    #else
    mesg_buffer end;
    end.mesg_type = 2;
    //send default message type == 2 means end
    msgsnd(msgid, &end, sizeof(end), 0);
    #endif
}

void DisplayWindow::zoomSettingsChanged(int value){
    zoom = std::pow(2, value);
    updateDisplay();
	this->setFixedSize(QSize(res_x*zoom+25, res_y*zoom+75));
    displayImageLabel->setPixmap(QPixmap::fromImage(displayPicture->scaled(res_x*zoom, res_y*zoom)));
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
            if(buffer[l]) {
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

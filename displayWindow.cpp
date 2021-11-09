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
    scrollAreaWidgetContents = new BufferFrame(this);
    //scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1218, 1218));

    scrollArea->setWidget(scrollAreaWidgetContents);

    verticalLayout->addWidget(scrollArea);
    
    #ifdef Q_OS_WIN32
    #else
    /* create producer semaphore | set to 0: */
    if ((sem_pro_id = semget(ftok(FILENAME, 'p'), 1, 0666 | IPC_CREAT)) == -1){
        emit printLog(QString("sem_prod failed (semget)\n"), Qt::red);
    }
    arg.val = 0;
    if (semctl(sem_pro_id, 0, SETVAL, arg) == -1){
        emit printLog(QString("sem_prod failed (semctl)\n"), Qt::red);
    }
    /* create consumer semaphore | set to 1*/
    if ((sem_con_id = semget(ftok(FILENAME, 'c'), 1, 0666 | IPC_CREAT)) == -1){
        emit printLog(QString("sem_con failed (semget)\n"), Qt::red);
    }
    arg.val = 1;
    if (semctl(sem_con_id, 0, SETVAL, arg) == -1){
        emit printLog(QString("sem_con failed (semctl)\n"), Qt::red);
    }
    #endif
}

void DisplayWindow::changeDisplay(int msgid){
    loop = true;
    displayPicture  = new QImage(512, 512, QImage::Format_RGB32);
    displayPicture->fill(qRgb(255, 255, 255));
    memset(buffer, 0xff, 512*512);
    scrollAreaWidgetContents->setFixedSize(512*zoom+26, 512*zoom+26);
    this->msgid = msgid;
    res_x = 512;
    res_y = 512;
    mode = 0;
    qint64 fps = 30;
    QElapsedTimer programExecutionTime;
    QElapsedTimer programExecutionTime2;
    this->setFixedSize(QSize(512+60, 512+92));
    scrollAreaWidgetContents->update();
    programExecutionTime.start();
    programExecutionTime2.start();
    #ifdef Q_OS_WIN32
    if(!ConnectNamedPipe(hCreateNamedPipe, NULL)){
        if(GetLastError()!=535)
            emit printLog(QString("Connection Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
    }

    DWORD dwNoBytesRead;
    BOOL readSuccess = ReadFile(
            hCreateNamedPipe,
            buffer,
            BLOCK_SIZE,
            &dwNoBytesRead,
            NULL);
    if(readSuccess && loop){
        res_x = buffer[0];
        res_y = buffer[4];
        for(int i = 1; i < 4; i++){
            res_x += buffer[i] << (8*i);
            res_y += buffer[4+i]  << (8*i);
        }
        mode = buffer[8];
        fps = buffer[9];
        display_size = (mode) ? res_x*res_y*3 : res_x*res_y;
        displayPicture  = new QImage(res_x, res_y, QImage::Format_RGB32);
        displayPicture->fill(qRgb(255, 255, 255));
        this->setFixedSize(QSize(res_x+60, res_y+92));
		scrollAreaWidgetContents->setFixedSize(res_x*zoom, res_y*zoom);
		scrollAreaWidgetContents->update();
	} else {
	    if(GetLastError()!=109&&GetLastError()!=0)
            emit printLog(QString("Read Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
		loop = false;
	}

    while(loop){
        DWORD dwNoBytesRead;
        BOOL readSuccess = ReadFile(
                hCreateNamedPipe,
                buffer,
                BLOCK_SIZE,
                &dwNoBytesRead,
                NULL);
		if(!loop)
			break;
        if(!readSuccess){
                if(GetLastError()!=109)
                    emit printLog(QString("Read Failed with Error (")+QString::number(GetLastError())+")\n", Qt::red);
            break;
        }
        // display the message and print on display
        int currentcharx;
        int currentchary;
        int needed_pixel = (mode) ? res_x*res_y*3 : res_x*res_y;
        if(mode){
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
        qint64 elapsed_time = programExecutionTime.elapsed();
        if(elapsed_time < 1000/fps)
            usleep((1000/fps - elapsed_time)*1000);
        programExecutionTime.start();
	scrollAreaWidgetContents->update();	
    }
    CloseHandle(hCreateNamedPipe);
    #else
    // wait sem_wait(sem_producer);
    struct sembuf sb = {0, -1, 0};
    if (semop(sem_pro_id, &sb, 1) == -1){
        emit printLog(QString("sem_pro failed (semop)"+QString(strerror(errno))+"\n"), Qt::red);
    }
    
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
        if(fps < 1 || fps > 60){
            emit printLog(QString("fps has wrong settings\n"), Qt::red);
            fps = 1;
        }
        display_size = (mode) ? res_x*res_y*3 : res_x*res_y;
        displayPicture  = new QImage(res_x, res_y, QImage::Format_RGB32);
        displayPicture->fill(qRgb(255, 255, 255));
        scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
        this->setFixedSize(QSize(res_x+60, res_y+92));
        scrollAreaWidgetContents->update();
        // sem_post(sem_consumer);
        sb.sem_op = 1;
        if (semop(sem_con_id, &sb, 1) == -1) {
             emit printLog(QString("sem_con failed (semop)\n"), Qt::red);
        }
    }
    ///
    
    while(loop){
        // receive message
        // sem_wait(sem_producer);
        sb.sem_op = -1;
        if (semop(sem_pro_id, &sb, 1) == -1) {
             emit printLog(QString("sem_pro failed (semop)\n"), Qt::red);
        }
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
            usleep((1000/fps - elapsed_time)*1000);
        programExecutionTime.start();
        scrollAreaWidgetContents->update();
        //sem_post(sem_consumer);
        sb.sem_op = 1;
        if (semop(sem_con_id, &sb, 1) == -1) {
             emit printLog(QString("sem_con failed (semop)\n"), Qt::red);
        }
    }
    //sem_close(sem_consumer);
    //sem_close(sem_producer);
    if(shmctl(shared_block_id, IPC_RMID, NULL) == IPC_RESULT_ERROR){
        //emit printLog(QString("shmctl failed\n"), Qt::red);
    }
    if (semctl(sem_pro_id, 0, IPC_RMID, arg) == -1){
        emit printLog(QString("sem_pro failed (close)\n"), Qt::red);
    }
    if (semctl(sem_con_id, 0, IPC_RMID, arg) == -1){
        emit printLog(QString("sem_con failed (close)\n"), Qt::red);
    }
    #endif
    qint64 elapsed_time2 = programExecutionTime2.elapsed();
    qint64 ass = elapsed_time2;
    loop = false;
    //zoomComboBox->setEditable(false);
    emit closeDisplay();
}

void DisplayWindow::finish(int msgid){
    this->msgid = msgid;
	loop = false;
    #ifdef Q_OS_WIN32
	HANDLE hFile = CreateFileW(
            L"\\\\.\\pipe\\SASMPIPE",
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		emit printLog("Could not create file object ("+QString::number(GetLastError())+").\n", Qt::red);
	}
	DWORD dwNoBytesWrote;
	BOOL writeSuccess = WriteFile(
			hFile,
			&buffer,
			BLOCK_SIZE,
			&dwNoBytesWrote,
			NULL);
	CloseHandle(hFile);
    #else
    //sem_post(sem_producer);
    struct sembuf sb = {0, -1, 0};
    sb.sem_op = 1;
    if (semop(sem_pro_id, &sb, 1) == -1) {
        emit printLog(QString("sem_pro failed (semop)\n"), Qt::red);
    }
    #endif
}

void DisplayWindow::zoomSettingsChanged(int value){
    if(!loop){
        zoom = std::pow(2, value);
        scrollAreaWidgetContents->setFixedSize(res_x*zoom+26, res_y*zoom+26);
        this->setFixedSize(QSize(res_x+60, res_y+92));
        scrollAreaWidgetContents->update();
	}
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

DisplayWindow::~DisplayWindow(){
    delete displayImageLabel;
    //delete layout;
}

BufferFrame::BufferFrame(DisplayWindow *parent) :
    QWidget(parent){
    d = parent;
}

void BufferFrame::paintEvent(QPaintEvent*){
	QPainter painter(this);
	//if(!painter.isActive()){
        painter.drawImage(QRect(0,0,d->res_x*d->zoom,d->res_y*d->zoom),*(d->displayPicture));
	//}
}

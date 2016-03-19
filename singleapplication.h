#ifndef SINGLE_APPLICATION_H
#define SINGLE_APPLICATION_H

// Change this to inherit from QGuiApplication or QCoreApplication
#ifndef QAPPLICATION_CLASS
  #define QAPPLICATION_CLASS QApplication
#endif

#define  QUOTE(C) #C
#define  INCLUDE_FILE(C) QUOTE(C)
#include INCLUDE_FILE(QAPPLICATION_CLASS)


/**
 * @file singleapplication.h
 * Defines SingleApplication class.
 */

class SingleApplicationPrivate;

/*!
 * \brief The SingleApplication class provides single-window mode ability.
 */
class SingleApplication : public QAPPLICATION_CLASS
{
  Q_OBJECT
public:
  explicit SingleApplication(int&, char *[]);
  ~SingleApplication();

signals:
  void otherInstanceDataReceived(QByteArray ba);

private slots:
  void slotConnectionEstablished();

private:
  SingleApplicationPrivate *d_ptr;
};

#endif // SINGLE_APPLICATION_H

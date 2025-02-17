#ifndef CLIENT_CONTROLLER_H
#define CLIENT_CONTROLLER_H

#include <QObject>
#include <QApplication>
#include <QColor>
#include "client_window.h"
#include "client_model.h"

class ClientController : public QObject
{
    Q_OBJECT

public:
    ClientController(int argc, char *argv[], QWidget *parent = nullptr);

    int init();

private:
    void connectWindowSignalSlots(QList<bool>& connectionResults);
    void connectModelSignalSlots(QList<bool>& connectionResults);

    QApplication m_app;
    ClientWindow m_window; // ui
    ClientModel m_model;   // data
};

#endif // CLIENT_CONTROLLER_H

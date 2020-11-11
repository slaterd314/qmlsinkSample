#ifndef MYAPP_H
#define MYAPP_H
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <gst/gst.h>
#include "gstLibrary.h"

class MyApp : public QObject
{
    Q_OBJECT
public:
    MyApp();
    virtual ~MyApp();
    GstElement *camerabin{nullptr};
public slots:
    void onTakePicture();
};

#endif // MYAPP_H

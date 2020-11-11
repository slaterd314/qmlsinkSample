#include "MyApp.h"

MyApp::MyApp()
 : QObject()
{}
MyApp::~MyApp(){}
void MyApp::onTakePicture()
{
    if(camerabin)
    {
        g_signal_emit_by_name(G_OBJECT(camerabin),"start-capture", nullptr);
    }
}


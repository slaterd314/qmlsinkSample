#include "MyApp.h"

using namespace thermo;



class SetPlaying : public QRunnable
{
public:
  SetPlaying(GstElement *);
  ~SetPlaying();

  void run ();

private:
  GstElement * pipeline_;
};

SetPlaying::SetPlaying (GstElement * pipeline)
{
  this->pipeline_ = pipeline ? static_cast<GstElement *> (gst_object_ref (pipeline)) : NULL;
}

SetPlaying::~SetPlaying ()
{
  if (this->pipeline_)
    gst_object_unref (this->pipeline_);
}

void
SetPlaying::run ()
{
  if (this->pipeline_)
    gst_element_set_state (this->pipeline_, GST_STATE_PLAYING);
}

static GstElement *glbSink = nullptr;

int runQmlApp(GstElement *pipeline, int argc, char *argv[])
{
    QGuiApplication app{argc, argv};
    QQmlApplicationEngine engine{};
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline),"viewfinder-sink");
    if(!sink)
    {
        sink = glbSink;
        // g_object_get(G_OBJECT(pipeline), "viewfinder-sink", &sink, nullptr);
    }
    g_assert (sink);
    /* find and set the videoItem on the sink */
    auto *rootObject = static_cast<QQuickWindow *> (engine.rootObjects().first());
    g_assert (rootObject);
    auto *videoItem = rootObject->findChild<QQuickItem *> ("videoItem");
    g_assert (videoItem);
    g_object_set(sink, "widget", videoItem, NULL);

    MyApp signalSink{};
    signalSink.camerabin = pipeline;

    QObject::connect(videoItem, SIGNAL(signal_takePicture()),&signalSink,SLOT(onTakePicture()));

    rootObject->scheduleRenderJob (new SetPlaying (pipeline),
        QQuickWindow::BeforeSynchronizingStage);

    return  app.exec();

}

GObjectWrapT<GstElement> create_pipeline3()
{
    auto videoSrc = GstElementFactoryMake("videotestsrc", "src");
    auto wrappersrc = GstElementFactoryMake("wrappercamerabinsrc");
    g_object_set(videoSrc,"pattern", 1, nullptr);
    g_object_set(wrappersrc,"video-source", videoSrc.get(), nullptr);
    auto pipeline = GstElementFactoryMake("camerabin", "cam");
    auto glupload = GstElementFactoryMake("glupload", nullptr);
    auto sink = GstElementFactoryMake ("qmlglsink", "sink");
    g_object_ref(sink.get());
    g_object_ref(glupload.get());
    g_object_ref(videoSrc.get());
    glbSink = sink;
    g_object_set(pipeline.get(), "camera-source", wrappersrc.get(), nullptr);
    g_object_set(pipeline.get(), "viewfinder-sink", sink.get(), nullptr);
    g_object_set(pipeline.get(), "viewfinder-filter", glupload.get(), nullptr);
    g_object_set(pipeline.get(), "mode", 1, nullptr);   // set it to image capture
    return pipeline;
}

GObjectWrapT<GstElement> create_pipeline2()
{
    auto pipeline = GstPipelineNew(nullptr);
    auto src = GstElementFactoryMake("videotestsrc", "src");
    auto glupload = GstElementFactoryMake("glupload", nullptr);
    auto sink = GstElementFactoryMake ("qmlglsink", "sink");

    GstBinAddMany(getGstBin(pipeline), src, glupload, sink);
    GstElementLinkMany(src, glupload, sink);
//    gst_bin_add_many (GST_BIN (pipeline.get()), src, glupload, sink, nullptr);
    return pipeline;
}

GstElement *create_pipeline()
{
    GstElement *pipeline = gst_pipeline_new (NULL);
    GstElement *src = gst_element_factory_make ("videotestsrc", "src");
    GstElement *glupload = gst_element_factory_make ("glupload", NULL);
    GstElement *sink = gst_element_factory_make ("qmlglsink", "sink");
    g_assert (pipeline && src && glupload && sink);
    gst_bin_add_many (GST_BIN (pipeline), src, glupload, sink, NULL);
    gst_element_link_many (src, glupload, sink, NULL);
    return pipeline;
}

int main(int argc, char *argv[])
{

    auto ret = 0;
    gst_init (&argc, &argv);
    {
        gst_debug_set_active(TRUE);
        gst_debug_set_default_threshold(GST_LEVEL_DEBUG);
//    QGuiApplication app(argc, argv);

//      auto *pipeline = create_pipeline();
//      g_assert(pipeline);
        auto pipeline = create_pipeline3();

//    GstElement *pipeline = gst_pipeline_new (NULL);
//    GstElement *src = gst_element_factory_make ("videotestsrc", NULL);
//    GstElement *glupload = gst_element_factory_make ("glupload", NULL);
//    /* the plugin must be loaded before loading the qml file to register the
//     * GstGLVideoItem qml item */
//    GstElement *sink = gst_element_factory_make ("qmlglsink", NULL);
//
//    g_assert (src && glupload && sink);
//
//    gst_bin_add_many (GST_BIN (pipeline), src, glupload, sink, NULL);
//    gst_element_link_many (src, glupload, sink, NULL);

    ret = runQmlApp(pipeline, argc, argv);

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//
//    QQuickItem *videoItem;
//    QQuickWindow *rootObject;
//
//    /* find and set the videoItem on the sink */
//    rootObject = static_cast<QQuickWindow *> (engine.rootObjects().first());
//    videoItem = rootObject->findChild<QQuickItem *> ("videoItem");
//    g_assert (videoItem);
//    g_object_set(sink, "widget", videoItem, NULL);
//
//    rootObject->scheduleRenderJob (new SetPlaying (pipeline),
//        QQuickWindow::BeforeSynchronizingStage);
//
//    ret = app.exec();

        gst_element_set_state (pipeline, GST_STATE_NULL);
        // gst_object_unref (pipeline);
    }

    gst_deinit ();
    return ret;
}

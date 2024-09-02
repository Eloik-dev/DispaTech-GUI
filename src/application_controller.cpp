#include "header/application_controller.h"

using namespace std;
namespace fs = filesystem;

Controller::Controller(FileManager *fileManager)
{
    this->_fileManager = fileManager;
}

void Controller::initializeApplication(int argc, char **argv)
{
    try
    {
        this->window = new GtkWidget();
        this->application = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(this->application, "activate", G_CALLBACK(Controller::initializeWindow), this);

        cout << "Initialized application" << endl;
        int status = g_application_run(G_APPLICATION(this->application), argc, argv);
        g_object_unref(this->application);

        cout << "Application was terminated with status " << status << endl;
    }
    catch (const exception &ex)
    {
        cerr << "An error occurred while initializing the application: " << ex.what() << endl;
    }
}

void Controller::initializeWindow(GtkApplication *app, gpointer user_data)
{
    Controller *controller = static_cast<Controller *>(user_data);

    try
    {
        controller->window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(controller->window), "Window");
        gtk_window_set_default_size(GTK_WINDOW(controller->window), 200, 200);
        cout << "Initialized window" << endl;
    }
    catch (const exception &ex)
    {
        cout << ex.what() << endl;
    }

    gtk_window_present(GTK_WINDOW(controller->window));

    controller->startSlideshow();
}

GtkWidget *Controller::getImageWidget(File *file)
{
    string path = FILES_DIRECTORY + file->name;
    cout << path.c_str() << endl;

    if (!Controller::file_exists(path))
    {
        return nullptr;
    }

    return gtk_image_new_from_file(path.c_str());
}

GtkWidget *Controller::getVideoWidget(File *file)
{
    string path = FILES_DIRECTORY + file->name;
    cout << path.c_str() << endl;

    if (!Controller::file_exists(path))
    {
        return nullptr;
    }

    GtkMediaStream *media_stream = gtk_media_file_new_for_filename(path.c_str());

    // Mute the audio
    gtk_media_stream_set_muted(media_stream, TRUE);

    GtkWidget *video = gtk_video_new_for_media_stream(media_stream);

    gtk_media_stream_play(media_stream);
    return video;
}


bool Controller::file_exists(string path)
{
    ifstream f(path);
    return f.good();
}

gboolean Controller::timeoutCallback(gpointer arguments)
{
    GtkCallbackData *data = static_cast<GtkCallbackData *>(arguments);
    File *file = data->file;

    int file_extension_code = FileManager::getFileExtensionCode(FILES_DIRECTORY + file->name);

    GtkWidget *widget = nullptr;
    switch (file_extension_code)
    {
    case IMAGE_FILE_CODE:
        widget = getImageWidget(file);
        break;
    case VIDEO_FILE_CODE:
        widget = getVideoWidget(file);
        break;
    default:
        cerr << "There was an error reading the file: " << file->name << endl;
        break;
    }

    if (widget != nullptr)
    {
        if (GTK_IS_WINDOW(data->window))
        {
            gtk_window_set_child(GTK_WINDOW(data->window), widget);
        }
        else
        {
            cerr << "Error: data->window is not a GtkWindow" << endl;
        }
    }

    if (file == data->fileManager->files.back())
    {
        g_timeout_add(file->duration, onRestartSlideshow, data);
    }

    return FALSE;
}

gboolean Controller::onRestartSlideshow(gpointer arguments)
{
    GtkCallbackData *data = static_cast<GtkCallbackData *>(arguments);
    Controller *controller = new Controller(data->fileManager);
    controller->window = data->window;
    controller->startSlideshow();

    return FALSE;
}

void Controller::startSlideshow()
{
    this->_fileManager->readFileConfiguration();

    while (this->_fileManager->files.empty())
    {
        sleep_for(seconds(2));
        this->_fileManager->readFileConfiguration();
    }

    int delay = 0;
    for (int i = 0; i < this->_fileManager->files.size(); i++)
    {
        File *file = this->_fileManager->files[i];
        GtkCallbackData *data = new GtkCallbackData;
        data->file = file;
        data->window = this->window;
        data->fileManager = this->_fileManager;

        cout << "Next delay: " << delay << endl;

        g_timeout_add(delay, timeoutCallback, data);

        delay += file->duration;
    }
}
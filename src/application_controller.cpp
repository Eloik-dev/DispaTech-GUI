#include "header/application_controller.h"

using namespace std;
namespace fs = filesystem;

Controller::Controller(FileManager *fileManager)
{
    this->fileManager = fileManager;
}

int Controller::initializeApplication(int argc, char **argv)
{
    try
    {
        GtkApplication *app = gtk_application_new("com.example.VideoLoop", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(Controller::initializeWindow), this);
        std::cout << "Initialized application" << std::endl;

        int status = g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);

        std::cout << "Application was terminated with status " << status << std::endl;
        return status;
    }
    catch (const exception &ex)
    {
        cerr << "An error occurred while initializing the application: " << ex.what() << endl;
        return -1;
    }
}

void Controller::initializeWindow(GtkApplication *app, gpointer controller_data)
{
    Controller *controller = static_cast<Controller *>(controller_data);

    try
    {
        GtkWindow *window = GTK_WINDOW(gtk_application_window_new(app));
        gtk_window_set_title(window, "Video Loop");
        gtk_window_set_default_size(window, 800, 600);
        controller->window = window;

        cout << "Initialized window" << endl;
        gtk_window_present(window);
    }
    catch (const exception &ex)
    {
        cout << ex.what() << endl;
    }

    controller->startSlideshow();
}

GtkWidget *Controller::createImageWidget(std::string path)
{
    return gtk_image_new_from_file(path.c_str());
}

GtkWidget *Controller::createVideoWidget(std::string path)
{
    GtkMediaStream *media_stream = gtk_media_file_new_for_filename(path.c_str());

    if (!media_stream)
    {
        g_warning("Failed to create media stream");
        return nullptr;
    }

    gtk_media_stream_set_muted(media_stream, FALSE);

    GtkWidget *video = gtk_video_new_for_media_stream(media_stream);
    g_object_unref(media_stream);
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
    Partition *partition = data->partition;
    File *file = partition->file;
    GtkWidget *widget = partition->widget;

    data->controller->hideWidgets();
    gtk_widget_show(widget);

    if (GTK_IS_VIDEO(widget))
    {
        GtkMediaStream *media_stream = gtk_video_get_media_stream(GTK_VIDEO(widget));
        gtk_media_stream_play(media_stream);
    }

    if (file->index == data->controller->fileManager->files.size() - 1)
    {
        std::cout << "Restarting" << std::endl;
        g_timeout_add(file->duration, onRestartSlideshow, data);
    }

    return FALSE;
}

gboolean Controller::onRestartSlideshow(gpointer arguments)
{
    GtkCallbackData *data = static_cast<GtkCallbackData *>(arguments);
    data->controller->startSlideshow();

    return FALSE;
}

void Controller::startSlideshow()
{
    this->fileManager->readFileConfiguration();

    while (this->fileManager->files.empty())
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        this->fileManager->readFileConfiguration();
    }

    this->generatePartitions();

    int delay = 0;
    for (auto &partition : this->partitions)
    {
        std::cout << delay << std::endl;
        auto data = std::make_unique<GtkCallbackData>();
        data->partition = partition;
        data->controller = this;

        g_timeout_add(delay, timeoutCallback, data.release());
        delay += partition->file->duration;
    }
}

void Controller::generatePartitions()
{
    this->removePartitions();

    GtkWidget *box = gtk_window_get_child(this->window);
    if (box == nullptr)
    {
        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_window_set_child(this->window, box);
    }

    for (auto &file : this->fileManager->files)
    {
        std::string path = FILES_DIRECTORY + file->name;
        if (!Controller::file_exists(path))
        {
            std::cout << "File `" << path << "` doesn't exist" << std::endl;
            continue;
        }

        GtkWidget *widget = nullptr;

        int file_extension_code = FileManager::getFileExtensionCode(path);
        switch (file_extension_code)
        {
        case IMAGE_FILE_CODE:
            widget = this->createImageWidget(path);
            break;
        case VIDEO_FILE_CODE:
            widget = this->createVideoWidget(path);
            break;
        default:
            std::cerr << "There was an error reading the file: " << file->name << std::endl;
            break;
        }

        if (widget != nullptr)
        {
            gtk_widget_hide(widget);

            Partition *partition = new Partition();
            partition->file = file;
            partition->widget = widget;

            this->partitions.emplace_back(partition);
            gtk_box_append(GTK_BOX(box), widget);
        }
    }
}

void Controller::removePartitions()
{
    for (auto &partition : this->partitions)
    {
        if (partition->widget != NULL)
        {
            if (G_IS_OBJECT(partition->widget))
            {
                gtk_widget_unparent(partition->widget);
                g_object_unref(partition->widget);
                partition->widget = NULL; // Clear the reference
            }
            else
            {
                g_warning("video_widget is not a valid GObject");
            }
        }
        else
        {
            g_warning("video_widget is NULL");
        }

        delete partition;
    }

    this->partitions.clear();
}

void Controller::hideWidgets()
{
    if (this->partitions.empty())
    {
        return;
    }

    for (auto &partition : this->partitions)
    {
        gtk_widget_hide(partition->widget);
    }
}

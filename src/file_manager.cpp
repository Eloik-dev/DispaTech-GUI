#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "header/file_manager.h"

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

FileManager::FileManager(GtkWidget *window)
{
    this->window = window;
    this->currentFileIndex = 0;
    this->initializeFilesDirectory();
    this->readFileConfiguration();
    this->startSlideshow();
}

void FileManager::initializeFilesDirectory()
{
    try
    {
        bool created = fs::create_directory(FILES_DIRECTORY);
        if (created)
        {
            cout << "Files folder was created" << endl;
        }
        else
        {
            cout << "Files folder already exists" << endl;
        }
    }
    catch (const exception *ex)
    {
        cerr << "An error occured during the files directory initialization: " << ex->what() << endl;
    }
}

void FileManager::readFileConfiguration()
{
    try
    {
        std::ifstream f(CONFIGURATION_FILE);
        json configuration = json::parse(f);

        for (json::iterator it = configuration.begin(); it != configuration.end(); ++it)
        {
            json file_object = it.value();
            File *file = new File{
                file_object["duration"].get<double>() * 1000,
                file_object["fileName"].get<std::string>()};

            this->files.push_back(file);
        }

        cout << "Read " << this->files.size() << " files to be shown" << endl;
    }
    catch (const exception *ex)
    {
        cerr << "An error occured while reading the files directory: " << ex->what() << endl;
    }
}

int FileManager::getFileExtensionCode(string fileName)
{
    string extension = fileName.substr(fileName.find_last_of(".") + 1);

    if (extension == "png")
    {
        return IMAGE_FILE_CODE;
    }

    if (extension == "mp4")
    {
        return VIDEO_FILE_CODE;
    }

    return -1;
}

GtkWidget *FileManager::getImageWidget(File *file)
{
    string directory = FILES_DIRECTORY;
    string path = directory + "/" + file->name;
    cout << path.c_str() << endl;
    return gtk_image_new_from_file(path.c_str());
}

GtkWidget *FileManager::getVideoWidget(File *file)
{
    string directory = FILES_DIRECTORY;
    string path = directory + "/" + file->name;
    cout << path.c_str() << endl;

    GtkMediaStream *media_stream = gtk_media_file_new_for_filename(path.c_str());

    GtkWidget *media_controls = gtk_media_controls_new(media_stream);
    gtk_widget_set_visible(media_controls, FALSE);

    GtkWidget *video = gtk_video_new_for_media_stream(media_stream);

    gtk_media_controls_set_media_stream(GTK_MEDIA_CONTROLS(media_controls), media_stream);
    gtk_media_stream_play(media_stream);
    return video;
}

gboolean FileManager::timeoutCallback(gpointer arguments)
{
    GtkCallbackData *data = static_cast<GtkCallbackData *>(arguments);
    File *file = data->file;

    int file_extension_code = FileManager::getFileExtensionCode(file->name);

    GtkWidget *widget;
    switch (file_extension_code)
    {
    case IMAGE_FILE_CODE:
        widget = FileManager::getImageWidget(file);
        break;
    case VIDEO_FILE_CODE:
        widget = FileManager::getVideoWidget(file);
        break;
    default:
        cerr << "There was an error reading the file: " << file->name << endl;
        return FALSE;
        break;
    }

    gtk_window_set_child(GTK_WINDOW(data->window), widget);

    if (file == data->manager->files.back())
    {
        g_timeout_add(file->duration, onRestartSlideshow, data->manager);
    }

    return FALSE;
}

gboolean FileManager::onRestartSlideshow(gpointer arguments)
{
    FileManager *manager = static_cast<FileManager *>(arguments);
    manager->startSlideshow();

    return FALSE;
}

void FileManager::startSlideshow()
{
    int delay = 0;

    for (int i = 0; i < this->files.size(); i++)
    {
        File *file = this->files[i];
        GtkCallbackData *data = new GtkCallbackData;
        data->file = file;
        data->window = this->window;
        data->manager = this;

        cout << "Next delay: " << delay << endl;

        g_timeout_add(delay, timeoutCallback, data);

        delay += file->duration;
    }
}
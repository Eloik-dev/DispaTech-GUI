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
                file_object["start"].get<int>(),
                file_object["stop"].get<int>(),
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

gboolean FileManager::timeoutCallback(gpointer arguments)
{
    cout << "Calling callback" << endl;
    GtkCallbackData *data = static_cast<GtkCallbackData *>(arguments);
    File *file = data->file;

    cout << "Showing file " << file->name << endl;
    string directory = FILES_DIRECTORY;
    string path = directory + "/" + file->name;
    cout << path.c_str() << endl;
    GtkWidget *image = gtk_image_new_from_file(path.c_str());
    gtk_window_set_child(GTK_WINDOW(data->window), image);

    return G_SOURCE_CONTINUE;
}

void FileManager::startSlideshow()
{
    File *file = this->files[this->currentFileIndex];

    GtkCallbackData *data = new GtkCallbackData{file, this->window};

    g_timeout_add(5000, timeoutCallback, data);

    if (this->currentFileIndex >= this->files.size())
    {
        this->currentFileIndex = 0;
    }
    else
    {
        this->currentFileIndex++;
    }
}

/*static gboolean FileManager::showFileRecursive(File *file, int currentFileIndex)
{
    try
    {
        File *file = this->files[this->currentFileIndex];
        GtkWidget *image = gtk_image_new_from_file(FILES_DIRECTORY + "/" + file->name);
        gtk_window_set_child(GTK_WINDOW(window), image);
    }
    catch (const exception &ex)
    {
        cout << ex.what() << endl;
    }

    if (this->files.size() > this->currentFileIndex)
    {
        currentFileIndex = 0;
    }
    else
    {
        this->currentFileIndex++;
    }

    g_timeout_add(3000, this->showFileRecursive, NULL);
}*/
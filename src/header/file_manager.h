#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#define FILES_DIRECTORY "files"
#define CONFIGURATION_FILE "configuration.json"
#define IMAGE_FILE_CODE 0
#define VIDEO_FILE_CODE 1

#include <string>
#include <vector>
#include <gtk/gtk.h>

class FileManager;

struct File
{
    double duration;
    std::string name;
};

struct GtkCallbackData
{
    File *file;
    GtkWidget *window;
    FileManager *manager;
};

class FileManager
{
private:
    GtkWidget *window;
    std::vector<File *> files;
    std::vector<File *> settings;
    int currentFileIndex;

    /**
     * Reads the JSON files configuration
     */
    void readFileConfiguration();

    /**
     * Checks if the `files` directory exists, if not generates it
     */
    void initializeFilesDirectory();

    static gboolean timeoutCallback(gpointer);

    static gboolean onRestartSlideshow(gpointer);

    /**
     * Shows the next file to be shown
     */
    void startSlideshow();

    static int getFileExtensionCode(std::string);
    static GtkWidget *getImageWidget(File *);
    static GtkWidget *getVideoWidget(File *);

public:
    FileManager(GtkWidget *);
};

#endif
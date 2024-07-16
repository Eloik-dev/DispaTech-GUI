#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#define FILES_DIRECTORY "files"
#define CONFIGURATION_FILE "configuration.json"

#include <string>
#include <vector>
#include <gtk/gtk.h>

struct File
{
    int start;
    int stop;
    std::string name;
};

struct GtkCallbackData
{
    File *file;
    GtkWidget *window;
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

    /**
     * Shows the next file to be shown
     */
    void startSlideshow();

public:
    FileManager(GtkWidget *);
};

#endif
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#define FILES_DIRECTORY "files"

#include <string>
#include <vector>
#include <gtk/gtk.h>

struct File {
    std::string path;
};

class FileManager
{
private:
    GtkWidget *window;
    std::vector<File> files;

    /**
     * Reads all files in the `files` directory
     */
    void readFiles();

    /**
     * Checks if the `files` directory exists, if not generates it
     */
    void initializeFilesDirectory();
public:
    FileManager(GtkWidget *);
};

#endif
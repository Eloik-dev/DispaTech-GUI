#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#define IMAGE_FILE_CODE 0
#define VIDEO_FILE_CODE 1

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <gtk/gtk.h>
#include "settings.h"

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
    FileManager *fileManager;
};

class FileManager
{
private:
    std::vector<File *> settings;
    int currentFileIndex;

    /**
     * Checks if the `files` directory exists, if not generates it
     */
    void initializeFilesDirectory();
    
    /**
     * Checks if the `temp` directory exists, if not generates it
     */
    void initializeTempFilesDirectory();

public:
    FileManager();

    std::vector<File *> files;

    /**
     * Reads the JSON files configuration
     */
    void readFileConfiguration();

    static int getFileExtensionCode(std::string);
};

#endif
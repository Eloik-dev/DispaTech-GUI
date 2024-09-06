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
#include <magic.h>
#include "settings.h"

class FileManager;

struct File
{
    int index;
    int start;
    int stop;
    int duration;
    std::string name;
};

class FileManager
{
private:
    std::vector<File *> settings;

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
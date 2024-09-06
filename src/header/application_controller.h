#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include "file_manager.h"

#define DEFAULT_RESTART_SLIDESHOW_DELAY 2000

using namespace std::this_thread;
using namespace std::chrono;

class Controller;

struct Partition
{
    File *file;
    GtkWidget *widget;
};

struct GtkCallbackData
{
    Partition *partition;
    Controller *controller;
};

class Controller
{
private:
    GtkWindow *window;
    FileManager *fileManager;
    std::vector<Partition *> partitions;

    /**
     * Callback d'activation de l'application
     */
    static void initializeWindow(GtkApplication *, gpointer);

    static gboolean timeoutCallback(gpointer);

    static gboolean onRestartSlideshow(gpointer arguments);

    /**
     * Shows the next file to be shown
     */
    void startSlideshow();
    
    /**
     * Generates widgets and assigns files
     */
    void generatePartitions();
    
    /**
     * Reset window partitions
     */
    void removePartitions();
    
    /**
     * Hides all widgets in the window
     */
    void hideWidgets();

    /**
     * Shows a blank screen
     */
    void showBlankScreen();

    static int getFileExtensionCode(std::string);
    GtkWidget* createImageWidget(std::string);
    GtkWidget* createVideoWidget(std::string);
    /**
     * Validates that a file exists
     */
    static bool file_exists(std::string fileName);

public:
    Controller(FileManager *fileManager);

    /**
     * Fait l'initialisation de l'application GTK4
     */
    int initializeApplication(int, char **);
};

#endif
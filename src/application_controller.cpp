#include "header/application_controller.h"

using namespace std;
namespace fs = filesystem;

Controller::Controller(int argc, char **argv) {
    this->initializeApplication(argc, argv);
}

void Controller::initializeApplication(int argc, char **argv) {
    try {
        this->application = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(this->application, "activate", G_CALLBACK(this->initializeWindow), NULL);
        cout << "Initialized application" << endl;
        int status = g_application_run(G_APPLICATION(this->application), argc, argv);
        g_object_unref(this->application);
        
        cout << "Application was terminated with status " << status << endl;
    } catch (const exception *ex) {
        cerr << "An error occured while initializing the application : " << ex->what() << endl;
    }
}

void Controller::initializeWindow(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    try {
        window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "Window");
        gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
        cout << "Initialized window" << endl;
    } catch (const exception& ex) {
        cout << ex.what() << endl;
    }

    FileManager *fileManager = new FileManager(window);

    gtk_window_present(GTK_WINDOW(window));
}
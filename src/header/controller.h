#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <gtk/gtk.h>

class Controller
{
private:
    GtkApplication *application;

    /**
     * Fait l'initialisation de l'application GTK4
     */
    void initializeApplication(int, char **);
    
    /**
     * Callback d'activation de l'application
     */
    static void initializeWindow(GtkApplication *, gpointer);
public:
    Controller(int, char **);
};

#endif
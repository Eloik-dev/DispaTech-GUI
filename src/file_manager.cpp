#include <iostream>
#include <filesystem>
#include <string>
#include "header/file_manager.h"

using namespace std;
namespace fs = filesystem;

FileManager::FileManager(GtkWidget* window) {
    this->window = window;
    this->initializeFilesDirectory();
    this->readFiles();
}

void FileManager::initializeFilesDirectory() {
    try {
        bool created = fs::create_directory(FILES_DIRECTORY);
        if (created) {
            cout << "Files folder was created" << endl;
        } else {
            cout << "Files folder already exists" << endl;
        }
    } catch (const exception* ex) {
        cerr << "An error occured during the files directory initialization: " << ex->what() << endl;
    }
}

void FileManager::readFiles() {
    try {
        for (const auto& entry : fs::directory_iterator(FILES_DIRECTORY)) {
            File file{entry.path().string()};
            this->files.push_back(file);
        }
    } catch (const exception* ex) {
            cerr << "An error occured while reading the files directory: " << ex->what() << endl;
    }
    
    for (const File file : this->files) {
        cout << "Found file: " << file.path << endl;
    }
}
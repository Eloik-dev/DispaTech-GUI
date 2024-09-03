#include "header/file_manager.h"

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

FileManager::FileManager()
{
    this->currentFileIndex = 0;
    this->initializeFilesDirectory();
    this->initializeTempFilesDirectory();
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

void FileManager::initializeTempFilesDirectory()
{
    try
    {
        bool created = fs::create_directory(TEMP_DIRECTORY);
        if (created)
        {
            cout << "Temp files folder was created" << endl;
        }
        else
        {
            cout << "Temp files folder already exists" << endl;
        }
    }
    catch (const exception *ex)
    {
        cerr << "An error occured during the temp files directory initialization: " << ex->what() << endl;
    }
}

void FileManager::readFileConfiguration()
{
    std::cout << "Reading file configuration..." << std::endl;
    try
    {
        std::ifstream f(CONFIGURATION_FILE);

        if (f.good() == false)
        {
            std::cerr << "Could not read configuration file" << std::endl;
            f.close();
            return;
        }

        json configuration = json::parse(f);

        for (File *file : this->files)
        {
            delete file;
        }
        this->files.clear();

        for (json::iterator it = configuration.begin(); it != configuration.end(); ++it)
        {
            json file_object = it.value();
            std::string name = file_object["name"].get<std::string>();
            int start = file_object["start"].get<double>() * 10;
            int stop = file_object["stop"].get<double>() * 10;
            int duration = stop - start;
            File *file = new File{
                start,
                stop,
                duration,
                name};

            this->files.push_back(file);
        }

        std::cout << "Read " << this->files.size() << " files to be shown" << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "An error occurred while reading the files directory: " << ex.what() << std::endl;
    }
}

int FileManager::getFileExtensionCode(string filePath)
{
    magic_t magic = magic_open(MAGIC_MIME_TYPE);
    if (magic == nullptr)
    {
        throw std::runtime_error("Unable to initialize magic library");
    }

    if (magic_load(magic, nullptr) != 0)
    {
        magic_close(magic);
        throw std::runtime_error("Unable to load magic database");
    }

    const char *mimeType = magic_file(magic, filePath.c_str());
    if (mimeType == nullptr)
    {
        magic_close(magic);
        throw std::runtime_error("Unable to determine MIME type");
    }

    std::string mimeTypeStr(mimeType);
    magic_close(magic);

    std::cout << mimeTypeStr << std::endl;

    if (mimeTypeStr == "image/png")
    {
        return IMAGE_FILE_CODE;
    }

    if (mimeTypeStr == "image/jpeg")
    {
        return IMAGE_FILE_CODE;
    }

    if (mimeTypeStr == "video/mp4")
    {
        return VIDEO_FILE_CODE;
    }

    return -1;
}
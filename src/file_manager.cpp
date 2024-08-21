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
            cerr << "Could not read configuration file" << endl;
            return;
        }

        json configuration = json::parse(f);

        if (!this->files.empty())
        {
            this->files.clear();
        }

        for (json::iterator it = configuration.begin(); it != configuration.end(); ++it)
        {
            json file_object = it.value();
            File *file = new File{
                file_object["duration"].get<double>() * 10,
                file_object["name"].get<std::string>()};

            this->files.push_back(file);
        }

        cout << "Read " << this->files.size() << " files to be shown" << endl;
    }
    catch (const exception *ex)
    {
        cerr << "An error occured while reading the files directory: " << ex->what() << endl;
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
    
    if (mimeTypeStr == "image/webp")
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
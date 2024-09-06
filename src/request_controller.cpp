#include "header/request_controller.h"

RequestController::RequestController(Settings *settings)
{
    this->_settings = settings;
}

json RequestController::get(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    long httpCode = 0;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 150);

        struct curl_slist *headers = NULL;
        std::string authorization = "Authorization: jwt_token " + this->_settings->jwt_token;
        headers = curl_slist_append(headers, authorization.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK || httpCode >= 400)
        {
            std::cerr << "GET request failed: " << curl_easy_strerror(res) << " HTTP code: " << httpCode << std::endl;
            return false;
        }
    }

    return json::parse(readBuffer);
}

json RequestController::post(const std::string &url, const json &data)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    long httpCode = 0;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 150);

        struct curl_slist *headers = NULL;
        std::string authorization = "Authorization: jwt_token " + this->_settings->jwt_token;
        headers = curl_slist_append(headers, authorization.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK || httpCode >= 400)
        {
            std::cerr << "POST request failed: " << curl_easy_strerror(res) << " HTTP code: " << httpCode << std::endl;
            return false;
        }
    }

    return json::parse(readBuffer);
}

size_t RequestController::HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    return size * nitems;
}

bool RequestController::getFile(const std::string &fileUrl, const std::string &destination)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    long httpCode = 0;

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(destination.c_str(), "wb");
        if (!fp)
        {
            std::cerr << "Failed to open file for writing: " << destination << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, fileUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 150);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);

        struct curl_slist *headerList = NULL;
        if (fileUrl.find("amazonaws.com") == std::string::npos)
        {
            std::string authorization = "Authorization: jwt_token " + this->_settings->jwt_token;
            headerList = curl_slist_append(headerList, authorization.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        curl_slist_free_all(headerList);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res != CURLE_OK || httpCode >= 400)
        {
            std::cerr << "GET request failed: " << curl_easy_strerror(res) << " HTTP code: " << httpCode << std::endl;
            std::cerr << "Request URL: " << fileUrl << std::endl;
            return false;
        }

        return true;
    }

    return false;
}

size_t RequestController::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char *)contents, newLength);
    }
    catch (std::bad_alloc &e)
    {
        return 0;
    }
    return newLength;
}

size_t RequestController::FileWriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    if (written < size * nmemb)
    {
        std::cerr << "File write error" << std::endl;
    }
    return written;
}
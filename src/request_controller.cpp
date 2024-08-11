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

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist *headers = NULL;
        std::string authorization = "Authorization: jwt_token " + this->_settings->jwt_token;
        headers = curl_slist_append(headers, authorization.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::runtime_error("GET request failed: " + std::string(curl_easy_strerror(res)));
        }
    }

    return json::parse(readBuffer);
}

json RequestController::post(const std::string &url, const json &data)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist *headers = NULL;
        std::string authorization = "Authorization: jwt_token " + this->_settings->jwt_token;
        headers = curl_slist_append(headers, authorization.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::runtime_error("POST request failed: " + std::string(curl_easy_strerror(res)));
        }
    }

    return json::parse(readBuffer);
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
            throw std::runtime_error("Failed to open file for writing: " + destination);
        }

        curl_easy_setopt(curl, CURLOPT_URL, fileUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FileWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res != CURLE_OK || httpCode != 200)
        {
            throw std::runtime_error("GET request failed: " + std::string(curl_easy_strerror(res)) + " HTTP code: " + std::to_string(httpCode));
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
    return written;
}

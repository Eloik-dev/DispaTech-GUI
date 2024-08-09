#ifndef RSA_CONTROLLER_H
#define RSA_CONTROLLER_H

#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class RSAController
{
private:
    std::string base64Encode(const std::string &input);

public:
    bool encryptString(const std::string &inStr, const std::string &publicKeyFile, std::string &outStr);
};

#endif
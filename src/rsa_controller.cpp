#include "header/rsa_controller.h"

bool RSAController::encryptString(const std::string &regKey, const std::string &publicKeyFile, std::string &encryptedKey)
{
    // Load the public key
    std::ifstream pubKeyFile(publicKeyFile, std::ios::binary);
    if (!pubKeyFile)
    {
        std::cerr << "Error opening public key file." << std::endl;
        return false;
    }

    std::string pubKeyStr((std::istreambuf_iterator<char>(pubKeyFile)), std::istreambuf_iterator<char>());
    BIO *bio = BIO_new_mem_buf(pubKeyStr.data(), pubKeyStr.size());
    if (!bio)
    {
        std::cerr << "Error creating BIO." << std::endl;
        return false;
    }

    EVP_PKEY *evpKey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!evpKey)
    {
        std::cerr << "Error reading public key." << std::endl;
        return false;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(evpKey, nullptr);
    if (!ctx)
    {
        std::cerr << "Error creating context." << std::endl;
        EVP_PKEY_free(evpKey);
        return false;
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0 ||
        EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256()) <= 0 ||
        EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha1()) <= 0)
    {
        std::cerr << "Error initializing encryption." << std::endl;
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(evpKey);
        return false;
    }

    size_t outLen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char *>(regKey.c_str()), regKey.size()) <= 0)
    {
        std::cerr << "Error determining buffer length." << std::endl;
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(evpKey);
        return false;
    }

    std::vector<unsigned char> outBuf(outLen);
    if (EVP_PKEY_encrypt(ctx, outBuf.data(), &outLen, reinterpret_cast<const unsigned char *>(regKey.c_str()), regKey.size()) <= 0)
    {
        std::cerr << "Error encrypting data." << std::endl;
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(evpKey);
        return false;
    }

    std::string buffer(outBuf.begin(), outBuf.end());
    encryptedKey = base64Encode(buffer);

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(evpKey);
    return true;
}

std::string RSAController::base64Encode(const std::string &input)
{
    BIO *bio = BIO_new(BIO_s_mem());
    BIO *b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);

    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string encodedData(bufferPtr->data, bufferPtr->length - 1);

    BIO_free_all(bio);
    return encodedData;
}
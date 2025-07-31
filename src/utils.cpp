#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "utils.h"

#define AES_BLOCK_SIZE 16

std::string getHiddenInput(const std::string& prompt) {
    std::string password;
    struct termios oldt, newt;

    std::cout << prompt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
    return password;
}

// Berechnet key für AES
std::string deriveKeyFromPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}

bool encryptAES(const std::string& plaintext, const std::string& key, std::string& ciphertext, std::string& iv_out) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    unsigned char iv[16];
    RAND_bytes(iv, sizeof(iv));
    iv_out.assign(reinterpret_cast<char*>(iv), sizeof(iv));

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::string out;
    out.resize(plaintext.size() + AES_BLOCK_SIZE);
    int outlen1 = 0;

    if (EVP_EncryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(&out[0]), &outlen1,
        reinterpret_cast<const unsigned char*>(plaintext.c_str()),
        plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int outlen2 = 0;
    if (EVP_EncryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(&out[0]) + outlen1, &outlen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    out.resize(outlen1 + outlen2);
    ciphertext = out;

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decryptAES(const std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    int len;
    int plaintext_len;
    plaintext.resize(ciphertext.size());

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(),
                        nullptr,
                        reinterpret_cast<const unsigned char*>(key.data()),
                        reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_DecryptUpdate(ctx,
                        reinterpret_cast<unsigned char*>(&plaintext[0]),
                        &len,
                        reinterpret_cast<const unsigned char*>(ciphertext.data()),
                        ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char*>(&plaintext[0]) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

void printUsage() {
    std::cout << "Ausführbare Befehle:\n"
              << "  add <name> [password]\n"
              << "  get <name>\n"
              << "  list\n"
              << "  verify <name> [password]\n"
              << "  delete <name>\n"
              << "  exit\n"
              << "  help\n";
}

bool confirmDeletion() {
    std::string input;
    std::cout << "Möchten Sie das Passwort wirklich löschen? [Y/n] ";
    std::getline(std::cin, input);

    if (input.empty() || input == "Y" || input == "y") {
        return true;
    }
    return false;
}
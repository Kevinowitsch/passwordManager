#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string getMaskedInput(const std::string& prompt);
std::string getHiddenInput(const std::string& prompt);

std::string deriveKeyFromPassword(const std::string& password);
bool encryptAES(const std::string& plaintext, const std::string& key, std::string& ciphertext, std::string& iv_out);
bool decryptAES(const std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext);

#endif
#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string getMaskedInput(const std::string& prompt);
std::string getHiddenInput(const std::string& prompt);

std::string deriveKeyFromPassword(const std::string& password);
bool encryptAES(const std::string& plaintext, const std::string& key, std::string& ciphertext, std::string& iv_out);

#endif
#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @brief Fragt eine Benutzereingabe ab, ohne die Eingabe im Terminal anzuzeigen.
 *
 * Praktisch für sensible Eingaben wie Passwörter.
 *
 * @param prompt Die Eingabeaufforderung, die dem Benutzer angezeigt wird.
 * @return Die eingegebene Zeichenkette.
 */
std::string getHiddenInput(const std::string& prompt);

/**
 * @brief Leitet aus einem Passwort einen kryptografischen Schlüssel ab.
 *
 * Nutzt SHA-256, um aus dem eingegebenen Passwort einen 256-Bit AES-Schlüssel zu erzeugen.
 *
 * @param password Das Benutzerpasswort.
 * @return Ein 32-Byte langer Schlüssel (als String) zur Verwendung bei AES-Verschlüsselung.
 */
std::string deriveKeyFromPassword(const std::string& password);

/**
 * @brief Verschlüsselt einen Text mit AES-256 im CBC-Modus.
 *
 * Es wird ein zufälliger Initialisierungsvektor (IV) generiert und zurückgegeben.
 *
 * @param plaintext Der zu verschlüsselnde Klartext.
 * @param key Der 256-Bit-Schlüssel (muss 32 Bytes lang sein).
 * @param ciphertext [out] Der resultierende verschlüsselte Text (Base64-codiert).
 * @param iv_out [out] Der verwendete Initialisierungsvektor (Base64-codiert).
 * @return true bei erfolgreicher Verschlüsselung, sonst false.
 */
bool encryptAES(const std::string& plaintext, const std::string& key, std::string& ciphertext, std::string& iv_out);

/**
 * @brief Entschlüsselt einen mit AES-256 verschlüsselten Text.
 *
 * Erwartet einen gültigen Schlüssel und den zur Verschlüsselung verwendeten IV.
 *
 * @param ciphertext Der verschlüsselte Text (Base64-codiert).
 * @param key Der 256-Bit-Schlüssel (muss 32 Bytes lang sein).
 * @param iv Der Initialisierungsvektor (Base64-codiert).
 * @param plaintext [out] Der entschlüsselte Klartext.
 * @return true bei erfolgreicher Entschlüsselung, sonst false.
 */
bool decryptAES(const std::string& ciphertext, const std::string& key, const std::string& iv, std::string& plaintext);

/**
 * @brief Gibt die Befehlsübersicht und Hinweise zur Nutzung des Programms aus.
 */
void printUsage();

/**
 * @brief Fragt den Benutzer, ob er eine Löschaktion wirklich durchführen möchte.
 *
 * Gibt eine einfache Ja/Nein-Abfrage im Terminal aus.
 *
 * @return true, wenn der Benutzer die Löschung bestätigt, sonst false.
 */
bool confirmDeletion();

#endif
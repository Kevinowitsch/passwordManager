#include <iostream>
#include "database.h"
#include "bcrypt.h"

Database::Database(const std::string& db_name) {
    if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Fehler beim Öffnen der Datenbank!\n";
    }
    std::string createTable = "CREATE TABLE IF NOT EXISTS passwords (name TEXT PRIMARY KEY, password TEXT);";
    executeQuery(createTable);
}

Database::~Database() {
    sqlite3_close(db);
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQLite Fehler: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::addPassword(const std::string& name, const std::string& password) {
    char hash[BCRYPT_HASHSIZE];
    char salt[BCRYPT_HASHSIZE];

    if (bcrypt_gensalt(12, salt) != 0) {
        std::cerr << "Fehler beim Erzeugen des Salts." << std::endl;
        return false;
    }

    if (bcrypt_hashpw(password.c_str(), salt, hash) != 0) {
        std::cerr << "Fehler beim Hashen des Passworts." << std::endl;
        return false;
    }

    std::string query = "INSERT INTO passwords (name, password) VALUES ('" + name + "', '" + hash + "');";
    return executeQuery(query);
}

std::string Database::getPassword(const std::string& name) {
    std::string query = "SELECT password FROM passwords WHERE name = '" + name + "';";
    sqlite3_stmt* stmt;
    std::string password = "Kein Passwort gefunden";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }
    sqlite3_finalize(stmt);
    return password;
}

bool Database::verifyPassword(const std::string& name, const std::string& inputPassword) {
    std::string query = "SELECT password FROM passwords WHERE name = '" + name + "';";
    sqlite3_stmt* stmt;
    bool result = false;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* storedHash = sqlite3_column_text(stmt, 0);
            if (storedHash != nullptr &&
                bcrypt_checkpw(inputPassword.c_str(), reinterpret_cast<const char*>(storedHash)) == 0) {
                result = true;
            }
        }
    }

    sqlite3_finalize(stmt);
    return result;
}


bool Database::deletePassword(const std::string& name) {
    std::string query = "DELETE FROM passwords WHERE name = '" + name + "';";
    return executeQuery(query);
}

void Database::listPasswords() {
    std::string query = "SELECT name FROM passwords;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::cout << "Gespeicherte Passwörter:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout << "- " << sqlite3_column_text(stmt, 0) << "\n";
        }
    } else {
        std::cout << "Fehler beim Abrufen der Passwörter.\n";
    }
    sqlite3_finalize(stmt);
}

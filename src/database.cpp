#include <iostream>
#include "database.h"

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
    std::string query = "INSERT INTO passwords (name, password) VALUES ('" + name + "', '" + password + "');";
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

#include <iostream>
#include "database.h"
#include "utils.h"
#include "bcrypt.h"

Database::Database(const std::string& db_name) {
    if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Fehler beim Öffnen der Datenbank!\n";
    }
    std::string createMasterTable = "CREATE TABLE IF NOT EXISTS master (id INTEGER PRIMARY KEY, hash TEXT NOT NULL);";
    if (!executeQuery(createMasterTable)) {
        std::cerr << "❌ Fehler beim Erstellen der Tabelle 'master'\n";
    }
    std::string createPasswordTable = "CREATE TABLE IF NOT EXISTS passwords (name TEXT PRIMARY KEY, password BLOB NOT NULL, iv BLOB NOT NULL);";
    if (!executeQuery(createPasswordTable)) {
        std::cerr << "❌ Fehler beim Erstellen der Tabelle 'passwords'\n";
    }
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

bool Database::isMasterPasswordSet() {
    std::string query = "SELECT COUNT(*) FROM master;";
    sqlite3_stmt* stmt;
    bool isSet = false;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
            isSet = true;
        }
    }
    sqlite3_finalize(stmt);
    return isSet;
}

bool Database::setMasterPassword(const std::string& password) {
    char hash[BCRYPT_HASHSIZE];
    char salt[BCRYPT_HASHSIZE];

    if (bcrypt_gensalt(12, salt) != 0 || bcrypt_hashpw(password.c_str(), salt, hash) != 0) {
        std::cerr << "Fehler beim Hashen des Masterpassworts.\n";
        return false;
    }

    std::string query = "INSERT INTO master (hash) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK){
        return false;
    }

    sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_STATIC);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::verifyMasterPassword(const std::string& inputPassword) {
    std::string query = "SELECT hash FROM master LIMIT 1;";
    sqlite3_stmt* stmt;
    bool result = false;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (storedHash && bcrypt_checkpw(inputPassword.c_str(), storedHash) == 0) {
                result = true;
            }
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

bool Database::addPassword(const std::string& name, const std::string& password, const std::string& masterPassword) {
    std::string key = deriveKeyFromPassword(masterPassword);

    std::string ciphertext;
    std::string iv;
    if (!encryptAES(password, key, ciphertext, iv)) {
        std::cerr << "Fehler bei der Verschlüsselung des Passworts.\n";
        return false;
    }

    // Angenommen, du hast die Tabelle so angepasst:
    // CREATE TABLE IF NOT EXISTS passwords (name TEXT PRIMARY KEY, password BLOB, iv BLOB);
    std::string query = "INSERT INTO passwords (name, password, iv) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Fehler beim Vorbereiten der SQL-Anweisung: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, ciphertext.data(), ciphertext.size(), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, iv.data(), iv.size(), SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}


std::string Database::getPassword(const std::string& name, const std::string& masterPassword) {
    const char* query = "SELECT password, iv FROM passwords WHERE name = ?;";
    sqlite3_stmt* stmt;
    std::string decryptedPassword = "❌ Kein Passwort gefunden oder Entschlüsselung fehlgeschlagen";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const void* ciphertext_blob = sqlite3_column_blob(stmt, 0);
            int ciphertext_size = sqlite3_column_bytes(stmt, 0);
            const void* iv_blob = sqlite3_column_blob(stmt, 1);
            int iv_size = sqlite3_column_bytes(stmt, 1);

            std::string ciphertext(reinterpret_cast<const char*>(ciphertext_blob), ciphertext_size);
            std::string iv(reinterpret_cast<const char*>(iv_blob), iv_size);

            std::string key = deriveKeyFromPassword(masterPassword);
            std::string plaintext;

            if (decryptAES(ciphertext, key, iv, plaintext)) {
                decryptedPassword = plaintext;
            }
        }
    } else {
        std::cerr << "Fehler beim SQL-Statement: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return decryptedPassword;
}

bool Database::verifyPassword(const std::string& name, const std::string& inputPassword) {
    const char* query = "SELECT password FROM passwords WHERE name = ?;";
    sqlite3_stmt* stmt;
    bool result = false;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        // Benutzername sicher binden
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* storedHash = sqlite3_column_text(stmt, 0);
            if (storedHash != nullptr &&
                bcrypt_checkpw(inputPassword.c_str(), reinterpret_cast<const char*>(storedHash)) == 0) {
                result = true;
            }
        }
    } else {
        std::cerr << "Fehler beim Vorbereiten des SQL-Statements: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return result;
}


bool Database::deletePassword(const std::string& name) {
    const char* query = "DELETE FROM passwords WHERE name = ?;";
    sqlite3_stmt* stmt;
    bool success = false;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    } else {
        std::cerr << "Fehler beim Vorbereiten des SQL-Statements: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

void Database::listAccounts() {
    const char* query = "SELECT name FROM passwords;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        bool foundAny = false;
        std::cout << "Gespeicherte Passwörter:\n";

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            if (name) {
                std::cout << "- " << name << "\n";
                foundAny = true;
            }
        }

        if (!foundAny) {
            std::cout << "(Keine Einträge gefunden)\n";
        }

    } else {
        std::cerr << "Fehler beim Abrufen der Passwörter: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
}

#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    Database(const std::string& db_name);
    ~Database();

    bool isMasterPasswordSet();
    bool setMasterPassword(const std::string& password);
    bool verifyMasterPassword(const std::string& inputPassword);

    bool addPassword(const std::string& name, const std::string& password, const std::string& masterPassword);
    std::string getPassword(const std::string& name, const std::string& masterPassword);
    bool verifyPassword(const std::string& name, const std::string& inputPassword);
    bool deletePassword(const std::string& name);
    void listAccounts();

private:
    sqlite3* db;
    bool executeQuery(const std::string& query);
};

#endif

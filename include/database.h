#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    Database(const std::string& db_name);
    ~Database();
    bool addPassword(const std::string& name, const std::string& password);
    std::string getPassword(const std::string& name);
    bool deletePassword(const std::string& name);
    void listPasswords();

private:
    sqlite3* db;
    bool executeQuery(const std::string& query);
};

#endif

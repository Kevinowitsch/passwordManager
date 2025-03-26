#include <iostream>
#include "database.h"

int main(int argc, char* argv[]) {
    Database db("passwords.db");

    if (argc < 2) {
        std::cout << "Verwendung: ./passwordManager <Befehl> [Argumente]\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "add" && argc == 4) {
        if (db.addPassword(argv[2], argv[3])) {
            std::cout << "Passwort gespeichert.\n";
        }
    } else if (command == "get" && argc == 3) {
        std::cout << "Passwort: " << db.getPassword(argv[2]) << "\n";
    } else if (command == "delete" && argc == 3) {
        if (db.deletePassword(argv[2])) {
            std::cout << "Passwort gelöscht.\n";
        }
    } else if (command == "list") {
        db.listPasswords();
    } else {
        std::cout << "Ungültiger Befehl!\n";
    }

    return 0;
}

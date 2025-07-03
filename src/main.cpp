#include <iostream>
#include "database.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    Database db("passwords.db");
    std::string masterInput;

    //TODO Entweder parameter übergeben oder Loop der auf eingabe wartet
    if (db.isMasterPasswordSet()) {
        masterInput = getHiddenInput("🔐 Bitte gib dein Masterpasswort ein: ");

        if (!db.verifyMasterPassword(masterInput)) {
            std::cerr << "❌ Falsches Masterpasswort. Zugriff verweigert.\n";
            return 1;
        }
        std::cout << "✅ Zugriff gewährt.\n";
    } else {
        masterInput = getHiddenInput("🔐 Kein Masterpasswort gesetzt. Bitte neues Masterpasswort eingeben: ");

        if (!db.setMasterPassword(masterInput)) {
            std::cerr << "❌ Fehler beim Speichern des Masterpassworts.\n";
            return 1;
        }
        std::cout << "✅ Masterpasswort gesetzt.\n";
    }
    

    if (argc < 2) {
        std::cout << "Verwendung: ./passwordManager <Befehl> [Argumente]\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "add" && argc == 3) {
        std::string pw = getHiddenInput("🔑 Passwort eingeben: ");
        if (pw.empty()) {
            std::cerr << "⚠️  Passwort darf nicht leer sein!\n";
            return 1;
        }

        if (db.addPassword(argv[2], pw, masterInput)) {
            std::cout << "✅ Passwort gespeichert.\n";
        } else {
            std::cerr << "❌ Fehler beim Speichern des Passworts.\n";
        }
    }
    else if (command == "add" && argc == 4) {
        std::string pw(argv[3]);
        if (pw.empty()) {
            std::cerr << "⚠️  Passwort fehlt oder ist leer. Tipp: Sonderzeichen wie '>' in Anführungszeichen setzen!\n";
            return 1;
        }

        if (db.addPassword(argv[2], pw, masterInput)) {
            std::cout << "✅ Passwort gespeichert.\n";
        } else {
            std::cerr << "❌ Fehler beim Speichern des Passworts.\n";
        }
    } else if (command == "get" && argc == 3) {
        std::string password = db.getPassword(argv[2], masterInput);
        std::cout << "🔓 Passwort: " << password << "\n";
    }
    else if (command == "verify" && argc == 4){
        bool status = db.verifyPassword(argv[2], argv[3]);
        if (status)
        {
            std::cout << "\033[32m" << "Passwort korrekt" << "\033[0m" << "\n";
        } else {
            std::cout << "\033[31m" << "Passwort falsch" << "\033[0m" << "\n";
        }
    } else if (command == "delete" && argc == 3) {
        if (db.deletePassword(argv[2])) {
            std::cout << "Passwort gelöscht.\n";
        }
    } else if (command == "list") {
        db.listAccounts();
    } else {
        std::cout << "Ungültiger Befehl!\n";
    }

    return 0;
}

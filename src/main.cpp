#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "database.h"
#include "utils.h"


int main(int argc, char* argv[]) {

    Database db("passwords.db");
    
    // Spezialmodus: Masterpasswort ändern
    if (argc == 2 && std::string(argv[1]) == "change_master") {
        std::string oldPw = getHiddenInput("Altes Masterpasswort: ");
        if (!db.verifyMasterPassword(oldPw)) {
            std::cerr << "\033[31mFalsches Masterpasswort.\033[0m\n";
            return 1;
        }
        std::string newPw = getHiddenInput("Neues Masterpasswort: ");
        if (db.setMasterPassword(newPw)) {
            std::cout << "\033[32mMasterpasswort erfolgreich geändert.\033[0m\n";
        } else {
            std::cerr << "\033[31mFehler beim Ändern des Masterpassworts.\033[0m\n";
        }
        return 0;
    }

    // Spezialmodus: Masterpasswort verifizieren
    if (argc == 2 && std::string(argv[1]) == "verify_master") {
        std::string pw = getHiddenInput("Masterpasswort: ");
        if (db.verifyMasterPassword(pw)) {
            std::cout << "\033[32mMasterpasswort korrekt.\033[0m\n";
            return 0;
        } else {
            std::cerr << "\033[31mFalsches Masterpasswort.\033[0m\n";
            return 1;
        }
    }
    
    // Ungültiger Aufruf mit Argumenten
    if (argc != 1) {
        std::cerr << "\033[31mFalscher Aufruf des Programms\033[0m\n";
        std::cerr << "Beispiele:\n ./passwordManager\n ./passwordManager change_master\n ./passwordManager verify_master\n";
        return 1;
    }
    
    // --- Interaktiver Modus ---
    std::string masterInput;

    if (db.isMasterPasswordSet()) {
        masterInput = getHiddenInput("Bitte gib dein Masterpasswort ein: ");

        if (!db.verifyMasterPassword(masterInput)) {
            std::cerr << "\033[31mFalsches Masterpasswort. Zugriff verweigert.\033[0m\n";
            return 1;
        }
        std::cout << "\033[32mZugriff gewährt.\033[0m\n";
    } else {
        masterInput = getHiddenInput("Kein Masterpasswort gesetzt. Bitte neues Masterpasswort eingeben: ");

        if (!db.setMasterPassword(masterInput)) {
            std::cerr << "\033[31mFehler beim Speichern des Masterpassworts.\033[0m\n";
            return 1;
        }
        std::cout << "\033[32mMasterpasswort gesetzt.\033[0m\n";
    }

    printUsage();

    std::string input;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::vector<std::string> args{
            std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>{}
        };

        if (args.empty()) continue;

        std::string command = args[0];

        // EXIT
        if (command == "exit") {
            std::cout << "Programm beendet.\n";
            break;

        // ADD
        } else if (command == "add" && args.size() == 2) {
            std::string pw = getHiddenInput("Passwort eingeben: ");
            if (pw.empty()) {
                std::cerr << "\033[31mPasswort darf nicht leer sein!\033[0m\n";
                continue;
            }
            if (db.addPassword(args[1], pw, masterInput)) {
                std::cout << "\033[32mPasswort gespeichert.\033[0m\n";
            } else {
                std::cerr << "\033[31mFehler beim Speichern des Passworts.\033[0m\n";
            }
        } else if (command == "add" && args.size() == 3) {
            if (args[2].empty()) {
                std::cerr << "\033[31mPasswort darf nicht leer sein!\033[0m\n";
                continue;
            }
            if (db.addPassword(args[1], args[2], masterInput)) {
                std::cout << "\033[32mPasswort gespeichert.\033[0m\n";
            } else {
                std::cerr << "\033[31mFehler beim Speichern des Passworts.\033[0m\n";
            }
        
        // GET
        } else if (command == "get" && args.size() == 2) {
            std::string password = db.getPassword(args[1], masterInput);
            std::cout << "Passwort: " << password << "\n";

        // VERIFY
        } else if (command == "verify" && args.size() == 2) {
            std::string pw = getHiddenInput("Passwort eingeben: ");
            if (db.verifyPassword(args[1], pw, masterInput)) {
                std::cout << "\033[32mPasswort korrekt.\033[0m\n";
            } else {
                std::cout << "\033[31mPasswort falsch.\033[0m\n";
            }
        } else if (command == "verify" && args.size() == 3) {
            if (db.verifyPassword(args[1], args[2], masterInput)) {
                std::cout << "\033[32mPasswort korrekt.\033[0m\n";
            } else {
                std::cout << "\033[31mPasswort falsch.\033[0m\n";
            }

        // DELETE
        } else if (command == "delete" && args.size() == 2) {
            if (confirmDeletion())
            {
                if (db.deletePassword(args[1])) {
                    std::cout << "\033[32mPasswort gelöscht.\033[0m\n";
                } else {
                    std::cerr << "\033[31mFehler beim Löschen.\033[0m\n";
                }
            }          

        // LIST
        } else if (command == "list") {
            db.listAccounts();

        // HELP
        } else if (command == "help") {
            printUsage();
        
        // UKNOWN COMMAND
        } else {
            std::cerr << "\033[31mUngültiger Befehl.\033[0m\n";
            printUsage();
        }
    }

    return 0;
}
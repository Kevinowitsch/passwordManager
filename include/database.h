#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    /**
     * @brief Erstellt eine neue Datenbankverbindung.
     *
     * Öffnet oder erstellt eine SQLite-Datenbank mit dem gegebenen Dateinamen.
     *
     * @param db_name Der Name bzw. Pfad zur SQLite-Datenbankdatei.
     */
    Database(const std::string& db_name);

    /**
     * @brief Schließt die Datenbankverbindung und räumt auf.
     */
    ~Database();

    /**
     * @brief Prüft, ob ein Masterpasswort in der Datenbank gesetzt ist.
     *
     * Diese Funktion führt eine SQL-Abfrage auf die Tabelle `master` aus,
     * um zu ermitteln, ob bereits ein Masterpasswort gespeichert wurde.
     *
     * @return true, wenn mindestens ein Eintrag in der `master`-Tabelle existiert, sonst false.
     */
    bool isMasterPasswordSet();

    /**
     * @brief Setzt oder ändert das Masterpasswort.
     *
     * Das Passwort wird mit bcrypt gehasht und in der Tabelle `master` gespeichert.
     *
     * @param password Das neue Masterpasswort im Klartext.
     * @return true bei Erfolg, false bei Fehler.
     */
    bool setMasterPassword(const std::string& password);

    /**
     * @brief Verifiziert das eingegebene Masterpasswort gegen den gespeicherten Hash.
     *
     * Verwendet bcrypt zur Prüfung der Übereinstimmung.
     *
     * @param inputPassword Das eingegebene Masterpasswort.
     * @return true, wenn das Passwort korrekt ist, sonst false.
     */
    bool verifyMasterPassword(const std::string& inputPassword);

    /**
     * @brief Fügt ein neues Passwort für einen Dienst hinzu.
     *
     * Das Passwort wird mit AES verschlüsselt und in der Datenbank gespeichert.
     *
     * @param name Der Name des Dienstes oder Accounts.
     * @param password Das zu speichernde Passwort.
     * @param masterPassword Das Masterpasswort zur Verschlüsselung.
     * @return true bei Erfolg, false bei Fehler.
     */
    bool addPassword(const std::string& name, const std::string& password, const std::string& masterPassword);

    /**
     * @brief Ruft ein gespeichertes Passwort ab.
     *
     * Das Passwort wird mit dem angegebenen Masterpasswort entschlüsselt.
     *
     * @param name Der Name des Dienstes oder Accounts.
     * @param masterPassword Das Masterpasswort zur Entschlüsselung.
     * @return Das entschlüsselte Passwort bei Erfolg, sonst ein leerer String.
     */
    std::string getPassword(const std::string& name, const std::string& masterPassword);

    /**
     * @brief Prüft, ob ein gespeichertes Passwort mit einer Benutzereingabe übereinstimmt.
     *
     * Das gespeicherte Passwort wird entschlüsselt und mit dem eingegebenen verglichen.
     *
     * @param name Der Name des Accounts.
     * @param inputPassword Das zu prüfende Passwort.
     * @param masterPassword Das Masterpasswort zur Entschlüsselung.
     * @return true, wenn die Passwörter übereinstimmen, sonst false.
     */
    bool verifyPassword(const std::string& name, const std::string& inputPassword, const std::string& masterPassword);

    /**
     * @brief Löscht einen Passwort-Eintrag aus der Datenbank.
     *
     * @param name Der Name des Dienstes oder Accounts.
     * @return true bei Erfolg, false bei Fehler.
     */
    bool deletePassword(const std::string& name);

    /**
     * @brief Gibt eine Liste aller gespeicherten Accountnamen aus.
     */
    void listAccounts();

private:
    /// Zeiger auf das SQLite-Datenbankobjekt.
    sqlite3* db;

    /**
     * @brief Führt eine allgemeine SQL-Abfrage ohne Ergebnisverarbeitung aus.
     *
     * @param query Die SQL-Abfrage als String.
     * @return true bei Erfolg, false bei Fehler.
     */
    bool executeQuery(const std::string& query);
};


#endif

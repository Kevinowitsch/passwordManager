# 🛡️ PasswordManager (C++ / SQLite)

Ein einfacher Passwortmanager in C++, der Passwörter verschlüsselt in einer SQLite-Datenbank speichert.

## ⚙️ Setup

1. Systemabhängige Abhängigkeiten installieren
   ```
   sudo apt-get update
   sudo apt-get install build-essential cmake libsqlite3-dev libssl-dev
   ```

2. Repository klonen:
   ```bash
   git clone https://github.com/Kevinowitsch/passwordManager.git
   cd passwordManager
   ```

3. Setup-Skript ausführen (erstellt den `build/`-Ordner, führt `cmake` und `make` aus):
   ```bash
   bash setup.sh
   ```

4. Programm ausführen aus dem `build/`-Ordner:
   ```bash
   ./passwordManager                  # Interaktiver Modus zum Speichern und Abrufen der Passwörter
   ./passwordManager change_master    # Masterpasswort ändern
   ./passwordManager verify_master    # Masterpasswort überprüfen
   ```

## 📦 Verfügbare Befehle im interaktiven Modus

- Passwort hinzufügen:
  ```bash
  >add <name> [password]  # Passwort optional, wird sonst abgefragt  
  ```

- Passwort abrufen:
  ```bash
  >get <name>
  ```

- Alle gespeicherten Einträge auflisten:
  ```bash
  >list
  ```

- Passwort überprüfen:
  ```bash
  >verify <name> [password]  # Passwort optional, wird sonst abgefragt  
  ```

- Passwort löschen:
  ```bash
  >delete <name>
  ```

- Programm beenden:
  ```bash
  >exit
  ```

- Hilfe:
  ```bash
  >help
  ```


## 🔐 Sicherheit

- Gespeicherte Passwörter werden mit AES-256 sicher verschlüsselt. Der Verschlüsselungsschlüssel wird aus einem vom Benutzer gewählten Masterpasswort abgeleitet, das den Zugriff auf alle gespeicherten Daten schützt.
- Die SQLite-Datenbankdatei befindet sich im `build/`-Verzeichnis (`passwords.db`).

## 📁 Projektstruktur

```text
passwordManager/
├── build/ (wird bei ausführung von setup.sh erzeugt)
├── include/
│   └── database.h
│   └── utils.h
├── src/
│   ├── main.cpp
│   └── database.cpp
│   └── utils.cpp
├── setup.sh
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

🛠 Entwickelt unter Linux (WSL) mit CMake, SQLite, OpenSSL und bcrypt. Viel Spaß beim Ausprobieren!
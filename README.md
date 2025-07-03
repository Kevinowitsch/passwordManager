# 🛡️ PasswordManager (C++ / SQLite)

Ein einfacher Passwortmanager in C++, der Passwörter verschlüsselt in einer SQLite-Datenbank speichert.

## ➕ TODO
-Master Passwort ändern
-Session Token
-CMakeList allgemein gestalten (user clont, installiert)

## ⚙️ Setup

1. Repository klonen:
   ```bash
   git clone https://github.com/Kevinowitsch/passwordManager.git
   cd passwordManager
   ```

2. Setup-Skript ausführen (erstellt den `build/`-Ordner, führt `cmake` und `make` aus):
   ```bash
   bash setup.sh
   ```

3. Programm ausführen aus dem `build/`-Ordner:
   ```bash
   ./passwordManager <funktion>
   ```

## 📦 Verfügbare Befehle

- Passwort hinzufügen:
  ```bash
  ./passwordManager add <name> <passwort>
  ```

- Passwort abrufen:
  ```bash
  ./passwordManager get <name>
  ```

- Passwort überprüfen:
  ```bash
  ./passwordManager verify <name> <passwort>
  ```

- Passwort löschen:
  ```bash
  ./passwordManager delete <name>
  ```

- Alle gespeicherten Einträge auflisten:
  ```bash
  ./passwordManager list
  ```

## 🔐 Sicherheit

- Passwörter werden **gehasht** mit `bcrypt` und nicht im Klartext gespeichert.
- Die SQLite-Datenbankdatei befindet sich im `build/`-Verzeichnis (`passwords.db`).

## 📁 Projektstruktur

```text
passwordManager/
├── build/ (wird bei ausführung von setup.sh erzeugt)
├── include/
│   └── database.h
├── src/
│   ├── main.cpp
│   └── database.cpp
├── setup.sh
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

🛠 Entwickelt unter Linux (WSL) mit CMake, SQLite und bcrypt. Viel Spaß beim Ausprobieren!
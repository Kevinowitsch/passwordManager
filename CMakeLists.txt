cmake_minimum_required(VERSION 3.10)
project(passwordManager)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    passwordManager
    src/main.cpp
    src/database.cpp
    src/utils.cpp
    /home/kevinfriedrich/projects/_libraries/libbcrypt/bcrypt.o
)

find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(passwordManager SQLite::SQLite3 OpenSSL::Crypto /usr/local/lib/bcrypt.a)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    /home/kevinfriedrich/projects/_libraries/libbcrypt
)

link_directories(/usr/local/lib)

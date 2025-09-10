cmake_minimum_required(VERSION 3.16)
project(Particle_sandbox)

set(CMAKE_CXX_STANDARD 17)

# Найти пакет SFML (graphics, window и system)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Указать исходные файлы
add_executable(Particle_sandbox main.cpp)

# Привязать библиотеки SFML
target_link_libraries(Particle_sandbox sfml-graphics sfml-window sfml-system)

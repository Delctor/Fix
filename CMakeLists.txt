cmake_minimum_required(VERSION 3.10)
project(fix)

set(CMAKE_CXX_STANDARD 20)

# Ruta al directorio de headers de la librería
include_directories(${INTEL_LIBRARY_PATH}/src)

# Crear el ejecutable desde el main
add_executable(fix 
    src/main.cpp
    src/Fix.cpp
    #src/parser/Parser.cpp
    #src/fixMsg/FixMsg.cpp
)

# Ruta donde está la librería .a
target_link_directories(fix PRIVATE ${INTEL_LIBRARY_PATH})

# Enlazar la librería estática libbid.a
target_link_libraries(fix PRIVATE bid)


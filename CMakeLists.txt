cmake_minimum_required(VERSION 3.10)
project(fix)

option(ENABLE_OPTIMIZATION "Activar optimización -O2" OFF)

set(CMAKE_CXX_STANDARD 23)

include_directories(${INTEL_LIBRARY_PATH}/src)

# Fuentes
set(SOURCES
    src/main.cpp
    src/Fix.cpp
    # src/parser/Parser.cpp
    # src/fixMsg/FixMsg.cpp
)

add_executable(fix ${SOURCES})

# Compilación normal con -O2
if(ENABLE_OPTIMIZATION)
    message(STATUS "Compilación con -O2 habilitada")
    target_compile_options(fix PRIVATE -O2 -maes -msse4.2)
else()
    message(STATUS "Compilación SIN optimización (-O0)")
    target_compile_options(fix PRIVATE -O0 -maes -msse4.2)
endif()


target_link_directories(fix PRIVATE ${INTEL_LIBRARY_PATH})
target_link_libraries(fix PRIVATE bid)

# ---------- TARGET PARA GENERAR ENSAMBLADOR CON COMENTARIOS -----------

# Opción para activar/desactivar la generación de ASM
option(BUILD_ASM "Generar código ensamblador con comentarios" OFF)

if(BUILD_ASM)
    message(STATUS "ASM generation ENABLED")

    # Directorios fuente y build
    set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

    # Ensamblador para main.cpp (puedes repetir por más archivos)
    add_custom_command(
        OUTPUT ${BIN_DIR}/main.s
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -msse4.2 -maes -O2 -S -fverbose-asm ${SRC_DIR}/main.cpp -o ${BIN_DIR}/main.s
        DEPENDS ${SRC_DIR}/main.cpp
        COMMENT "Generando ensamblador con comentarios para main.cpp"
    )

    add_custom_target(asm ALL DEPENDS ${BIN_DIR}/main.s)
else()
    message(STATUS "ASM generation DISABLED")
endif()

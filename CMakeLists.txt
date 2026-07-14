cmake_minimum_required(VERSION 3.21)
project(BellezaSys LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

# nucleo do sistema (sem Qt): regras de negocio do salao
file(GLOB CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp)
add_library(bellezasys_core STATIC ${CORE_SOURCES})
target_include_directories(bellezasys_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# aplicativo com a interface Qt
add_executable(bellezasys_gui
    src/gui/main.cpp
    src/gui/MainWindow.cpp
)
target_link_libraries(bellezasys_gui PRIVATE bellezasys_core Qt6::Widgets)

# testes funcionais do nucleo (sem GUI)
add_executable(funcional_tests
    test/funcional/main.cpp
    test/funcional/funcional_tests.cpp
)
target_include_directories(funcional_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test/funcional)
target_link_libraries(funcional_tests PRIVATE bellezasys_core)

# testes unitarios do nucleo (uma suite por classe)
file(GLOB UNIT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/unit/*.cpp)
add_executable(unit_tests ${UNIT_SOURCES})
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test/unit)
target_link_libraries(unit_tests PRIVATE bellezasys_core)

# testes automatizados da GUI (Qt Test)
add_executable(gui_tests
    test/gui/gui_tests.cpp
    src/gui/MainWindow.cpp
)
target_include_directories(gui_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/gui)
target_link_libraries(gui_tests PRIVATE bellezasys_core Qt6::Widgets Qt6::Test)

enable_testing()
add_test(NAME unit COMMAND unit_tests)
add_test(NAME funcional COMMAND funcional_tests)
add_test(NAME gui COMMAND gui_tests)

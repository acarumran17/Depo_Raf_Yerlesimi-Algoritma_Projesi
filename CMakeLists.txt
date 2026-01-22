cmake_minimum_required(VERSION 3.16)
project(algo_proje)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
qt_standard_project_setup()

add_executable(algo_proje
        main.cpp
        model/Product.h
        model/Shelf.h
        algorithm/WarehouseAlgorithms.h
        algorithm/WarehouseAlgorithms.cpp
        ui/MainWindow.h
        ui/MainWindow.cpp
)

target_link_libraries(algo_proje PRIVATE Qt6::Widgets Qt6::Charts)

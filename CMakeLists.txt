cmake_minimum_required(VERSION 3.7...3.25)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
    DAC8532 
    VERSION 1.0
    DESCRIPTION "C driver for the DAC8532 dual channel, 16-bit digital-to-analog converter using STM32F7 HAL."
    LANGUAGES CXX
)

add_library(
    dac8532  
    src/dac8532.c include/dac8532.h
)

set_target_properties(dac8532 PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dac8532 PROPERTIES PUBLIC_HEADER include/dac8532.h)
target_include_directories(dac8532 PUBLIC include)

#find_package(HAL COMPONENTS STM32F7 REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
#target_link_libraries(dac8532 ${HAL})


include(GNUInstallDirs)
install(
    TARGETS dac8532 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
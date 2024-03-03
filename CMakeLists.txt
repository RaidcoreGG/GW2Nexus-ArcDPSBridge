cmake_minimum_required(VERSION 3.28)

# set default name if not overridden
if (NOT DEFINED BRIDGE_FILENAME)
    set(BRIDGE_FILENAME "arcdps_integration64")
endif ()

project(GW2Nexus-ArcDPSBridge)

find_package(ArcdpsExtension CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(curl REQUIRED)
find_path(NEXUS_INCLUDE_DIRS "Nexus/Nexus.h")

include(${ArcdpsExtension_DIR}/vs-version.cmake)

generate_version_file(
        VERSION_MAJOR 0
        VERSION_MINOR 1
        VERSION_PATCH 0
        VERSION_REF 0
        INTERNAL_NAME "${BRIDGE_FILENAME}.dll"
        COPYRIGHT "Copyright (c) 2024 RaidcoreGG"
        ORIGINAL_FILENAME "${BRIDGE_FILENAME}.dll"
        PRODUCT_NAME "GW2Nexus-ArcDPSBridge"
)

add_library(${PROJECT_NAME} SHARED dllmain.cpp
        EventHandler.h
        BridgeData.h
        "${VERSION_FILE}")

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Use -MT / -MTd runtime library
set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${BRIDGE_FILENAME}")

target_link_libraries(${PROJECT_NAME} PRIVATE Version.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ArcdpsExtension::ArcdpsExtension)
target_link_libraries(${PROJECT_NAME} PRIVATE magic_enum::magic_enum)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
target_include_directories(${PROJECT_NAME} PRIVATE ${NEXUS_INCLUDE_DIRS})

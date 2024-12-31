
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("HidingFolderSystem")

# Add executable target
add_executable(HidingFolderSystem "HidingFolderSystem.c" "Headers/shared.h" "Headers/DatabaseF.h" "Functions/DatabaseF.c" "Headers/CryptF.h" "Functions/CryptF.c" "Functions/FolderF.c" "Headers/FolderF.h")


# link libraries and directories
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
    find_package(OpenSSL REQUIRED)
    target_link_libraries(HidingFolderSystem PRIVATE OpenSSL::SSL OpenSSL::Crypto)

    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    target_link_libraries(HidingFolderSystem PRIVATE unofficial::sqlite3::sqlite3)
    add_custom_command(TARGET HidingFolderSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/bin/sqlite3.dll" $<TARGET_FILE_DIR:HidingFolderSystem>)


    include_directories(${CMAKE_SOURCE_DIR}/Headers)
    include_directories(${CMAKE_SOURCE_DIR}/Functions)

# Set C++ standard (if applicable)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HidingFolderSystem PROPERTY CXX_STANDARD 20)
endif()

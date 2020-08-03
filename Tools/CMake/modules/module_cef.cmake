# -----------------------------------------------------------------------------
# Copyright (c) 2015 GarageGames, LLC
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
# -----------------------------------------------------------------------------

# module CEF 3

option(TORQUE_CEF_WEB "Add Cef web browser plugin" OFF)

if(APPLE AND TORQUE_CEF_WEB)
    message(FATAL_ERROR "Apple operating systems are not yet supported with the Torque3D CeF web browser plugin.")
endif()

if("${TORQUE_CEF_PATH}" STREQUAL "")
    set(TORQUE_CEF_PATH "" CACHE PATH "Path to the Cef standard distribution directory.")
endif()

if( NOT TORQUE_CEF_WEB )
    return()
endif()

#no path we can't go any further
if("${TORQUE_CEF_PATH}" STREQUAL "")
    message(FATAL_ERROR "No Cef path selected")
endif()

# Source
addPathRec( "${srcDir}/cef" )

# Includes
addInclude( "${TORQUE_CEF_PATH}" )

if( WIN32 )
    #find lib_cef & libcef_dll_wrapper
    #release
    find_library(CEF_LIBRARY NAMES cef libcef PATHS ${TORQUE_CEF_PATH}/Release/ REQUIRED)
    find_library(CEF_DLL_WRAPPER_LIBRARY NAMES cef_dll_wrapper libcef_dll_wrapper PATHS ${TORQUE_CEF_PATH}/libcef_dll_wrapper/Release/ REQUIRED)
    #debug
    find_library(CEF_LIBRARY_DEBUG NAMES cef libcef PATHS ${TORQUE_CEF_PATH}/Debug/ REQUIRED)
    find_library(CEF_DLL_WRAPPER_LIBRARY_DEBUG NAMES cef_dll_wrapper libcef_dll_wrapper PATHS ${TORQUE_CEF_PATH}/libcef_dll_wrapper/Debug/ REQUIRED)

    set(CEF_LIBS ${CEF_LIBRARY} ${CEF_DLL_WRAPPER_LIBRARY})
    set(CEF_LIBS_DEBUG ${CEF_LIBRARY_DEBUG} ${CEF_DLL_WRAPPER_LIBRARY_DEBUG})

    # Add the libs
    addLibRelease("${CEF_LIBS}")
    addLibDebug("${CEF_LIBS_DEBUG}")

    # Add the cef process project
    addLibSrc("${CMAKE_CURRENT_SOURCE_DIR}/libraries/cefproc.cmake")

    # Install files
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/chrome_elf.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/libcef.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/libEGL.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/libGLESv2.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Release/natives_blob.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/snapshot_blob.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")
    INSTALL(FILES "${TORQUE_CEF_PATH}/Release/v8_context_snapshot.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Release")

    # Because the debug libraries and bin files have identical names, we only install release.
    # If you need step through debugging of the chromium libs, you'll need the following files.
    # See: https://www.chromium.org/developers/how-tos/debugging-on-windows for more details.
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/chrome_elf.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/libcef.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/libEGL.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/libGLESv2.dll" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/natives_blob.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/snapshot_blob.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
    #INSTALL(FILES "${TORQUE_CEF_PATH}/Debug/v8_context_snapshot.bin" DESTINATION "${projectOutDir}" CONFIGURATIONS "Debug")
endif()

if( UNIX AND NOT APPLE )
    #Find the wrapper library
    if(TORQUE_BUILD_TYPE STREQUAL "Debug")
        set(cefLibDir "${TORQUE_CEF_PATH}/Debug")
        find_library(CEF_DLL_WRAPPER_LIBRARY NAMES cef_dll_wrapper libcef_dll_wrapper PATHS ${TORQUE_CEF_PATH}/libcef_dll_wrapper/ REQUIRED)
    else()
        set(cefLibDir "${TORQUE_CEF_PATH}/Release")
        find_library(CEF_DLL_WRAPPER_LIBRARY NAMES cef_dll_wrapper libcef_dll_wrapper PATHS ${TORQUE_CEF_PATH}/libcef_dll_wrapper/ REQUIRED)
    endif()

    addLib("${CEF_DLL_WRAPPER_LIBRARY}")

    #Copy the shared library before build and link against the local
    configure_file("${cefLibDir}/libcef.so" "${projectOutDir}/libcef.so" COPYONLY)
    addLib("${projectOutDir}/libcef.so")

    # Add the cef process project
    addLibSrc("${CMAKE_CURRENT_SOURCE_DIR}/libraries/cefproc.cmake")

    #INSTALL(FILES "${cefLibDir}/libcef.so" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/libEGL.so" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/libGLESv2.so" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/chrome-sandbox" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/natives_blob.bin" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/snapshot_blob.bin" DESTINATION "${projectOutDir}")
    INSTALL(FILES "${cefLibDir}/v8_context_snapshot.bin" DESTINATION "${projectOutDir}")
endif()

# Install the cef .pak files
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/icudtl.dat" DESTINATION "${projectOutDir}")
INSTALL(FILES "${TORQUE_CEF_PATH}/LICENSE.txt" DESTINATION "${projectOutDir}/cef")
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/cef.pak" DESTINATION "${projectOutDir}/cef")
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/cef_100_percent.pak" DESTINATION "${projectOutDir}/cef")
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/cef_200_percent.pak" DESTINATION "${projectOutDir}/cef")
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/cef_extensions.pak" DESTINATION "${projectOutDir}/cef")
INSTALL(FILES "${TORQUE_CEF_PATH}/Resources/devtools_resources.pak" DESTINATION "${projectOutDir}/cef")

# Install the Locale files
file(GLOB_RECURSE CEFLOCALE_FILES_AND_DIRS "${TORQUE_CEF_PATH}/Resources/locales/*")

foreach(ITEM ${CEFLOCALE_FILES_AND_DIRS})
    get_filename_component( dir ${ITEM} DIRECTORY )
    STRING(REGEX REPLACE "${TORQUE_CEF_PATH}/Resources/locales" "${projectOutDir}/cef/locales/" INSTALL_DIR ${dir})
    install( FILES ${ITEM} DESTINATION ${INSTALL_DIR} )
endforeach()

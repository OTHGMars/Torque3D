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

# module Steamworks API

option(TORQUE_STEAMWORKS "Add Steamworks API plugin" OFF)
option(TORQUE_STEAMWORKS_REQUIRED "Check if the engine should not launch without the steam client running." OFF)

if(APPLE AND TORQUE_STEAMWORKS)
   message(FATAL_ERROR "Apple operating systems are not yet supported with the Torque3D Steamworks API plugin.")
endif()

if("${TORQUE_STEAMWORKS_APPID}" STREQUAL "")
    set(TORQUE_STEAMWORKS_APPID 0 CACHE INTEGER "The Steam App ID of your game.")
endif()

if("${TORQUE_STEAMWORKS_SDKPATH}" STREQUAL "")
   set(TORQUE_STEAMWORKS_SDKPATH "" CACHE PATH "Path to the Steamworks API /sdk directory.")
endif()

#if(WIN32 AND TORQUE_CPU_X32)
#    option(TORQUE_STEAMWORKS_MINIDUMP "Add Steamworks minidump support. WIN32 only." OFF)
#endif()

if( NOT TORQUE_STEAMWORKS )
    return()
endif()

# If no path or appid, we can't go any further
if("${TORQUE_STEAMWORKS_SDKPATH}" STREQUAL "")
   message(FATAL_ERROR "No Steamworks API path selected")
endif()
if(("${TORQUE_STEAMWORKS_APPID}" STREQUAL "") OR ("${TORQUE_STEAMWORKS_APPID}" STREQUAL "0"))
   message(FATAL_ERROR "No Steamworks App ID entered")
endif()

# Source
addPath( "${srcDir}/steamworks" )

# Includes
addInclude( "${TORQUE_STEAMWORKS_SDKPATH}/public/steam" )

# Libs
if( WIN32 ) 
    if( TORQUE_CPU_X64 )
        set(STEAMWORKS_LIBPATH ${TORQUE_STEAMWORKS_SDKPATH}/redistributable_bin/win64)
        set(STEAMWORKS_LIBNAME steam_api64)
    else()
        set(STEAMWORKS_LIBPATH ${TORQUE_STEAMWORKS_SDKPATH}/redistributable_bin)
        set(STEAMWORKS_LIBNAME steam_api)
    endif()
endif()
if( UNIX AND NOT APPLE )
    if( TORQUE_CPU_X64 )
        set(STEAMWORKS_LIBPATH ${TORQUE_STEAMWORKS_SDKPATH}/redistributable_bin/linux64)
    else()
        set(STEAMWORKS_LIBPATH ${TORQUE_STEAMWORKS_SDKPATH}/redistributable_bin/linux32)
    endif()
    set(STEAMWORKS_LIBNAME libsteam_api)
endif()

find_library(STEAMWORKS_LIBRARY NAMES ${STEAMWORKS_LIBNAME} PATHS ${STEAMWORKS_LIBPATH}/ REQUIRED)
addLib( "${STEAMWORKS_LIBRARY}" )

# Install files
if( WIN32 )
   INSTALL(FILES "${STEAMWORKS_LIBPATH}/${STEAMWORKS_LIBNAME}.dll" DESTINATION "${projectOutDir}")
endif()
if( UNIX AND NOT APPLE )
   INSTALL(FILES "${STEAMWORKS_LIBPATH}/${STEAMWORKS_LIBNAME}.so" DESTINATION "${projectOutDir}")
endif()

CONFIGURE_FILE("${srcDir}/steamworks/setup/steamConfig.h.in" "${srcDir}/steamworks/steamConfig.h")
CONFIGURE_FILE("${srcDir}/steamworks/setup/steam_appid.txt.in" "${projectOutDir}/steam_appid.txt")

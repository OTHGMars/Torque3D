
# module openvr

option(TORQUE_OPENVR "Enable openvr module" OFF)
#mark_as_advanced(TORQUE_OPENVR)
if(TORQUE_OPENVR)
    if(TORQUE_OPENVR_SDK_PATH STREQUAL "")
        set(TORQUE_OPENVR_SDK_PATH "" CACHE PATH "openvr library path" FORCE)
    endif()
else() # hide variable
    set(TORQUE_OPENVR_SDK_PATH "" CACHE INTERNAL "" FORCE) 
endif() 
 
if(TORQUE_OPENVR)
    # Source
    addPathRec( "${srcDir}/platform/input/openVR" )

    # Includes
    addInclude( "${TORQUE_OPENVR_SDK_PATH}/headers" )

	# Libs
    if( WIN32 ) 
        if( TORQUE_CPU_X64 )
            find_library(OPENVR_LIBRARY NAMES openvr_api PATHS ${TORQUE_OPENVR_SDK_PATH}/lib/win64/ REQUIRED)
        else()
            find_library(OPENVR_LIBRARY NAMES openvr_api PATHS ${TORQUE_OPENVR_SDK_PATH}/lib/win32/ REQUIRED)
        endif()
        addLib( "${OPENVR_LIBRARY}" )
    endif()

    if(UNIX AND NOT APPLE)
        # On linux, we need to copy the .so to the project directory during the configuration step
        # and then link against that.
        if(TORQUE_BUILD_TYPE STREQUAL "Debug")
            set(ovrLibName "libopenvr_api.so.dbg")
        else()
            set(ovrLibName "libopenvr_api.so")
        endif()
        if( TORQUE_CPU_X64 )
            find_file(OPENVR_LIBRARY NAMES ${ovrLibName} PATHS ${TORQUE_OPENVR_SDK_PATH}/lib/linux64/ REQUIRED)
        else()
            find_file(OPENVR_LIBRARY NAMES ${ovrLibName} PATHS ${TORQUE_OPENVR_SDK_PATH}/lib/linux32/ REQUIRED)
        endif()
        configure_file("${OPENVR_LIBRARY}" "${projectOutDir}/${ovrLibName}" COPYONLY)
        addLib( "${projectOutDir}/${ovrLibName}" )
    endif()

    if(APPLE)
        find_library(OPENVR_LIBRARY NAMES libopenvr_api PATHS ${TORQUE_OPENVR_SDK_PATH}/lib/osx32/ REQUIRED)
        addLib( "${OPENVR_LIBRARY}" )
    endif()

    # Install Files
    if( WIN32 )
        if( TORQUE_CPU_X64 )
            install(FILES "${TORQUE_OPENVR_SDK_PATH}/bin/win64/openvr_api.dll" DESTINATION "${projectOutDir}")
        else()
            install(FILES "${TORQUE_OPENVR_SDK_PATH}/bin/win32/openvr_api.dll" DESTINATION "${projectOutDir}")
        endif()
    endif()
    if(APPLE)
        if( TORQUE_CPU_X64 )
            set(OPENVR_BINPATH ${TORQUE_OPENVR_SDK_PATH}/bin/osx64)
        else()
            set(OPENVR_BINPATH ${TORQUE_OPENVR_SDK_PATH}/bin/osx32)
        endif()

        file(GLOB_RECURSE OPENVR_FILES_AND_DIRS "${OPENVR_BINPATH}/*")
        foreach(ITEM ${OPENVR_FILES_AND_DIRS})
            get_filename_component( dir ${ITEM} DIRECTORY )
            STRING(REGEX REPLACE "${OPENVR_BINPATH}/" "${projectOutDir}/" INSTALL_DIR ${dir})
            install( FILES ${ITEM} DESTINATION ${INSTALL_DIR} )
        endforeach()
    endif()

    addDef(TORQUE_OPENVR)
endif()

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

project(cef_process)

# cef_process sources.
addFile(${libDir}/cefproc/cefsimple/simple_app.cc)
addFile(${libDir}/cefproc/cefsimple/simple_app.h)

if(WIN32)
   addFile(${libDir}/cefproc/cefsimple/cefsimple.exe.manifest)
   addFile(${libDir}/cefproc/cefsimple/cefsimple.rc)
   addFile(${libDir}/cefproc/cefsimple/cefsimple_win.cc)
   addFile(${libDir}/cefproc/cefsimple/resource.h)
   addFile(${libDir}/cefproc/cefsimple/res/cefsimple.ico)
   addFile(${libDir}/cefproc/cefsimple/res/small.ico)
endif()

if(UNIX AND NOT APPLE)
  addFile(${libDir}/cefproc/cefsimple/cefsimple_linux.cc)
endif()

if(APPLE)
   addFile(${libDir}/cefproc/cefsimple/process_helper_mac.cc)
endif()

LIST(APPEND ${PROJECT_NAME}_paths "${libDir}/cefproc")

addInclude( "${TORQUE_CEF_PATH}" )

# Add the libs - these set from module_cef.cmake
addLibRelease("${CEF_LIBS}")
addLibDebug("${CEF_LIBS_DEBUG}")

finishExecutable()

# set the debug executable postfix property - be sure to match WebEngine::mSubProcessPath in c++ land with any changes to this postfix property
set_target_properties(cef_process PROPERTIES DEBUG_POSTFIX "_DEBUG")

# Main wxMathPlot CMakeLists.txt
# Manage project
#
# Author: Davide Rondini
# Last Update: 2008-02-20
# License: wxWindows license

project(wxMathPlot)

#set(LINUX_64_32_CROSSCOMPILE TRUE)

if(LINUX_64_32_CROSSCOMPILE)
    message(STATUS "Cross compiling from Linux x86_64 to Linux x86")
else(LINUX_64_32_CROSSCOMPILE)
    message(STATUS "Native build")
endif(LINUX_64_32_CROSSCOMPILE)

SET(wxWidgets_USE_LIBS base core)
find_package(wxWidgets)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    add_library(mathplot mathplot.cpp mathplot.h)
    if(LINUX_64_32_CROSSCOMPILE)
        set_source_files_properties(mathplot.cpp PROPERTIES COMPILE_FLAGS "-m32" )
        set_target_properties(mathplot PROPERTIES LINK_FLAGS "-m32")
    endif(LINUX_64_32_CROSSCOMPILE)
    target_link_libraries(mathplot ${wxWidgets_LIBRARIES})
    add_subdirectory(samples)
else(wxWidgets_FOUND)
    MESSAGE("wxWidgets not found!")
endif(wxWidgets_FOUND)
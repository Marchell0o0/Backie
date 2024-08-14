# /path/to/your/project/Backie/CMakeLists.txt
project(BackieGui VERSION 0.1 LANGUAGES CXX)

# Define the executable and list all the source files
qt_add_executable(BackieGui
    MANUAL_FINALIZATION
	${CMAKE_CURRENT_SOURCE_DIR}/../BackieGui/main.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/datavec.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/pch.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/schedule.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/taskscheduler.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/template.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/templatebuilder.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/utils.h


	${CMAKE_CURRENT_SOURCE_DIR}/../src/pch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/datavec.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/schedule.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/taskscheduler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/template.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/templatebuilder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/utils.cpp
)

# Include other headers and nlohmann library
target_include_directories(BackieGui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../nlohmann
)

# Include precompiled header
target_precompile_headers(BackieGui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/pch.h)

# Link libraries
target_link_libraries(BackieGui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
	spdlog::spdlog
	${CMAKE_CURRENT_SOURCE_DIR}/../taskschd.lib
)

add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# Set target properties for different platforms
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BackieGui)
endif()

set_target_properties(BackieGui PROPERTIES
    ${BUNDLE_ID_OPTION}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS BackieGui
    BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize the executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BackieGui)
endif()
























# cmake_minimum_required(VERSION 3.16)

# project(Backie VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# # find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
# # find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# qt_add_executable(Backie
#    MANUAL_FINALIZATION

#    include/datavec.h
#    include/pch.h
#    include/schedule.h
#    include/taskscheduler.h
#    include/template.h
#    include/templatebuilder.h
#    include/utils.h

#    src/main.cpp
#    src/pch.cpp
#    src/datavec.cpp
#    src/schedule.cpp
#    src/taskscheduler.cpp
#    src/template.cpp
#    src/templatebuilder.cpp
#    src/utils.cpp
# )

# # include other headers and nlohmann library
# target_include_directories(Backie PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/include
#    ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann
# )

# # include spdlog library
# add_subdirectory(spdlog)

# # include precompiled header
# target_precompile_headers(Backie PRIVATE include/pch.h)

# target_link_libraries(Backie PRIVATE

#    Qt${QT_VERSION_MAJOR}::Widgets
#    # Qt${QT_VERSION_MAJOR}::Core

#    spdlog::spdlog

#    # HAS TO BE WITH CURRENT DIR SPECIFIER.
#    # STANDARD QT VERSION DOESN'T HAVE WEEKLY AND MONTHLY TRIGGERS
#    ${CMAKE_CURRENT_SOURCE_DIR}/taskschd.lib
# )

# add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# # If you are developing for iOS or macOS you should consider setting an
# # explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Backie)
# endif()

# set_target_properties(Backie PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

# include(GNUInstallDirs)
# install(TARGETS Backie
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(Backie)
# endif()

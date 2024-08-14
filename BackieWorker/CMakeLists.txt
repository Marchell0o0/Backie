# /path/to/your/project/BackieWorker/CMakeLists.txt
project(BackieWorker LANGUAGES CXX)

# Define the executable
add_executable(BackieWorker
    ${CMAKE_CURRENT_SOURCE_DIR}/../BackieWorker/main.cpp

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
target_include_directories(BackieWorker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../nlohmann
)

# Include precompiled header
target_precompile_headers(BackieWorker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/pch.h)

# Link the Core Qt module
target_link_libraries(
   BackieWorker Qt${QT_VERSION_MAJOR}::Core
   spdlog::spdlog
   ${CMAKE_CURRENT_SOURCE_DIR}/../taskschd.lib
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS BackieWorker
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

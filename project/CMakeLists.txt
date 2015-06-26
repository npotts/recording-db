cmake_minimum_required(VERSION 2.8.11)


SET(APPNAME "Recording-db")


set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON) # Instruct CMake to run uic on ui files automatically when needed.
set(CMAKE_AUTORCC ON) #same, but for RCC files
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall") # enable warnings (always good)

project(${APPNAME})
include (InstallRequiredSystemLibraries)
INCLUDE_DIRECTORIES(${project_BINARY_DIR}/source/gui)

find_package(Qt4 REQUIRED QtGui QtNetwork QtSql)
#find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Network REQUIRED)
#find_package(Qt5Sql REQUIRED)
#find_package(Qt5PrintSupport REQUIRED)

SET(SRC main.cpp recording_db.cpp recording_db.h recording_db.ui)
QT4_WRAP_UI(UIS_HDRS recording_db.ui)

SET(RESOURCES resources.qrc)

MESSAGE(STATUS "Sources: ")
foreach(S ${SRC})
	MESSAGE(STATUS "\t * ${S}")
endforeach(S)
MESSAGE(STATUS "Installing to : " ${CMAKE_INSTALL_PREFIX})

add_executable(${APPNAME} ${SRC} ${RESOURCES} ${UIS_HDRS}) # Tell CMake to create the executable
target_link_libraries(${APPNAME} Qt4::QtGui Qt4::QtNetwork Qt4::QtSql)
#target_link_libraries(${APPNAME} Qt5::Widgets Qt5::Network Qt5::PrintSupport Qt5::Sql)

#install(TARGETS ${APPNAME} RUNTIME DESTINATION ".")
#install(TARGETS ${APPNAME} RUNTIME DESTINATION "dist" LIBRARY DESTINATION "dist" ARCHIVE DESTINATION "dist")

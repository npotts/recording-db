cmake_minimum_required(VERSION 2.8.11)

SET(APPNAME "recording-db")


set(CMAKE_INCLUDE_CURRENT_DIR ON) # Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOUIC ON) # Instruct CMake to run uic on ui files automatically when needed.
set(CMAKE_AUTORCC ON) #same, but for RCC files
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall") # enable warnings (always good)

project(${APPNAME})

#use our own mirror of a built qwt
find_package(Qt4 REQUIRED QtGui QtNetwork QtSql)
add_executable(myexe main.cpp)
target_link_libraries(myexe Qt4::QtGui Qt4::QtXml)



SET(SRC main.cpp recording_db.cpp recording_db.h)

SET(RESOURCES resources.qrc)

MESSAGE(STATUS "Sources: ")
foreach(S ${SRC})
	MESSAGE(STATUS "\t * ${S}")
endforeach(S)
MESSAGE(STATUS "Installing to : " ${CMAKE_INSTALL_PREFIX})

add_executable(${APPNAME} ${SRC} ${RESOURCES} ${UIS_HDRS}) # Tell CMake to create the executable
target_link_libraries(${APPNAME} Qt4::QtGui Qt4::QtNetwork Qt4::QtSql)

install(TARGETS ${APPNAME} RUNTIME DESTINATION ".")
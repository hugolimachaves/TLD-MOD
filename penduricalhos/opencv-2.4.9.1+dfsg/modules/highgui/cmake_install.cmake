# Install script for directory: /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/highgui

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "libs")
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so.2.4.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/usr/local/lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_highgui.so.2.4.9"
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_highgui.so.2.4"
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_highgui.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so.2.4.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_highgui.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib:"
           NEW_RPATH "/usr/local/lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/highgui" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/highgui/include/opencv2/highgui/highgui.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/highgui" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/highgui/include/opencv2/highgui/highgui_c.h")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/highgui" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/highgui/include/opencv2/highgui/cap_ios.h")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/highgui" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/highgui/include/opencv2/highgui/ios.h")
endif()


# Install script for directory: /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core

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
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so.2.4.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/usr/local/lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_core.so.2.4.9"
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_core.so.2.4"
    "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/lib/libopencv_core.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so.2.4.9"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_core.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "::::::::::::::"
           NEW_RPATH "/usr/local/lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/internal.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/opengl_interop.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/eigen.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/operations.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/core.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/opengl_interop_deprecated.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/version.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/mat.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/affine.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/gpumat.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/wimage.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/cuda_devptrs.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/devmem2d.hpp")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/core_c.h")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include/opencv2/core/types_c.h")
endif()


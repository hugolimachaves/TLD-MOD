# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg

# Utility rule file for pch_Generate_opencv_flann.

# Include the progress variables for this target.
include modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/progress.make

modules/flann/CMakeFiles/pch_Generate_opencv_flann: modules/flann/precomp.hpp.gch/opencv_flann_Release.gch


modules/flann/precomp.hpp.gch/opencv_flann_Release.gch: modules/flann/src/precomp.hpp
modules/flann/precomp.hpp.gch/opencv_flann_Release.gch: modules/flann/precomp.hpp
modules/flann/precomp.hpp.gch/opencv_flann_Release.gch: lib/libopencv_flann_pch_dephelp.a
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating precomp.hpp.gch/opencv_flann_Release.gch"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann && /usr/bin/cmake -E make_directory /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/precomp.hpp.gch
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann && /usr/bin/c++ -O3 -DNDEBUG -DNDEBUG -fPIC -isystem"/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg" -isystem"/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/include" -isystem"/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/src" -isystem"/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann" -isystem"/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/core/include" -fsigned-char -W -Wall -Werror=return-type -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wno-narrowing -Wno-delete-non-virtual-dtor -fdiagnostics-show-option -Wno-long-long -pthread -fomit-frame-pointer -msse -msse2 -msse3 -ffunction-sections -DCVAPI_EXPORTS -x c++-header -o /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/precomp.hpp.gch/opencv_flann_Release.gch /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/precomp.hpp

modules/flann/precomp.hpp: modules/flann/src/precomp.hpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating precomp.hpp"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann && /usr/bin/cmake -E copy /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/src/precomp.hpp /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/precomp.hpp

pch_Generate_opencv_flann: modules/flann/CMakeFiles/pch_Generate_opencv_flann
pch_Generate_opencv_flann: modules/flann/precomp.hpp.gch/opencv_flann_Release.gch
pch_Generate_opencv_flann: modules/flann/precomp.hpp
pch_Generate_opencv_flann: modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/build.make

.PHONY : pch_Generate_opencv_flann

# Rule to build all files generated by this target.
modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/build: pch_Generate_opencv_flann

.PHONY : modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/build

modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/clean:
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann && $(CMAKE_COMMAND) -P CMakeFiles/pch_Generate_opencv_flann.dir/cmake_clean.cmake
.PHONY : modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/clean

modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/depend:
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/flann/CMakeFiles/pch_Generate_opencv_flann.dir/depend


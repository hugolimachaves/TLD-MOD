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

# Include any dependencies generated for this target.
include modules/video/CMakeFiles/opencv_perf_video.dir/depend.make

# Include the progress variables for this target.
include modules/video/CMakeFiles/opencv_perf_video.dir/progress.make

# Include the compile flags for this target's objects.
include modules/video/CMakeFiles/opencv_perf_video.dir/flags.make

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o: modules/video/CMakeFiles/opencv_perf_video.dir/flags.make
modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o: modules/video/perf/perf_optflowpyrlk.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o -c /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_optflowpyrlk.cpp

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.i"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -E /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_optflowpyrlk.cpp > CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.i

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.s"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -S /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_optflowpyrlk.cpp -o CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.s

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.requires:

.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.requires

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.provides: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.requires
	$(MAKE) -f modules/video/CMakeFiles/opencv_perf_video.dir/build.make modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.provides.build
.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.provides

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.provides.build: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o


modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o: modules/video/CMakeFiles/opencv_perf_video.dir/flags.make
modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o: modules/video/perf/perf_tvl1optflow.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o -c /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_tvl1optflow.cpp

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.i"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -E /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_tvl1optflow.cpp > CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.i

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.s"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -S /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_tvl1optflow.cpp -o CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.s

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.requires:

.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.requires

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.provides: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.requires
	$(MAKE) -f modules/video/CMakeFiles/opencv_perf_video.dir/build.make modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.provides.build
.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.provides

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.provides.build: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o


modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o: modules/video/CMakeFiles/opencv_perf_video.dir/flags.make
modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o: modules/video/perf/perf_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -o CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o -c /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_main.cpp

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.i"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -E /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_main.cpp > CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.i

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.s"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -include "/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf_precomp.hpp" -Winvalid-pch  -S /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/perf/perf_main.cpp -o CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.s

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.requires:

.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.requires

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.provides: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.requires
	$(MAKE) -f modules/video/CMakeFiles/opencv_perf_video.dir/build.make modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.provides.build
.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.provides

modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.provides.build: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o


# Object files for target opencv_perf_video
opencv_perf_video_OBJECTS = \
"CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o" \
"CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o" \
"CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o"

# External object files for target opencv_perf_video
opencv_perf_video_EXTERNAL_OBJECTS =

bin/opencv_perf_video: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o
bin/opencv_perf_video: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o
bin/opencv_perf_video: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o
bin/opencv_perf_video: modules/video/CMakeFiles/opencv_perf_video.dir/build.make
bin/opencv_perf_video: lib/libopencv_video.so.2.4.9
bin/opencv_perf_video: lib/libopencv_ts.a
bin/opencv_perf_video: lib/libopencv_features2d.so.2.4.9
bin/opencv_perf_video: lib/libopencv_highgui.so.2.4.9
bin/opencv_perf_video: lib/libopencv_imgproc.so.2.4.9
bin/opencv_perf_video: lib/libopencv_flann.so.2.4.9
bin/opencv_perf_video: lib/libopencv_core.so.2.4.9
bin/opencv_perf_video: modules/video/CMakeFiles/opencv_perf_video.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../../bin/opencv_perf_video"
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_perf_video.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
modules/video/CMakeFiles/opencv_perf_video.dir/build: bin/opencv_perf_video

.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/build

modules/video/CMakeFiles/opencv_perf_video.dir/requires: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_optflowpyrlk.cpp.o.requires
modules/video/CMakeFiles/opencv_perf_video.dir/requires: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_tvl1optflow.cpp.o.requires
modules/video/CMakeFiles/opencv_perf_video.dir/requires: modules/video/CMakeFiles/opencv_perf_video.dir/perf/perf_main.cpp.o.requires

.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/requires

modules/video/CMakeFiles/opencv_perf_video.dir/clean:
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video && $(CMAKE_COMMAND) -P CMakeFiles/opencv_perf_video.dir/cmake_clean.cmake
.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/clean

modules/video/CMakeFiles/opencv_perf_video.dir/depend:
	cd /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video /home/hugo/Documents/Mestrado/codigoRastreador/penduricalhos/opencv-2.4.9.1+dfsg/modules/video/CMakeFiles/opencv_perf_video.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/video/CMakeFiles/opencv_perf_video.dir/depend


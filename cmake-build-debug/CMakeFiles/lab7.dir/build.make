# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.1\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\SubjectsIn5Sem\ComputerNetworks\lab7

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/lab7.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/lab7.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lab7.dir/flags.make

CMakeFiles/lab7.dir/client.c.obj: CMakeFiles/lab7.dir/flags.make
CMakeFiles/lab7.dir/client.c.obj: ../client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lab7.dir/client.c.obj"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\lab7.dir\client.c.obj   -c E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c

CMakeFiles/lab7.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab7.dir/client.c.i"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c > CMakeFiles\lab7.dir\client.c.i

CMakeFiles/lab7.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab7.dir/client.c.s"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c -o CMakeFiles\lab7.dir\client.c.s

CMakeFiles/lab7.dir/server.c.obj: CMakeFiles/lab7.dir/flags.make
CMakeFiles/lab7.dir/server.c.obj: ../server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/lab7.dir/server.c.obj"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\lab7.dir\server.c.obj   -c E:\SubjectsIn5Sem\ComputerNetworks\lab7\server.c

CMakeFiles/lab7.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab7.dir/server.c.i"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\SubjectsIn5Sem\ComputerNetworks\lab7\server.c > CMakeFiles\lab7.dir\server.c.i

CMakeFiles/lab7.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab7.dir/server.c.s"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\SubjectsIn5Sem\ComputerNetworks\lab7\server.c -o CMakeFiles\lab7.dir\server.c.s

# Object files for target lab7
lab7_OBJECTS = \
"CMakeFiles/lab7.dir/client.c.obj" \
"CMakeFiles/lab7.dir/server.c.obj"

# External object files for target lab7
lab7_EXTERNAL_OBJECTS =

lab7.exe: CMakeFiles/lab7.dir/client.c.obj
lab7.exe: CMakeFiles/lab7.dir/server.c.obj
lab7.exe: CMakeFiles/lab7.dir/build.make
lab7.exe: CMakeFiles/lab7.dir/linklibs.rsp
lab7.exe: CMakeFiles/lab7.dir/objects1.rsp
lab7.exe: CMakeFiles/lab7.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable lab7.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\lab7.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lab7.dir/build: lab7.exe

.PHONY : CMakeFiles/lab7.dir/build

CMakeFiles/lab7.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\lab7.dir\cmake_clean.cmake
.PHONY : CMakeFiles/lab7.dir/clean

CMakeFiles/lab7.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\SubjectsIn5Sem\ComputerNetworks\lab7 E:\SubjectsIn5Sem\ComputerNetworks\lab7 E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles\lab7.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lab7.dir/depend

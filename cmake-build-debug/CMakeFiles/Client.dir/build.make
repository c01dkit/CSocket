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
include CMakeFiles/Client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Client.dir/flags.make

CMakeFiles/Client.dir/client.c.obj: CMakeFiles/Client.dir/flags.make
CMakeFiles/Client.dir/client.c.obj: ../client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Client.dir/client.c.obj"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\Client.dir\client.c.obj   -c E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c

CMakeFiles/Client.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Client.dir/client.c.i"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c > CMakeFiles\Client.dir\client.c.i

CMakeFiles/Client.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Client.dir/client.c.s"
	C:\PROGRA~1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\SubjectsIn5Sem\ComputerNetworks\lab7\client.c -o CMakeFiles\Client.dir\client.c.s

# Object files for target Client
Client_OBJECTS = \
"CMakeFiles/Client.dir/client.c.obj"

# External object files for target Client
Client_EXTERNAL_OBJECTS =

Client.exe: CMakeFiles/Client.dir/client.c.obj
Client.exe: CMakeFiles/Client.dir/build.make
Client.exe: CMakeFiles/Client.dir/linklibs.rsp
Client.exe: CMakeFiles/Client.dir/objects1.rsp
Client.exe: CMakeFiles/Client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Client.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Client.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Client.dir/build: Client.exe

.PHONY : CMakeFiles/Client.dir/build

CMakeFiles/Client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Client.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Client.dir/clean

CMakeFiles/Client.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\SubjectsIn5Sem\ComputerNetworks\lab7 E:\SubjectsIn5Sem\ComputerNetworks\lab7 E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug E:\SubjectsIn5Sem\ComputerNetworks\lab7\cmake-build-debug\CMakeFiles\Client.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Client.dir/depend


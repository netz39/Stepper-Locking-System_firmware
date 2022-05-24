cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)
#TODO Add your projects name here. Replace myProject with it
#TODO migration from makefile? -> use content of TARGET
project(myProject LANGUAGES CXX C ASM)

# boilerplate cmake for setting up compiler flags, extraction from cubemx code
# internal macros and such
#TODO check paths to each of those files
include("libraries/core/cmake/setupEmbeddedBuild.cmake")
include("libraries/core/cmake/setupEmbeddedExtraFiles.cmake")
include("libraries/core/cmake/generateCubeMXTargets.cmake")

#TODO change HalDir to your .cubemx folder
set(HalDir "myproject.cubemx")

# sets up compiler flags, you may change the c++ and c version here
#TODO in order for CMake to be able to read the cubemx makefile it
#TODO needs to be 'instrumented' with a bit of helper code.
#TODO cubemx will not overwrite these changes on regenerate.
#TODO Open the *HalDir*/Makefile and add the following line just above the
#TODO *** EOF *** marker
#TODO  include ../core/mk/CubeMXMakefileInstrumentation.mk
#TODO check that the path to core/ is correct from where the Makefile is
#TODO situated
SETUP_BUILD(${HalDir} ${PROJECT_NAME} c11 c++17)

# generates cmake libraries for hal and freertos
GENERATE_CUBEMX_TARGETS(${HalDir} true)

# instructs core to set all internal macros for embedded configuration
# Affects if arm specific code is compiled in which would SEGFAULT on non arm targets
set(core_BUILDCONFIGURATION "embedded")

# Adds the core library
#TODO Check path to core library
add_subdirectory(core)

# TODO setup variables for uavcan. Remove all bus_node_base stuff if not required
# TODO when you are migrating from Makefile you can copy the values over:

# TODO migration from makefile?
# TODO bus_node_base_UAVCAN_MemPool_Size is at the same default value as the old buildsystem's. If changed use value of UAVCAN_MEMPOOL_SIZE
# TODO bus_node_base_UAVCAN_RX_Queue_Size same as mempool. If changed use value of UAVCAN_RX_QUEUE_SIZE
# TODO bus_node_base_STM32_UAVCAN_Timer_Number is UAVCAN_STM32_TIMER_NUMBER
# TODO bus_node_base_STM32_UAVCAN_PCLK1 is STM32_PCLK1
# TODO bus_node_base_STM32_UAVCAN_TIMCLK1 is STM32_TIMCLK1
set(bus_node_base_UAVCAN_MemPool_Size 16384)
set(bus_node_base_UAVCAN_RX_Queue_Size 128)
set(bus_node_base_STM32_UAVCAN_Timer_Number 7)
set(bus_node_base_STM32_UAVCAN_PCLK1 42000000)
set(bus_node_base_STM32_UAVCAN_TIMCLK1 84000000)
# TODO Check path to bus-node-base library
add_subdirectory(bus-node-base)

# TODO add additional libraries if necessary
# add_subdirectory(path/to/myLibrary)

# TODO add include folders of your firmware
# TODO do not add your libraries' include folders here as it is done automatically
# TODO further down below the file
set(INCLUDE_FILES
        src)

# TODO add sources of your firmware
# Lookup the file(GLOB_RECURSE ...) function if you don't want to explicitly name
# every file but rahter include everyhting recursively from a path
set(Sources
        src/myFile.cpp)

# Instructs CMake to build your firmware
add_executable(${PROJECT_NAME} ${Sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_FILES})

# TODO if your firmware has any defines that are specific to it and not
# TODO to a sensor's library add them here. Adding libraries is discussed in the
# TODO 'Custom Libraries' guide.
#target_compile_definitions(${PROJECT_NAME} PUBLIC
#        MyDefine
#        MyDefine2=MyValue)

# TODO add your libraries
# Every call to add_subdirectory() will load the libraries' CMakeLists.txt file. Those will contain one or more
# static / object libraries whose names you have to add here. target_link_libraries will not only see that the
# libraries' code is added to your firmware but also sets up include directories. So you don't have to add them manually.
# Read libraries' README files for information about specifics.
target_link_libraries(${PROJECT_NAME} PRIVATE
        # Core's libraries
        hal
        core
        freertos

        # bus-node-base's libraries
        # Will contain uavcan, stm32 uavcan driver, bus-node-base support code
        bus_node_base_stm32)

# Sets up generation of .list and .bin files after your firmware has generated.
# Makes firmware image generate as an .elf file.
# Also performs checks for some common bugs that can happen in buildsystem configuration
SETUP_EXTRA_FILE_GENERATION(${PROJECT_NAME})
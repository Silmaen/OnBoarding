cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(CMAKE_CXX_STANDARD 17)

########################################################################################################################
########################################################################################################################
########################################################################################################################
set(PROJECT_NAME OnBoarding)

set(${PROJECT_NAME}_BOARD micro)
# setTime(ARDUINO_CPU)
project(${PROJECT_NAME})

file(GLOB
    LIBSRCS
    *.cpp
    )

file(GLOB
    LIBHDRS
    *.h
    )
# Define additional source and header files or default arduino sketch files
set(${PROJECT_NAME}_SRCS ${LIBSRCS})
set(${PROJECT_NAME}_HDRS ${LIBHDRS})

### Additional static libraries to include in the target.
set(${PROJECT_NAME}_ARDLIBS Wire SPI)
set(${PROJECT_NAME}_LIBS )

### Main sketch file
#set(${PROJECT_NAME}_SKETCH OnBoarding.ino)

### Add project directories into the build
# add_subdirectory()

### Additional settings to add non-standard or your own Arduino libraries.
# For this example (libs will contain additional arduino libraries)
# An Arduino library my_lib will contain files in libs/my_lib/: my_lib.h, my_lib.cpp + any other cpp files
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# For nested library sources replace ${LIB_NAME} with library name for each library
#set(_RECURSE true)

#### Additional settings for programmer. From programmers.txt
set(${PROJECT_NAME}_PROGRAMMER avrispmkii)
set(${PROJECT_NAME}_PORT COM14)
#setTime(micro.upload.speed 115200)

## Verbose build process
set(${PROJECT_NAME}_AFLAGS -v)

generate_arduino_firmware(${PROJECT_NAME})

########################################################################################################################
########################################################################################################################
########################################################################################################################

set(RECEIVER_PROJECT_NAME OnBoardingReceiver)

set(${RECEIVER_PROJECT_NAME}_BOARD micro)
# setTime(ARDUINO_CPU)
project(${RECEIVER_PROJECT_NAME})

# Define additional source and header files or default arduino sketch files
# set(${RECEIVER_PROJECT_NAME}_SRCS ${LIBSRCS})
# set(${RECEIVER_PROJECT_NAME}_HDRS ${LIBHDRS})

### Additional static libraries to include in the target.
set(${RECEIVER_PROJECT_NAME}_ARDLIBS SPI RF24)
# set(${RECEIVER_PROJECT_NAME}_LIBS )


### Main sketch file
set(${RECEIVER_PROJECT_NAME}_SKETCH Receiver/receiver.ino)

### Add project directories into the build
# add_subdirectory()

### Additional settings to add non-standard or your own Arduino libraries.
# For this example (libs will contain additional arduino libraries)
# An Arduino library my_lib will contain files in libs/my_lib/: my_lib.h, my_lib.cpp + any other cpp files
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# For nested library sources replace ${LIB_NAME} with library name for each library
# set(_RECURSE true)

#### Additional settings for programmer. From programmers.txt
set(${RECEIVER_PROJECT_NAME}_PROGRAMMER avrispmkii)
set(${RECEIVER_PROJECT_NAME}_PORT COM15)
#setTime(micro.upload.speed 115200)

## Verbose build process
set(${RECEIVER_PROJECT_NAME}_AFLAGS -v)

generate_arduino_firmware(${RECEIVER_PROJECT_NAME})


########################################################################################################################
########################################################################################################################
########################################################################################################################
set(CMAKE_VERBOSE_MAKEFILE TRUE)
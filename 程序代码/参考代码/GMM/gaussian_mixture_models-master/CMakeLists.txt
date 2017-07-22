#########################################################
#                                                       #
#    gaussian_mixture_models                            #
#    Roberto Capobianco  <capobianco@dis.uniroma1.it>   #
#                                                       #
#########################################################


################## Header  definitions ##################

cmake_minimum_required(VERSION 2.7)
project(gaussian_mixture_models)

# Set the default path for built executables to the "bin" directory.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# Set the default path for built libraries to the "lib" directory.
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#########################################################

############### Configuration  Generation ###############

find_package(Eigen3 REQUIRED)
find_package(OpenMP)

# Build type configuration.
if (CMAKE_BUILD_TYPE)
  string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type)
  set(CMAKE_BUILD_TYPE ${_build_type} CACHE STRING "" FORCE)
endif (CMAKE_BUILD_TYPE)

# Build type must be either DEBUG or RELEASE.
if (CMAKE_BUILD_TYPE STREQUAL "")
  message("No build type specified: forcing to RELEASE.")
  set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "" FORCE)
endif (CMAKE_BUILD_TYPE STREQUAL "")

# Linux bitness type.
if (LINUX_BITNESS)
  string(TOUPPER ${LINUX_BITNESS} _linux_bitness)
  set(LINUX_BITNESS ${_linux_bitness} CACHE STRING "" FORCE)
else (LINUX_BITNESS)
  set(LINUX_BITNESS "" CACHE STRING "" FORCE)
endif (LINUX_BITNESS)

# Linux bitness type must be either 32-BIT or 64-BIT.
if (LINUX_BITNESS STREQUAL "")
  EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
  
  if (${ARCHITECTURE} STREQUAL "x86_64")
    message("No linux bitness type specified. Detected 64-bit architecture, forcing to 64-bit bitness compilation.")
    set(LINUX_BITNESS "64-BIT" CACHE STRING "" FORCE)
  elseif (${ARCHITECTURE} STREQUAL "i386")
    message("No linux bitness type specified. Detected 32-bit architecture, forcing to 32-bit bitness compilation.")
    set(LINUX_BITNESS "32-BIT" CACHE STRING "" FORCE)
  else (${ARCHITECTURE} STREQUAL "i386")
    message(FATAL_ERROR "Unknown architecture found. The library cannot be compiled under this architecture yet.")
  endif (${ARCHITECTURE} STREQUAL "x86_64")
endif (LINUX_BITNESS STREQUAL "")

#########################################################

##################### Flag  Setting #####################

set(GMMs_CXX_FLAGS "")

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  set(GMMs_CXX_FLAGS "${GMMs_CXX_FLAGS} -ggdb -frounding-math")
elseif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  set(GMMs_CXX_FLAGS "${GMMs_CXX_FLAGS} -O3")
else (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  message(FATAL_ERROR "Invalid build type specified: doing nothing.")
endif (CMAKE_BUILD_TYPE STREQUAL "DEBUG")

if (LINUX_BITNESS STREQUAL "32-BIT")
  set(GMMs_CXX_FLAGS "${GMMs_CXX_FLAGS} -m32")
elseif (LINUX_BITNESS STREQUAL "64-BIT")
  set(GMMs_CXX_FLAGS "${GMMs_CXX_FLAGS} -m64")
else (LINUX_BITNESS STREQUAL "32-BIT")
  message(FATAL_ERROR "Invalid linux bitness type specified.")
endif (LINUX_BITNESS STREQUAL "32-BIT")

set(GMMs_CXX_FLAGS "${GMMs_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${GMMs_CXX_FLAGS}")

if(OPENMP_FOUND)
  message("OpenMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#########################################################

#################### Code  Compiling ####################

file(GLOB_RECURSE GMMs_src "src/gmms/*.cpp")

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src/gmms)

add_library(gmms SHARED ${GMMs_src})
target_link_libraries(gmms)

add_executable(k_means_test src/test/k_means_test.cpp)
target_link_libraries(k_means_test gmms)

add_executable(pca_test src/test/pca_test.cpp)
target_link_libraries(pca_test gmms)

add_executable(gmm_test src/test/gmm_test.cpp)
target_link_libraries(gmm_test gmms)

add_executable(gmm_classifier_test src/test/gmm_classifier_test.cpp)
target_link_libraries(gmm_classifier_test gmms)

add_executable(gmm_regressor_test src/test/gmm_regressor_test.cpp)
target_link_libraries(gmm_regressor_test gmms)

#########################################################

################# Software Installation #################

set(CMAKE_INSTALL_PREFIX /usr/local)

# Headers.
install(DIRECTORY src/gmms/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gaussian_mixture_models FILES_MATCHING PATTERN "*.h")

# Shared objects.
install(DIRECTORY lib/ LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gaussian_mixture_models FILES_MATCHING PATTERN "*.so")

# Binaries.
#install(TARGETS bin/GMMsearning RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/gaussian_mixture_models)


# - Try to find gmms
# Once done this will define
#  GMMS_FOUND - System has gmms
#  GMMS_INCLUDE_DIRS - The gmms include directories
#  GMMS_LIBRARIES - The libraries needed to use gmms

find_path(GMMS_INCLUDE_DIR gaussian_mixture_models
          HINTS
	  /usr/local/include
	  /usr/include
	  /opt/local/include
	  /sw/local/include
	  /sw/include
          PATH_SUFFIXES gmms)

find_library(GMMS_LIBRARY NAMES gmms
             HINTS
	     ~/Library/Frameworks
	     /Library/Frameworks
	     /usr/local/lib
	     /usr/local/lib64
	     /usr/lib
	     /usr/lib64
	     /opt/local/lib
	     /sw/local/lib
	     /sw/lib
	     PATH_SUFFIXES gaussian_mixture_models)

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set GMMS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(gmms DEFAULT_MSG
                                  GMMS_LIBRARY GMMS_INCLUDE_DIR)

mark_as_advanced(GMMS_INCLUDE_DIR GMMS_LIBRARY)

set(GMMS_LIBRARIES ${GMMS_LIBRARY})
set(GMMS_INCLUDE_DIRS ${GMMS_INCLUDE_DIR})
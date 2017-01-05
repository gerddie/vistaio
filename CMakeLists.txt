#
#  Copyright 1993, 1994 University of British Columbia
#  Copyright (c) Leipzig, Madrid 2004 - 2014
#  Max-Planck-Institute for Human Cognitive and Brain Science	
#  Max-Planck-Institute for Evolutionary Anthropology 
#  BIT, ETSI Telecomunicacion, UPM
#
#  Permission to use, copy, modify, distribute, and sell this software and its
#  documentation for any purpose is hereby granted without fee, provided that
#  the above copyright notice appears in all copies and that both that
#  copyright notice and this permission notice appear in supporting
#  documentation. UBC makes no representations about the suitability of this
#  software for any purpose. It is provided "as is" without express or
#  implied warranty.
#
#  Maintainance and Fixes: Gert Wollny gw.fossdev@gmail.com , UPM, Madrid  
#

cmake_minimum_required(VERSION 2.6)
ENABLE_TESTING()

INCLUDE (CheckIncludeFiles) 

SET(VENDOR "Gert Wollny")
SET(PACKAGE_NAME "vistaio")
SET(MAJOR_VERSION 2)
SET(MINOR_VERSION 0)
SET(MICRO_VERSION 0)

INCLUDE(GNUInstallDirs)

SET(VISTAIO_CURRENT 15)
SET(VISTAIO_AGE 0)
SET(VISTAIO_REVISION 0)
  
SET(LIBRARY_VERSION_INFO "${VISTAIO_CURRENT}.${VISTAIO_AGE}.${VISTAIO_REVISION}")

SET(PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")
SET(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")
SET(VISTAIO_NAME "${PACKAGE_NAME}-${VERSION}")
SET(VISTAIO_PACKAGE "${PACKAGE_NAME}-${PACKAGE_VERSION}")

OPTION(ENABLE_COVERAGE "Enable code coverage tests" FALSE)



IF (ENABLE_COVERAGE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
  set(ENABLE_LARGEFILE_TEST_DEFAULT ON)
ELSE()
  set(ENABLE_LARGEFILE_TEST_DEFAULT OFF)
ENDIF()

OPTION(ENABLE_LARGEFILE_TEST "Enable test(s) for IO of data sets larger then 2GB" ${ENABLE_LARGEFILE_TEST_DEFAULT})

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

ADD_DEFINITIONS(-D_DEFAULT_SOURCE=2)

ADD_SUBDIRECTORY(vistaio)
ADD_SUBDIRECTORY(doc)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libvistaio.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libvistaio.pc)

SET(DOC_FILES Authors COPYING README.md)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libvistaio.pc DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
INSTALL(FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${VISTAIO_NAME})
INSTALL(FILES vistaio.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${VISTAIO_NAME})

ADD_CUSTOM_TARGET(dist bash ${CMAKE_SOURCE_DIR}/create_package "${PACKAGE_NAME}" "${PACKAGE_VERSION}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

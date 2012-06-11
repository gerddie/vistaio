#
#  Copyright 1993, 1994 University of British Columbia
#  Copyright (c) Leipzig, Madrid 2004 - 2012
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
SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 2)
SET(MICRO_VERSION 10)
SET(INTERFACE_AGE 1)
SET(BINARY_AGE    1)


MATH(EXPR VISTAIO_CURRENT "${MICRO_VERSION} - ${INTERFACE_AGE}")
MATH(EXPR VISTAIO_AGE     "${BINARY_AGE} - ${INTERFACE_AGE}")

SET(LIBRARY_VERSION_INFO "${VISTAIO_CURRENT}.${INTERFACE_AGE}.${VISTAIO_AGE}")

SET(PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")
SET(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")
SET(VISTAIO_NAME "${PACKAGE_NAME}-${VERSION}")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

ADD_DEFINITIONS(-D_SVID_SOURCE=2)

ADD_SUBDIRECTORY(vistaio)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libvistaio.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libvistaio.pc)

SET(DOC_FILES Authors COPYING)

INSTALL(FILES libvistaio.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${VISTAIO_NAME})
INSTALL(FILES vistaio.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${VISTAIO_NAME})

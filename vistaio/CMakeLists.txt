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

set(VISTAIO_SRC 
  alloc.c
  attr.c
  dictionary.c
  edges.c
  edgesType.c
  error.c
  fileio.c
  graphType.c
  image.c
  imageType.c
  list.c
  option.c
  packData.c
  type.c
  vcplist.c
  version.c
  vspline2d.c
  vfield2d.c
  vfield3d.c
  volumes.c
  volumesType.c
)

set(VISTAIO_HEADERS  
  vcplist.h
  vfield2d.h
  vspline2d.h
  vfield3d.h
  vista.h
  vistaio.h
)

add_library(vistaio SHARED ${VISTAIO_SRC})
SET_TARGET_PROPERTIES(vistaio PROPERTIES 
  VERSION ${LIBRARY_VERSION_INFO}
  SOVERSION ${VISTAIO_CURRENT}
)


MACRO(ADD_SIMPLE_TEST name)
  add_executable(test-${name} test_${name}.c)
  target_link_libraries(test-${name} vistaio)
  add_test(${name} test-${name})
ENDMACRO(ADD_SIMPLE_TEST)

ADD_SIMPLE_TEST(attributes)
ADD_SIMPLE_TEST(images)
ADD_SIMPLE_TEST(3dfield)
ADD_SIMPLE_TEST(2dfield)
ADD_SIMPLE_TEST(2dspline)

IF(ENABLE_LARGEFILE_TEST)
  ADD_SIMPLE_TEST(bigfile)
ENDIF()


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/vconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/vconfig.h)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/vconfig.h DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${VISTAIO_NAME}/vistaio)
INSTALL(FILES ${VISTAIO_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${VISTAIO_NAME}/vistaio)
INSTALL(TARGETS vistaio 
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})


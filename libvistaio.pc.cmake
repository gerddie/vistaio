prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@/bin
libdir=@CMAKE_INSTALL_PREFIX@/lib
includedir=@CMAKE_INSTALL_PREFIX@/include/@VISTAIO_NAME@

Name: vistaio
Description:  is a library for loading and storing vista data
Version: @PACKAGE_VERSION@
Requires:
Conflicts:

Libs: -L@CMAKE_INSTALL_PREFIX@/lib -lvistaio
Cflags: -I@CMAKE_INSTALL_PREFIX@/include/@VISTAIO_NAME@

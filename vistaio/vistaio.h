/*
 *  Copyright 1993, 1994 University of British Columbia
 *  Copyright (c) Leipzig, Madrid 2004 - 2012
 *  Max-Planck-Institute for Human Cognitive and Brain Science	
 *  Max-Planck-Institute for Evolutionary Anthropology 
 *  BIT, ETSI Telecomunicacion, UPM
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appears in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation. UBC makes no representations about the suitability of this
 *  software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author: Arthur Pope, UBC Laboratory for Computational Intelligence
            Marc Tittgemeyer, MPI for Human and Cognitive Brain Sciences
 *  Maintainance and Fixes: Gert Wollny, UPM 
 */

#ifndef __VISTAIO_H
#define __VISTAIO_H 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vistaio/vista.h> 
#include <vistaio/vcplist.h>
#include <vistaio/vfield3d.h>
#include <vistaio/vfield2d.h>
#include <vistaio/vconfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const unsigned int vistaio_major_version;
extern const unsigned int vistaio_minor_version;
extern const unsigned int vistaio_micro_version;
extern const unsigned int vistaio_interface_age;
extern const unsigned int vistaio_binary_age;

#define VISTAIO_CHECK_VERSION(major,minor,micro)    \
    (vistaio_major_version > (major) || \
    (vistaio_major_version == (major) && vistaio_minor_version > (minor)) ||   \
    (vistaio_major_version == (major) && vistaio_minor_version == (minor) && \
     vistaio_micro_version >= (micro)))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __VISTAIO_H */

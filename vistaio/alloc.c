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
 *  Maintainance and Fixes: Gert Wollny, UPM 
 */

#include "vistaio/vistaio.h"

/*! \brief Perform error checking on malloc() call.
 *
 *  \param  size
 *  \return VistaIOPointer
 */

VistaIOPointer VistaIOMalloc (int64_t size)
{
	VistaIOPointer p;

	if (size == 0)
		return NULL;
	p = (VistaIOPointer) malloc (size); 
	if (!p)
		VistaIOSystemError ("VistaIOMalloc: Memory allocation failure: requested %ld", size);
	return p;
}


/*! \brief Perform error checking on realloc() call.
 *
 *  \param  p
 *  \param  size
 *  \return VistaIOPointer
 */

VistaIOPointer VistaIORealloc (VistaIOPointer p, int64_t size)
{
	if (size == 0) {
		VistaIOFree (p);
		return NULL;
	}
	if (!p)
		return VistaIOMalloc (size);
	if (!(p = (VistaIOPointer) realloc (p, size)))
		VistaIOSystemError ("VistaIORealloc: Memory allocation failure");
	return p;
}


/*! \brief Perform error checking on calloc() call.
 *
 *  \param  n
 *  \param  size
 *  \return VistaIOPointer
 */

VistaIOPointer VistaIOCalloc (int64_t n, int64_t size)
{
	VistaIOPointer p;

	if (n == 0 || size == 0)
		return NULL;
	if (!(p = (VistaIOPointer) calloc (n, size)))
		VistaIOSystemError ("VistaIOCalloc: Memory allocation failure");
	return p;
}


/*! \brief Perform error checking on free() call.
 *
 *  \param p
 */

EXPORT_VISTA void VistaIOFree (VistaIOPointer p)
{
	if (p)
		free ((char *)p);
}

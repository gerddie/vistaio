/*
 *  Copyright (c) Leipzig, Madrid 2004 - 2012
 *  Max-Planck-Institute for Human Cognitive and Brain Science	
 *  Max-Planck-Institute for Evolutionary Anthropology 
 *  BIT, ETSI Telecomunicacion, UPM
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appears in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation. The authors makes no representations about the suitability 
 *  of this software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author: Gert Wollny <gw.fossdev@gmail.com>
 */

#ifndef __VSpline2D_h
#define __VSpline2D_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vistaio/vfield2d.h>


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VistaIOSpline2DRec {
		VistaIOFloat x_range; 
		VistaIOFloat y_range; 
		VistaIOString kernel_descr; 
		VistaIOAttrList attr; 
		VistaIOField2D data; 
	} VistaIOSpline2DRec, *VistaIOSpline2D;
	
	EXPORT_VISTA VistaIOSpline2D VistaIOCreateSpline2DFrom(
		VistaIOLong _x_dim, 
		VistaIOLong _y_dim, 
		float _x_range, 
		float _y_range, 
		VistaIOString kernel_descr,
		VistaIOLong nsize_elemet,
		VistaIORepnKind repn,
		VistaIOPointerConst data);
	
	EXPORT_VISTA VistaIOSpline2D VistaIOCreateSpline2D(VistaIOLong _x_dim, 
					       VistaIOLong _y_dim, 
					       float _x_range, 
					       float _y_range, 
					       VistaIOString kernel_descr,
					       VistaIOLong nsize_elemet,
					       VistaIORepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VistaIODestroySpline2D (VistaIOSpline2D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VistaIOSpline2D VistaIOCopySpline2D (VistaIOSpline2D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VistaIOSpline2D VistaIOMirrorSpline2D(VistaIOSpline2D src);
	
#define VistaIOSpline2DElement(FIELD, X, Y, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * Y )]
	
	EXPORT_VISTA char *VistaIOGetSpline2DListName(VistaIOSpline2D field);

	extern VistaIOTypeMethods VistaIOSpline2DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


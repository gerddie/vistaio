/*
 *  Copyright (c) Leipzig, Madrid 2004 - 2009
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
 *  Author:  Gert Wollny, UPM 
 */

#ifndef __VField2D_h
#define __VField2D_h

#include "vista.h"


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VistaIOField2DRec {
		VistaIOLong x_dim;
		VistaIOLong y_dim;
		VistaIORepnKind repn;
		VistaIOLong nsize_element;
		VistaIOAttrList attr; 
                union {
                        VistaIOPointer data;
                        VistaIOPointerConst cdata;
                } p;
		VistaIOBoolean owns_data; 
		VistaIOLong nsize; 
	} VistaIOField2DRec, *VistaIOField2D;
	
	EXPORT_VISTA VistaIOField2D VistaIOCreateField2DFrom(VistaIOLong _x_dim, 
				VistaIOLong _y_dim, 
				VistaIOLong nsize_elemet,
				VistaIORepnKind repn,
				VistaIOPointerConst data);
	
	EXPORT_VISTA VistaIOField2D VistaIOCreateField2D(VistaIOLong _x_dim, 
				VistaIOLong _y_dim, 
				VistaIOLong nsize_elemet,
				VistaIORepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VistaIODestroyField2D (VistaIOField2D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VistaIOField2D VistaIOCopyField2D (VistaIOField2D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VistaIOField2D VistaIOMirrorField2D(VistaIOField2D src);
	
	
#define VistaIOField2DElement(FIELD, X, Y, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * Y )]
	
	EXPORT_VISTA char *VistaIOGetField2DListName(VistaIOField2D field);	

extern VistaIOTypeMethods VistaIOField2DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


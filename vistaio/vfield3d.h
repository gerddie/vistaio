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
 *  documentation. UBC makes no representations about the suitability of this
 *  software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author: Gert Wollny, UPM 
 */

#ifndef __VField3D_h
#define __VField3D_h

#include "vista.h"


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VistaIOField3DRec {
		VistaIOLong x_dim;
		VistaIOLong y_dim;
		VistaIOLong z_dim; 
		VistaIORepnKind repn;
		VistaIOLong nsize_element;
		VistaIOAttrList attr; 
                union {
                        VistaIOPointer data;
                        VistaIOPointerConst cdata;
                } p; 
		VistaIOBoolean owns_data; 
		VistaIOLong nsize; 
	} VistaIOField3DRec, *VistaIOField3D;
	
	EXPORT_VISTA VistaIOField3D VistaIOCreateField3DFrom(VistaIOLong _x_dim, 
				VistaIOLong _y_dim, 
				VistaIOLong _z_dim, 
				VistaIOLong nsize_elemet,
				VistaIORepnKind repn,
				VistaIOPointerConst data);
	
	EXPORT_VISTA VistaIOField3D VistaIOCreateField3D(VistaIOLong _x_dim, 
				VistaIOLong _y_dim, 
				VistaIOLong _z_dim, 
				VistaIOLong nsize_elemet,
				VistaIORepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VistaIODestroyField3D (VistaIOField3D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VistaIOField3D VistaIOCopyField3D (VistaIOField3D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VistaIOField3D VistaIOMirrorField3D(VistaIOField3D src);
	
	
#define VistaIOField3DElement(FIELD, X, Y, Z, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * ( Y + ny * Z))]
	
	EXPORT_VISTA char *VistaIOGetField3DListName(VistaIOField3D field);	

extern VistaIOTypeMethods VistaIOField3DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


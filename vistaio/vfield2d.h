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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vista.h"


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VField2DRec {
		VLong x_dim;
		VLong y_dim;
		VRepnKind repn;
		VLong nsize_element;
		VAttrList attr; 
                union {
                        VPointer data;
                        VPointerConst cdata;
                } p;
		VBoolean owns_data; 
		VLong nsize; 
	} VField2DRec, *VField2D;
	
	EXPORT_VISTA VField2D VCreateField2DFrom(VLong _x_dim, 
				VLong _y_dim, 
				VLong nsize_elemet,
				VRepnKind repn,
				VPointerConst data);
	
	EXPORT_VISTA VField2D VCreateField2D(VLong _x_dim, 
				VLong _y_dim, 
				VLong nsize_elemet,
				VRepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VDestroyField2D (VField2D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VField2D VCopyField2D (VField2D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VField2D VMirrorField2D(VField2D src);
	
	
#define VField2DElement(FIELD, X, Y, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * Y )]
	
	EXPORT_VISTA char *VGetField2DListName(VField2D field);	

extern VTypeMethods VField2DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


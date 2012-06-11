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
 *  Author: Gert Wollny, UPM 
 */

#ifndef __VField3D_h
#define __VField3D_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vista.h"


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VField3DRec {
		VLong x_dim;
		VLong y_dim;
		VLong z_dim; 
		VRepnKind repn;
		VLong nsize_element;
		VAttrList attr; 
                union {
                        VPointer data;
                        VPointerConst cdata;
                } p; 
		VBoolean owns_data; 
		VLong nsize; 
	} VField3DRec, *VField3D;
	
	EXPORT_VISTA VField3D VCreateField3DFrom(VLong _x_dim, 
				VLong _y_dim, 
				VLong _z_dim, 
				VLong nsize_elemet,
				VRepnKind repn,
				VPointerConst data);
	
	EXPORT_VISTA VField3D VCreateField3D(VLong _x_dim, 
				VLong _y_dim, 
				VLong _z_dim, 
				VLong nsize_elemet,
				VRepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VDestroyField3D (VField3D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VField3D VCopyField3D (VField3D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VField3D VMirrorField3D(VField3D src);
	
	
#define VField3DElement(FIELD, X, Y, Z, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * ( Y + ny * Z))]
	
	EXPORT_VISTA char *VGetField3DListName(VField3D field);	

extern VTypeMethods VField3DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


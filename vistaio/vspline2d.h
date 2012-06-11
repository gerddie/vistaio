/*
 *  Copyright (c) Leipzig, Madrid 2004 - 2009
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

#ifndef __VSpline2D_h
#define __VSpline2D_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vistaio/vfield2d.h>


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct VSpline2DRec {
		VFloat x_range; 
		VFloat y_range; 
		VString kernel_descr; 
		VAttrList attr; 
		VField2D data; 
	} VSpline2DRec, *VSpline2D;
	
	EXPORT_VISTA VSpline2D VCreateSpline2DFrom(
		VLong _x_dim, 
		VLong _y_dim, 
		float _x_range, 
		float _y_range, 
		VString kernel_descr,
		VLong nsize_elemet,
		VRepnKind repn,
		VPointerConst data);
	
	EXPORT_VISTA VSpline2D VCreateSpline2D(VLong _x_dim, 
					       VLong _y_dim, 
					       float _x_range, 
					       float _y_range, 
					       VString kernel_descr,
					       VLong nsize_elemet,
					       VRepnKind repn);
	
	/* destroy the field and release all data (if owned) */
	EXPORT_VISTA void VDestroySpline2D (VSpline2D field);
	
	/* copy the data field with all data */ 
	EXPORT_VISTA VSpline2D VCopySpline2D (VSpline2D src);
	
	/* mirrors the datafield without copying the real data*/
	EXPORT_VISTA VSpline2D VMirrorSpline2D(VSpline2D src);
	
#define VSpline2DElement(FIELD, X, Y, type) &((type*)FIELD->data)[field->nsize_element * (x + nx * Y )]
	
	EXPORT_VISTA char *VGetSpline2DListName(VSpline2D field);

	extern VTypeMethods VSpline2DMethods; 
	
#ifdef __cplusplus
}
#endif
			   
#endif


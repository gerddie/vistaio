/*
 *  Copyright 1993, 1994 University of British Columbia
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <vistaio/vspline2d.h>
#include <assert.h>

static VPointer VSpline2DDecodeMethod (VStringConst name, VBundle b); 
static VAttrList VSpline2DEncodeAttrMethod (VPointer value, size_t *lengthp); 
static VPointer VSpline2DEncodeDataMethod (VPointer value, VAttrList list,
					  size_t length, VBoolean *free_itp); 

VTypeMethods VSpline2DMethods = {
	(VCopyMethod*)VCopySpline2D,		 /* copy a VSpline2D */
	(VDestroyMethod*)VDestroySpline2D,	 /* destroy a VSpline2D */
	VSpline2DDecodeMethod,	                 /* decode a VSpline2D's value */
	VSpline2DEncodeAttrMethod,	         /* encode a VSpline2D's attr list */
	VSpline2DEncodeDataMethod	         /* encode a VSpline2D's binary data */
};


EXPORT_VISTA VSpline2D VCreateSpline2DFrom(
	VLong _x_dim, 
	VLong _y_dim, 
	float _x_range, 
	float _y_range, 
	VString kernel_descr,
	VLong nsize_element,
	VRepnKind repn,
	VPointerConst data)
{
	VSpline2D result = (VSpline2D)malloc(sizeof(VSpline2DRec)); 
	result->data = VCreateField2DFrom(_x_dim, _y_dim, nsize_element, repn, data); 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

static VSpline2D VCreateSpline2DFromField(
	float _x_range, 
	float _y_range, 
	VString kernel_descr,
	VAttrList attr, 
	VField2D data)
{
	VSpline2D result = (VSpline2D)malloc(sizeof(VSpline2DRec)); 
	result->attr = attr; 
	result->data = data; 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

EXPORT_VISTA VSpline2D VCreateSpline2D(VLong _x_dim, 
				       VLong _y_dim, 
				       float _x_range, 
				       float _y_range, 
				       VString kernel_descr,
				       VLong nsize_element,
				       VRepnKind repn)
{
	VSpline2D result = (VSpline2D)malloc(sizeof(VSpline2DRec)); 
	result->data = VCreateField2D(_x_dim, _y_dim, nsize_element, repn); 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

/* destroy the field and release all data (if owned) */
EXPORT_VISTA void VDestroySpline2D (VSpline2D spline)
{
	VDestroyField2D(spline->data); 
	if (spline->attr)
		VDestroyAttrList(spline->attr);
	free(spline);
}

/* copy the data field with all data */ 
EXPORT_VISTA VSpline2D VCopySpline2D (VSpline2D src)
{
	VSpline2D result = (VSpline2D)malloc(sizeof(VSpline2DRec)); 
	result->x_range = src->x_range; 
	result->y_range = src->y_range; 
	result->kernel_descr = src->kernel_descr; 
	result->attr = VCopyAttrList(src->attr);
	result->data = VCopyField2D(src->data); 
	return result; 
}

	
/* mirrors the datafield without copying the real data*/
EXPORT_VISTA VSpline2D VMirrorSpline2D(VSpline2D src)
{
	VSpline2D result = (VSpline2D)malloc(sizeof(VSpline2DRec)); 
	result->x_range = src->x_range; 
	result->y_range = src->y_range; 
	result->kernel_descr = src->kernel_descr; 
	result->attr = VCopyAttrList(src->attr);
	result->data = VMirrorField2D(src->data); 
	return result; 
}

EXPORT_VISTA char *VGetSpline2DListName(VSpline2D field)
{
	return "spline2d"; 
}

static VAttrList VSpline2DEncodeAttrMethod (VPointer value, size_t *lengthp)
{
	VSpline2D spline = value;
	VAttrList list;
	
	/* Temporarily prepend several attributes to the spline attr list: */
	if ((list = spline->attr) == NULL)
		list = spline->attr = VCreateAttrList ();
	
	VPrependAttr (list, "y_range", NULL, VFloatRepn,(VFloat)spline->y_range);
	VPrependAttr (list, "x_range", NULL, VFloatRepn,(VFloat)spline->x_range);
	VPrependAttr (list, "kernel", NULL, VStringRepn,spline->kernel_descr);
	VPrependAttr (list, "coefficients", NULL, VField2DRepn, spline->data);
	
	return list;
}


static VPointer VSpline2DDecodeMethod (VStringConst name, VBundle b)
{
	VSpline2D spline;
	VField2D  data; 
	VFloat    x_range; 
	VFloat    y_range; 
	VString   kernel_descr; 
	

	if (!VExtractAttr (b->list,"x_range",NULL, VFloatRepn, &x_range, TRUE)) return NULL;
	if (!VExtractAttr (b->list,"y_range",NULL, VFloatRepn, &y_range, TRUE)) return NULL;
	if (!VExtractAttr (b->list,"kernel",NULL, VStringRepn, &kernel_descr, TRUE)) return NULL;
	if (!VExtractAttr (b->list,"coefficients",NULL, VField2DRepn, &data, TRUE)) return NULL;

	spline =  VCreateSpline2DFromField(x_range, y_range, kernel_descr, b->list, data);
	if (!spline) 
		return NULL; 
	return spline;  
}
static VPointer VSpline2DEncodeDataMethod (VPointer value, VAttrList list,
					size_t length, VBoolean *free_itp)
{
	return NULL; 
}



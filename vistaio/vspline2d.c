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
 *  Author: Gert Wollny, UPM 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <vistaio/vspline2d.h>
#include <assert.h>

static VistaIOPointer VistaIOSpline2DDecodeMethod (VistaIOStringConst name, VistaIOBundle b); 
static VistaIOAttrList VistaIOSpline2DEncodeAttrMethod (VistaIOPointer value, size_t *lengthp); 
static VistaIOPointer VistaIOSpline2DEncodeDataMethod (VistaIOPointer value, VistaIOAttrList list,
					  size_t length, VistaIOBoolean *free_itp); 

VistaIOTypeMethods VistaIOSpline2DMethods = {
	(VistaIOCopyMethod*)VistaIOCopySpline2D,		 /* copy a VistaIOSpline2D */
	(VistaIODestroyMethod*)VistaIODestroySpline2D,	 /* destroy a VistaIOSpline2D */
	VistaIOSpline2DDecodeMethod,	                 /* decode a VistaIOSpline2D's value */
	VistaIOSpline2DEncodeAttrMethod,	         /* encode a VistaIOSpline2D's attr list */
	VistaIOSpline2DEncodeDataMethod	         /* encode a VistaIOSpline2D's binary data */
};


EXPORT_VISTA VistaIOSpline2D VistaIOCreateSpline2DFrom(
	VistaIOLong _x_dim, 
	VistaIOLong _y_dim, 
	float _x_range, 
	float _y_range, 
	VistaIOString kernel_descr,
	VistaIOLong nsize_element,
	VistaIORepnKind repn,
	VistaIOPointerConst data)
{
	VistaIOSpline2D result = (VistaIOSpline2D)malloc(sizeof(VistaIOSpline2DRec)); 
	result->data = VistaIOCreateField2DFrom(_x_dim, _y_dim, nsize_element, repn, data); 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

static VistaIOSpline2D VistaIOCreateSpline2DFromField(
	float _x_range, 
	float _y_range, 
	VistaIOString kernel_descr,
	VistaIOAttrList attr, 
	VistaIOField2D data)
{
	VistaIOSpline2D result = (VistaIOSpline2D)malloc(sizeof(VistaIOSpline2DRec)); 
	result->attr = attr; 
	result->data = data; 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

EXPORT_VISTA VistaIOSpline2D VistaIOCreateSpline2D(VistaIOLong _x_dim, 
				       VistaIOLong _y_dim, 
				       float _x_range, 
				       float _y_range, 
				       VistaIOString kernel_descr,
				       VistaIOLong nsize_element,
				       VistaIORepnKind repn)
{
	VistaIOSpline2D result = (VistaIOSpline2D)malloc(sizeof(VistaIOSpline2DRec)); 
	result->data = VistaIOCreateField2D(_x_dim, _y_dim, nsize_element, repn); 
	result->x_range = _x_range; 
	result->y_range = _y_range; 
	result->kernel_descr = kernel_descr; 
	return result; 
}

/* destroy the field and release all data (if owned) */
EXPORT_VISTA void VistaIODestroySpline2D (VistaIOSpline2D spline)
{
	VistaIODestroyField2D(spline->data); 
	if (spline->attr)
		VistaIODestroyAttrList(spline->attr);
	free(spline);
}

/* copy the data field with all data */ 
EXPORT_VISTA VistaIOSpline2D VistaIOCopySpline2D (VistaIOSpline2D src)
{
	VistaIOSpline2D result = (VistaIOSpline2D)malloc(sizeof(VistaIOSpline2DRec)); 
	result->x_range = src->x_range; 
	result->y_range = src->y_range; 
	result->kernel_descr = src->kernel_descr; 
	result->attr = VistaIOCopyAttrList(src->attr);
	result->data = VistaIOCopyField2D(src->data); 
	return result; 
}

	
/* mirrors the datafield without copying the real data*/
EXPORT_VISTA VistaIOSpline2D VistaIOMirrorSpline2D(VistaIOSpline2D src)
{
	VistaIOSpline2D result = (VistaIOSpline2D)malloc(sizeof(VistaIOSpline2DRec)); 
	result->x_range = src->x_range; 
	result->y_range = src->y_range; 
	result->kernel_descr = src->kernel_descr; 
	result->attr = VistaIOCopyAttrList(src->attr);
	result->data = VistaIOMirrorField2D(src->data); 
	return result; 
}

EXPORT_VISTA char *VistaIOGetSpline2DListName(VistaIOSpline2D UNUSED(field))
{
	return "spline2d"; 
}

static VistaIOAttrList VistaIOSpline2DEncodeAttrMethod (VistaIOPointer value, size_t *UNUSED(lengthp))
{
	VistaIOSpline2D spline = value;
	VistaIOAttrList list;
	
	/* Temporarily prepend several attributes to the spline attr list: */
	if ((list = spline->attr) == NULL)
		list = spline->attr = VistaIOCreateAttrList ();
	
	VistaIOPrependAttr (list, "y_range", NULL, VistaIOFloatRepn,(VistaIOFloat)spline->y_range);
	VistaIOPrependAttr (list, "x_range", NULL, VistaIOFloatRepn,(VistaIOFloat)spline->x_range);
	VistaIOPrependAttr (list, "kernel", NULL, VistaIOStringRepn,spline->kernel_descr);
	VistaIOPrependAttr (list, "coefficients", NULL, VistaIOField2DRepn, spline->data);
	
	return list;
}


static VistaIOPointer VistaIOSpline2DDecodeMethod (VistaIOStringConst UNUSED(name), VistaIOBundle b)
{
	VistaIOSpline2D spline;
	VistaIOField2D  data; 
	VistaIOFloat    x_range; 
	VistaIOFloat    y_range; 
	VistaIOString   kernel_descr; 
	

	if (!VistaIOExtractAttr (b->list,"x_range",NULL, VistaIOFloatRepn, &x_range, TRUE)) return NULL;
	if (!VistaIOExtractAttr (b->list,"y_range",NULL, VistaIOFloatRepn, &y_range, TRUE)) return NULL;
	if (!VistaIOExtractAttr (b->list,"kernel",NULL, VistaIOStringRepn, &kernel_descr, TRUE)) return NULL;
	if (!VistaIOExtractAttr (b->list,"coefficients",NULL, VistaIOField2DRepn, &data, TRUE)) return NULL;

	spline =  VistaIOCreateSpline2DFromField(x_range, y_range, kernel_descr, b->list, data);
	if (!spline) 
		return NULL; 
	return spline;  
}
static VistaIOPointer VistaIOSpline2DEncodeDataMethod (VistaIOPointer UNUSED(value), VistaIOAttrList UNUSED(list),
					   size_t UNUSED(length), VistaIOBoolean *UNUSED(free_itp))
{
	return NULL; 
}



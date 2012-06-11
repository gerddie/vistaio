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

#include <string.h>
#include <stdio.h>
#include <vistaio/vspline2d.h>



int nx = 10; 
int ny = 11; 

float sx = 80.6; 
float sy = 90.2; 


#define VTYPE double
#define TYPEREPN VDoubleRepn

VBoolean RepnFilter(VBundle UNUSED(bundle), VRepnKind repn) 
{
	return repn == VSpline2DRepn; 
}

VSpline2D VRead2DSpline(VAttrList list) 
{
	VSpline2D result = NULL; 
	
	if (!list) 
		return NULL; 
	
	if (!VExtractAttr(list, "Spline2D",NULL, VSpline2DRepn, &result,TRUE)) {
		VMessage("Spline2D element of type 2dfield found");
	}
	
	VDestroyAttrList(list);
	return result;	
}

VAttrList VWrite2DSpline(VSpline2D field)
{
	int retval; 
	VSpline2D help; 
	VAttrList list;
	
	retval = FALSE; 
	
	list = VCreateAttrList();
	if (list) {
		
		help = VMirrorSpline2D(field);
		
		VSetAttr(list,"Spline2D",NULL,VSpline2DRepn,help);
		
	}
	return list; 
}


int main(int UNUSED(argc), const char **UNUSED(args))
{
	VSpline2D spline; 
	VSpline2D spline2;
	VAttrListPosn pos; 
	int x,y,z; 
	VTYPE *h; 
	VTYPE *k; 
	float test_float,number; 

	
	spline2 = NULL; 
	test_float = 1.2; 
		/* Create the field */ 
	spline = VCreateSpline2D(nx,ny,sx, sy, "bspline4", 2, TYPEREPN);

	
	if (spline->x_range != sx)
		VWarning("x_range not set properly: expect %f, got %f", sx, spline->x_range);	

	if (spline->y_range != sy)
		VWarning("y_range not set properly: expect %f, got %f", sy, spline->y_range);	
	
	/* put in some data */
	h = spline->data->p.data;
	for (y = 0; y < ny; y++) 
		for (x = 0; x < nx; x++){
			
			*h++ = x; 
			*h++ = y + x;
		}

	if (!spline->attr) 
		spline->attr = VCreateAttrList(); 
	/* add some attribute the the field */
	VSetAttr(spline->attr,"test_attribute", NULL, VFloatRepn, test_float);
	
		
	/* Write out the data */
	VAttrList list = VWrite2DSpline(spline);
	
	/* Read field from file */     
	spline2 = VRead2DSpline(list);
	
	
	/* testing follows */ 
	VField2D field = spline->data; 
	VField2D field2 = spline2->data; 

	h = field->p.data;
	k = field2->p.data;
	z = field->nsize; 
	if (z != field2->nsize) {
		VError("Error rereading test file");
	}
	
	z /= sizeof(VTYPE); 
	
	while (z--) {
		if (*h != *k) {
			VWarning("spline en/decoding error: delta = %e",*h - *k);
		}
		h++; 
		k++; 
			
	}
	
	if (spline2->x_range != sx)
		VWarning("x_range not set properly: expect %f, got %f", sx, spline2->x_range);	

	if (spline2->y_range != sy)
		VWarning("y_range not set properly: expect %f, got %f", sy, spline2->y_range);	
	
	if (!VLookupAttr(spline2->attr,"test_attribute",&pos)) 
		VWarning("'test_attribute'  not found");	

	if (strcmp("bspline4", spline2->kernel_descr)) 
	    VWarning("kernel not set properly: expect 'bspline4', got %s", spline2->kernel_descr);
	
	VGetAttrValue(&pos, NULL, VFloatRepn, &number);

	if (number != test_float) {
		VError("numbers differ by %e",number - test_float);
	}
	VDestroySpline2D(spline);
	VDestroySpline2D(spline2);

	remove( "test.v" ); 
	return 0;
}

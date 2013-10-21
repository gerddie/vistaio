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
#define TYPEREPN VistaIODoubleRepn

VistaIOBoolean RepnFilter(VistaIOBundle UNUSED(bundle), VistaIORepnKind repn) 
{
	return repn == VistaIOSpline2DRepn; 
}

VistaIOSpline2D VistaIORead2DSpline(VistaIOAttrList list) 
{
	VistaIOSpline2D result = NULL; 
	
	if (!list) 
		return NULL; 
	
	if (!VistaIOExtractAttr(list, "Spline2D",NULL, VistaIOSpline2DRepn, &result,TRUE)) {
		VistaIOMessage("Spline2D element of type 2dfield found");
	}
	
	VistaIODestroyAttrList(list);
	return result;	
}

VistaIOAttrList VistaIOWrite2DSpline(VistaIOSpline2D field)
{
	VistaIOSpline2D help; 
	VistaIOAttrList list;
	
	list = VistaIOCreateAttrList();
	if (list) {
		
		help = VistaIOMirrorSpline2D(field);
		
		VistaIOSetAttr(list,"Spline2D",NULL,VistaIOSpline2DRepn,help);
		
	}
	return list; 
}


int main(int UNUSED(argc), const char **UNUSED(args))
{
	VistaIOSpline2D spline; 
	VistaIOSpline2D spline2;
	VistaIOAttrListPosn pos; 
	int x,y,z; 
	VTYPE *h; 
	VTYPE *k; 
	float test_float,number; 

	
	spline2 = NULL; 
	test_float = 1.2; 
		/* Create the field */ 
	spline = VistaIOCreateSpline2D(nx,ny,sx, sy, "bspline4", 2, TYPEREPN);

	
	if (spline->x_range != sx)
		VistaIOWarning("x_range not set properly: expect %f, got %f", sx, spline->x_range);	

	if (spline->y_range != sy)
		VistaIOWarning("y_range not set properly: expect %f, got %f", sy, spline->y_range);	
	
	/* put in some data */
	h = spline->data->p.data;
	for (y = 0; y < ny; y++) 
		for (x = 0; x < nx; x++){
			
			*h++ = x; 
			*h++ = y + x;
		}

	if (!spline->attr) 
		spline->attr = VistaIOCreateAttrList(); 
	/* add some attribute the the field */
	VistaIOSetAttr(spline->attr,"test_attribute", NULL, VistaIOFloatRepn, test_float);
	
		
	/* Write out the data */
	VistaIOAttrList list = VistaIOWrite2DSpline(spline);
	
	/* Read field from file */     
	spline2 = VistaIORead2DSpline(list);
	
	
	/* testing follows */ 
	VistaIOField2D field = spline->data; 
	VistaIOField2D field2 = spline2->data; 

	h = field->p.data;
	k = field2->p.data;
	z = field->nsize; 
	if (z != field2->nsize) {
		VistaIOError("Error rereading test file");
	}
	
	z /= sizeof(VTYPE); 
	
	while (z--) {
		if (*h != *k) {
			VistaIOWarning("spline en/decoding error: delta = %e",*h - *k);
		}
		h++; 
		k++; 
			
	}
	
	if (spline2->x_range != sx)
		VistaIOWarning("x_range not set properly: expect %f, got %f", sx, spline2->x_range);	

	if (spline2->y_range != sy)
		VistaIOWarning("y_range not set properly: expect %f, got %f", sy, spline2->y_range);	
	
	if (!VistaIOLookupAttr(spline2->attr,"test_attribute",&pos)) 
		VistaIOWarning("'test_attribute'  not found");	

	if (strcmp("bspline4", spline2->kernel_descr)) 
	    VistaIOWarning("kernel not set properly: expect 'bspline4', got %s", spline2->kernel_descr);
	
	VistaIOGetAttrValue(&pos, NULL, VistaIOFloatRepn, &number);

	if (number != test_float) {
		VistaIOError("numbers differ by %e",number - test_float);
	}
	VistaIODestroySpline2D(spline);
	VistaIODestroySpline2D(spline2);

	remove( "test.v" ); 
	return 0;
}

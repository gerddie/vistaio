/*
 *  Copyright (c) Leipzig, Madrid 2004 - 2015
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appears in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation. The authors makes no representations about the suitability 
 *  of this software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author:  Gert Wollny <gw.fossdev@gmail.com> 
 * 
 */

#include <string.h>
#include <stdio.h>
#include <vistaio/vistaio.h>



int nx = 7; 
int ny = 8;
int nz = 9; 

float dx = 1.6; 
float dy = 2.2;
float dz = 3.2; 


#define VTYPE double
#define TYPEREPN VistaIODoubleRepn

VistaIOBoolean RepnFilter(VistaIOBundle UNUSED(bundle), VistaIORepnKind repn) 
{
	return repn == VistaIOImageRepn; 
}


#define CHECK_EQUAL(A, B, ErrorMsg) \
	if (A != B) VWarning(ErrorMsg, A, B)


int main(int UNUSED(argc), const char **UNUSED(args))
{
	VistaIOImage image1; 
	VistaIOImage image2;
	VistaIOAttrList list; 
	VistaIOAttrList in_list; 
	int x,y,z; 
	float test_float,number; 
	FILE *file; 
	int tests_run = 0;
	int tests_failed = 0;
	
	image2 = NULL; 
	test_float = 1.2;
	
	/* Create the image */ 
	image1 = VistaIOCreateImage(nz, ny, nx, VistaIOShortRepn);

	
	/* put in some data */
	VistaIOShort *s = VistaIOPixelPtr(image1, 0, 0, 0);
	for (z = 0; z < nz; z++) 
		for (y = 0; y < ny; y++) 
			for (x = 0; x < nx; x++){
				*s++ = x + 10 * y + 100 * z;
			}

	/* add some attribute the the field */
	VistaIOSetAttr(VistaIOImageAttrList(image1),"test_attribute", NULL, VistaIOFloatRepn, test_float);


	list = VistaIOCreateAttrList();
	VistaIOSetAttr(list, "test-image", NULL, VistaIOImageRepn, image1);

	/* open file for saving */
	if ((file = fopen("test-image.v","w"))==0)
		VistaIOError("Unable to open file 'test-image.v' for writing");

	VistaIOWriteFile(file, list);
	fclose(file);

	if ((file = fopen("test-image.v","r")) == NULL)
		VistaIOError("Unable to open file test-image.v for reading");

	/* Read field from file */     
	in_list = VistaIOReadFile(file, RepnFilter);

	++tests_run; 
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(in_list, "test-image", NULL, VistaIOImageRepn, &image2)) {
		VistaIOWarning("Attribute test-image not found");
		++tests_failed; 
	}
	
	++tests_run; 
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(VistaIOImageAttrList(image1),"test_attribute", NULL, VistaIOFloatRepn, &number)) {
		VistaIOWarning("Attribute test_attribute not found");
		++tests_failed;
	}

	++tests_run; 
	if ( number != test_float) {
		VistaIOWarning("Attribute test_float: got %f, expect %f", number, test_float); 
		++tests_failed;
	}
	
	
#define CHECK_EQUAL_IMAGE_ATTRIBUTE(A, B, ATTR)				\
	++tests_run;							\
	if (A -> ATTR != B -> ATTR) {					\
		VistaIOWarning( "Image %s: got %d, expect %d", #ATTR,  A -> ATTR, B -> ATTR); \
		++tests_failed;						\
	}
	
	
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, nrows);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, nbands);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, ncolumns);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, nframes);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, pixel_repn);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, nviewpoints);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, ncolors);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image2, image1, ncomponents); 

	/* put in some data */
	VistaIOShort *t = VistaIOPixelPtr(image2, 0, 0, 0);
	for (z = 0; z < nz; z++) 
		for (y = 0; y < ny; y++) 
			for (x = 0; x < nx; x++, ++t){
				++tests_run;
				if (*t != x + 10 * y + 100 * z) {
					VistaIOWarning("Pixel value at (%d, %d, %d): got %hd, expect %d",
						 x,y,z,*t, x + 10 * y + 100 * z);
					++tests_failed; 
				}
			}
	
	
	remove( "test-image.v" );

	fprintf(stderr,"%d out of %d tests failed\n", tests_failed, tests_run); 
	
	return tests_failed != 0; 
}

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



int nx = 1024; 
int ny = 1024;
int nz = 520; 

int nx2 = 24; 
int ny2 = 23;
int nz2 = 52; 


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
	VistaIOImage image_in1;
	VistaIOImage image_in2;

	VistaIOImage image_read1;
	VistaIOImage image_read2;

	
	VistaIOAttrList list; 
	VistaIOAttrList in_list; 
	int x,y,z; 
	float test_float,number; 
	FILE *file; 
	int tests_run = 0;
	int tests_failed = 0;
	
	image_read1 = NULL; 
	test_float = 1.2;
	
	/* Create the image */ 
	image_in1 = VistaIOCreateImage(nz, ny, nx, VistaIOLongRepn);
	image_in2 = VistaIOCreateImage(nz2, ny2, nx2, VistaIOShortRepn);

	
	/* put in some data */
	VistaIOLong *s1 = VistaIOPixelPtr(image_in1, 0, 0, 0);
	for (z = 0; z < nz; z++) 
		for (y = 0; y < ny; y++) 
			for (x = 0; x < nx; x++){
				*s1++ = x + 10 * y + 100 * z;
			}

	VistaIOShort *s2 = VistaIOPixelPtr(image_in2, 0, 0, 0);
	for (z = 0; z < nz2; z++) 
		for (y = 0; y < ny2; y++) 
			for (x = 0; x < nx2; x++){
				*s2++ = x + 10 * y + 100 * z;
			}

	
	/* add some attribute the the field */
	VistaIOSetAttr(VistaIOImageAttrList(image_in1),"test_attribute", NULL, VistaIOFloatRepn, test_float);


	list = VistaIOCreateAttrList();
	VistaIOSetAttr(list, "test-image1", NULL, VistaIOImageRepn, image_in1);
	VistaIOSetAttr(list, "test-image2", NULL, VistaIOImageRepn, image_in2);

	/* open file for saving */
	if ((file = fopen("test-big.v","w"))==0)
		VistaIOError("Unable to open file 'test-image-big.v' for writing");

	VistaIOWriteFile(file, list);
	fclose(file);

	if ((file = fopen("test-big.v","r")) == NULL)
		VistaIOError("Unable to open file test-image-big.v for reading");

	/* Read field from file */     
	in_list = VistaIOReadFile(file, RepnFilter);

	++tests_run; 
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(in_list, "test-image1", NULL, VistaIOImageRepn, &image_read1)) {
		VistaIOWarning("Attribute test-image1 not found");
		++tests_failed; 
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(in_list, "test-image2", NULL, VistaIOImageRepn, &image_read2)) {
		VistaIOWarning("Attribute test-image2 not found");
		++tests_failed; 
	}

	
	++tests_run; 
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(VistaIOImageAttrList(image_in1),"test_attribute", NULL, VistaIOFloatRepn, &number)) {
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
	
	
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, nrows);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, nbands);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, ncolumns);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, nframes);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, pixel_repn);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, nviewpoints);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, ncolors);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read1, image_in1, ncomponents); 

	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, nrows);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, nbands);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, ncolumns);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, nframes);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, pixel_repn);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, nviewpoints);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, ncolors);
	CHECK_EQUAL_IMAGE_ATTRIBUTE(image_read2, image_in2, ncomponents); 

	
	VistaIOLong *t = VistaIOPixelPtr(image_read1, 0, 0, 0);
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


	VistaIOShort *t2 = VistaIOPixelPtr(image_read2, 0, 0, 0);
	for (z = 0; z < nz2; z++) 
		for (y = 0; y < ny2; y++) 
			for (x = 0; x < nx2; x++, ++t2){
				++tests_run;
				if (*t2 != x + 10 * y + 100 * z) {
					VistaIOWarning("Pixel value at (%d, %d, %d): got %hd, expect %d",
						 x,y,z,*t2, x + 10 * y + 100 * z);
					++tests_failed; 
				}
			}

	
//	remove( "test-big.v" );

	fprintf(stderr,"%d out of %d tests failed\n", tests_failed, tests_run); 
	
	return tests_failed != 0; 
}

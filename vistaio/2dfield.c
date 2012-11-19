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
 *  Author:  Gert Wollny, UPM 
 */

#include <vistaio/vistaio.h>
#include <stdio.h>

int nx = 10; 
int ny = 11; 

#define VTYPE double
#define TYPEREPN VistaIODoubleRepn

VistaIOBoolean RepnFilter(VistaIOBundle UNUSED(bundle), VistaIORepnKind repn) 
{
	return repn == VistaIOField2DRepn; 
}

VistaIOField2D VistaIORead2DVectorfield(FILE *file) 
{
	VistaIOField2D result; 
	VistaIOAttrList list; 
	
	result = NULL; 
	
	list = (VistaIOAttrList)VistaIOReadFile(file,RepnFilter);
	
	if (!list) 
		return NULL; 
	
	if (!VistaIOExtractAttr(list, "2DVector",NULL, VistaIOField2DRepn, &result,TRUE)) {
		VistaIOWarning("2DVector element of type 2dfield found");
	}
	
	VistaIODestroyAttrList(list);
	return result;	
}

int VistaIOWrite2DVectorfield(FILE *file, VistaIOField2D field)
{
	int retval; 
	VistaIOField2D help; 
	VistaIOAttrList list;
	
	retval = FALSE; 
	
	list = VistaIOCreateAttrList();
	if (list) {
		
		help = VistaIOMirrorField2D(field);
		
		VistaIOSetAttr(list,"2DVector",NULL,VistaIOField2DRepn,help);
		
		retval = VistaIOWriteFile(file,list);
		
		VistaIODestroyAttrList(list);
	}
	return retval; 
}


int main(int UNUSED(argc), const char **UNUSED(args))
{
	FILE *file; 
	VistaIOField2D field; 
	VistaIOField2D field2;
	VistaIOAttrListPosn pos; 
	int x,y,z; 
	VTYPE *h; 
	VTYPE *k; 
	float test_float,number; 

	
	field2 = NULL; 
	test_float = 1.2; 
	
	/* Create the field */ 
	field = VistaIOCreateField2D(nx,ny,2,TYPEREPN);
	
	/* put in some data */
	h = field->p.data;
	for (y = 0; y < ny; y++) 
		for (x = 0; x < nx; x++){
			
			*h++ = x; 
			*h++ = y + x;
		}
	
	/* add some attribute the the field */
	VistaIOSetAttr(field->attr,"test_attribute", NULL, VistaIOFloatRepn,test_float);
	
	/* open file for saving */
	if ((file = fopen("test.v","w"))==0)
		VistaIOError("unable to open file");
	
	/* Write out the data */
	VistaIOWrite2DVectorfield(file,field);
	
	/* close the file */ 
	fclose(file);
	
	
	/* open file for reading */
	if ((file = fopen("test.v","r")) == NULL)
	    VistaIOError("unable to open file for reading");
	
	/* Read field from file */     
	field2 = VistaIORead2DVectorfield(file);
	
	/* close the file */ 
	fclose(file);
	
	
	/* testing follows */ 
	
	h = field->p.data;
	k = field2->p.data;
	z = field->nsize; 
	if (z != field2->nsize) {
		VistaIOError("Error rereading test file");
	}
	
	z /= sizeof(VTYPE); 
	
	while (z--) {
		if (*h != *k) {
			VistaIOWarning("field en/decoding error: delta = %e",*h - *k);
		}
		h++; 
		k++; 
			
	}
	
	if (!VistaIOLookupAttr(field2->attr,"test_attribute",&pos)) 
		VistaIOError("not found");	
	
	
	VistaIOGetAttrValue(&pos, NULL, VistaIOFloatRepn, &number);

	if (number != test_float) {
		VistaIOError("numbers differ by %e",number - test_float);
	}
	VistaIODestroyField2D(field);
	VistaIODestroyField2D(field2);

	remove( "test.v" ); 
	return 0;
}

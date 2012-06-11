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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <vistaio/vistaio.h>



int nx = 10; 
int ny = 11; 

#define VTYPE double
#define TYPEREPN VDoubleRepn

VBoolean RepnFilter(VBundle bundle, VRepnKind repn) 
{
	return repn == VField2DRepn; 
}

VField2D VRead2DVectorfield(FILE *file) 
{
	VField2D result; 
	VAttrList list; 
	
	result = NULL; 
	
	list = (VAttrList)VReadFile(file,RepnFilter);
	
	if (!list) 
		return NULL; 
	
	if (!VExtractAttr(list, "2DVector",NULL, VField2DRepn, &result,TRUE)) {
		VWarning("2DVector element of type 2dfield found");
	}
	
	VDestroyAttrList(list);
	return result;	
}

int VWrite2DVectorfield(FILE *file, VField2D field)
{
	int retval; 
	VField2D help; 
	VAttrList list;
	
	retval = FALSE; 
	
	list = VCreateAttrList();
	if (list) {
		
		help = VMirrorField2D(field);
		
		VSetAttr(list,"2DVector",NULL,VField2DRepn,help);
		
		retval = VWriteFile(file,list);
		
		VDestroyAttrList(list);
	}
	return retval; 
}


int main(int argc, const char *args[])
{
	FILE *file; 
	VField2D field; 
	VField2D field2;
	VAttrListPosn pos; 
	int x,y,z; 
	VTYPE *h; 
	VTYPE *k; 
	float test_float,number; 

	
	field2 = NULL; 
	test_float = 1.2; 
	
	/* Create the field */ 
	field = VCreateField2D(nx,ny,2,TYPEREPN);
	
	/* put in some data */
	h = field->p.data;
	for (y = 0; y < ny; y++) 
		for (x = 0; x < nx; x++){
			
			*h++ = x; 
			*h++ = y + x;
		}
	
	/* add some attribute the the field */
	VSetAttr(field->attr,"test_attribute", NULL, VFloatRepn,test_float);
	
	/* open file for saving */
	if ((file = fopen("test.v","w"))==0)
		VError("unable to open file");
	
	/* Write out the data */
	VWrite2DVectorfield(file,field);
	
	/* close the file */ 
	fclose(file);
	
	
	/* open file for reading */
	if ((file = fopen("test.v","r")) == NULL)
	    VError("unable to open file for reading");
	
	/* Read field from file */     
	field2 = VRead2DVectorfield(file);
	
	/* close the file */ 
	fclose(file);
	
	
	/* testing follows */ 
	
	h = field->p.data;
	k = field2->p.data;
	z = field->nsize; 
	if (z != field2->nsize) {
		VError("Error rereading test file");
	}
	
	z /= sizeof(VTYPE); 
	
	while (z--) {
		if (*h != *k) {
			VWarning("field en/decoding error: delta = %e",*h - *k);
		}
		h++; 
		k++; 
			
	}
	
	if (!VLookupAttr(field2->attr,"test_attribute",&pos)) 
		VError("not found");	
	
	
	VGetAttrValue(&pos, NULL, VFloatRepn, &number);

	if (number != test_float) {
		VError("numbers differ by %e",number - test_float);
	}
	VDestroyField2D(field);
	VDestroyField2D(field2);

	unlink( "test.v" ); 
	return 0;
}

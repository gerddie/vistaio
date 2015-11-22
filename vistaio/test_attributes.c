/*
 *  Copyright (c) Madrid 2015
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appears in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation. The authors makes no representations about the suitability 
 *  of this software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author: Gert Wollny <gw.fossdev@gmail.com>
 */

#include <vistaio/vistaio.h>
#include <stdio.h>



int main(int UNUSED(argc), const char **UNUSED(args))
{
	FILE *file;
	
	VistaIOAttrList test_list;
	VistaIODouble test_double = -3.0;
	VistaIOFloat test_float = 2.0;
	VistaIOShort test_short = 3;
	VistaIOLong  test_long = 6;
	VistaIOString test_string = "This is a test string \" ;}:"; 


	VistaIOAttrList read_list;
	VistaIODouble read_double = 1.0;
	VistaIOFloat read_float = 1.0;
	VistaIOShort read_short = 0;
	VistaIOLong  read_long = 0;
	VistaIOString read_string = NULL;

	int test_failed = FALSE; 
	
	test_list = VistaIOCreateAttrList();

	VistaIOSetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, test_double);
	VistaIOSetAttr(test_list, "test-float", NULL, VistaIOFloatRepn, test_float);
	VistaIOSetAttr(test_list, "test-short", NULL, VistaIOShortRepn, test_short);
	VistaIOSetAttr(test_list, "test-long", NULL, VistaIOLongRepn, test_long);
	VistaIOSetAttr(test_list, "test-string", NULL, VistaIOStringRepn, test_string);

	
	/* open file for saving */
	if ((file = fopen("test-attr.v","w"))==0) {
		VistaIOError("Unable to open file 'test-attr.v' for writing");
		++test_failed;  
	}
	
	VistaIOWriteFile(file,test_list);
	fclose(file);

	
	/* open file for reading */
	if ((file = fopen("test-attr.v","r")) == NULL) {
	    VistaIOError("Unable to open file test-attr.v for reading");
	    ++test_failed;  
	}

	/* Read field from file */     
	read_list = VistaIOReadFile(file, NULL);

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, &read_double)) {
		VistaIOWarning("Attribute test-double not found");
		++test_failed; 
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-float", NULL, VistaIOFloatRepn, &read_float)) {
		VistaIOWarning("Attribute test-float not found");
		++test_failed;  
	}
		
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-short", NULL, VistaIOShortRepn, &read_short)) {
		++test_failed;  
		VistaIOWarning("Attribute test-short not found");
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long", NULL, VistaIOLongRepn, &read_long)) {
		++test_failed;  
		VistaIOWarning("Attribute test-short not found");
	}
	
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-string", NULL, VistaIOStringRepn, &read_string)) {
		VistaIOWarning("Attribute test-string not found");
		++test_failed;  
	}
	
	if (test_double != read_double) {
		VistaIOWarning("Attribute test-double: read %f, expected %f", read_double, test_double);
		++test_failed;  
	}

	if (test_float != read_float) {
		VistaIOWarning("Attribute test-float: read %f, expected %f", read_float, test_float);
		++test_failed;  
	}

	if (test_short != read_short) {
		VistaIOWarning("Attribute test-short: read %f, expected %f", read_short, test_short);
		++test_failed;  
	}
		
	if (test_long != read_long) {
		VistaIOWarning("Attribute test-long: read %f, expected %f", read_long, test_long);
		++test_failed;  
	}

	if (strcmp(read_string, test_string)) {
		VistaIOWarning("Attribute test-string: read '%s', expected '%s'", read_string, test_string);
		++test_failed;  
	}
			
	
	VistaIODestroyAttrList(test_list);
	VistaIODestroyAttrList(read_list);

	if (remove( "test-attr.v" ) != 0) {
		VistaIOWarning("Unable to remove test file test-attr.v");
	}
	
	return test_failed;
}

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
	
	test_list = VistaIOCreateAttrList();

	VistaIOSetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, test_double);
	VistaIOSetAttr(test_list, "test-float", NULL, VistaIOFloatRepn, test_float);
	VistaIOSetAttr(test_list, "test-short", NULL, VistaIOShortRepn, test_short);
	VistaIOSetAttr(test_list, "test-long", NULL, VistaIOLongRepn, test_long);
	VistaIOSetAttr(test_list, "test-string", NULL, VistaIOStringRepn, test_string);

	
	/* open file for saving */
	if ((file = fopen("test-attr.v","w"))==0)
		VistaIOError("unable to open file");
	
	VistaIOWriteFile(file,test_list);
	fclose(file);

	/* open file for reading */
	if ((file = fopen("test-attr.v","r")) == NULL)
	    VistaIOError("unable to open file for reading");

	/* Read field from file */     
	read_list = VistaIOReadFile(file, NULL);

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, &read_double))
		VistaIOError("Attribute test-double not found");

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-float", NULL, VistaIOFloatRepn, &read_float))
		VistaIOError("Attribute test-float not found");
	
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-short", NULL, VistaIOShortRepn, &read_short))
		VistaIOError("Attribute test-short not found");

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long", NULL, VistaIOLongRepn, &read_long))
		VistaIOError("Attribute test-short not found");
	
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-string", NULL, VistaIOStringRepn, &read_string))
		VistaIOError("Attribute test-string not found");
	
	if (test_double != read_double)
		VistaIOError("Attribute test-double: read %f, expected %f", read_double, test_double);

	if (test_float != read_float)
		VistaIOError("Attribute test-float: read %f, expected %f", read_float, test_float);

	if (test_short != read_short)
		VistaIOError("Attribute test-short: read %f, expected %f", read_short, test_short);
		
	if (test_long != read_long)
		VistaIOError("Attribute test-long: read %f, expected %f", read_long, test_long);

	if (strcmp(read_string, test_string)) {
		VistaIOError("Attribute test-string: read '%s', expected '%s'", read_string, test_string);
	}
			
	
	VistaIODestroyAttrList(test_list);
	VistaIODestroyAttrList(read_list);

	remove( "test-attr.v" ); 
	return 0;
}

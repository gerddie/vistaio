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
#include <inttypes.h>


int main(int UNUSED(argc), const char **UNUSED(args))
{
	FILE *file;
	
	VistaIOAttrList test_list;
	VistaIODouble test_double = -3.0;
	VistaIOFloat test_float = 2.0;
	VistaIOShort test_short = 3;
	VistaIOShort test_short_negative = -4;
	VistaIOLong  test_long = 6;
	VistaIOLong  test_long_negative = -6;
	VistaIOLong64  test_long64 = -10;
	VistaIOLong64  test_long64_max = INT64_MAX;

	
	VistaIOString test_string = "This is a test string \" ;}:"; 


	VistaIOAttrList read_list;
	VistaIODouble read_double = 1.0;
	VistaIOFloat read_float = 1.0;
	VistaIOShort read_short = 0;
	VistaIOShort read_short_negative = 0;
	VistaIOLong  read_long = 0;
	VistaIOLong  read_long_negative = 0;
	VistaIOLong64  read_long64 = 0;
	VistaIOLong64 read_long64_max = 0; 
	VistaIOString read_string = NULL;

	int test_failed = FALSE; 
	
	test_list = VistaIOCreateAttrList();

	VistaIOSetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, test_double);
	VistaIOSetAttr(test_list, "test-float", NULL, VistaIOFloatRepn, test_float);
	VistaIOSetAttr(test_list, "test-short", NULL, VistaIOShortRepn, test_short);
	VistaIOSetAttr(test_list, "test-short-negative", NULL, VistaIOShortRepn, test_short_negative);
	VistaIOSetAttr(test_list, "test-long", NULL, VistaIOLongRepn, test_long);
	VistaIOSetAttr(test_list, "test-long-negative", NULL, VistaIOLongRepn, test_long_negative);
	VistaIOSetAttr(test_list, "test-long64", NULL, VistaIOLong64Repn, test_long64);
	VistaIOSetAttr(test_list, "test-long64-max", NULL, VistaIOLong64Repn, test_long64_max);
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
	    VistaIOGetAttr(test_list, "test-short-negative", NULL, VistaIOShortRepn, &read_short_negative)) {
		++test_failed;  
		VistaIOWarning("Attribute test-short-negative not found");
	}

	
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long", NULL, VistaIOLongRepn, &read_long)) {
		++test_failed;  
		VistaIOWarning("Attribute test-long not found");
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long-negative", NULL, VistaIOLongRepn, &read_long_negative)) {
		++test_failed;  
		VistaIOWarning("Attribute test-long-negative not found");
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long64", NULL, VistaIOLong64Repn, &read_long64)) {
		++test_failed;  
		VistaIOWarning("Attribute test-long64 not found");
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-long64-max", NULL, VistaIOLong64Repn, &read_long64_max)) {
		++test_failed;  
		VistaIOWarning("Attribute test-long64-max not found");
	}

	
	
	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-string", NULL, VistaIOStringRepn, &read_string)) {
		VistaIOWarning("Attribute test-string not found");
		++test_failed;  
	}
	
	if (test_double != read_double) {
		VistaIOWarning("Attribute test-double: read %g, expected %g", read_double, test_double);
		++test_failed;  
	}

	if (test_float != read_float) {
		VistaIOWarning("Attribute test-float: read %f, expected %f", read_float, test_float);
		++test_failed;  
	}

	if (test_short != read_short) {
		VistaIOWarning("Attribute test-short: read %hd, expected %hd", read_short, test_short);
		++test_failed;  
	}

	if (test_short_negative != read_short_negative) {
		VistaIOWarning("Attribute test-short: read %hd, expected %hd", read_short_negative, test_short_negative);
		++test_failed;  
	}

	
	if (test_long != read_long) {
		VistaIOWarning("Attribute test-long: read %d, expected %d", read_long, test_long);
		++test_failed;  
	}

	if (test_long_negative != read_long_negative) {
		VistaIOWarning("Attribute test-long-negative: read %ld, expected %ld", read_long_negative,
			       test_long_negative);
		++test_failed;  
	}

	if (test_long64 != read_long64) {
		VistaIOWarning("Attribute test-long64: read %" PRId64 ", expected %" PRId64, read_long64, test_long64);
		++test_failed;  
	}

	if (test_long64_max != read_long64_max) {
		VistaIOWarning("Attribute test-long64-max: read %" PRId64 ", expected %" PRId64,
			       read_long64_max, test_long64_max);
		++test_failed;  
	}

	
	
	if (strcmp(read_string, test_string)) {
		VistaIOWarning("Attribute test-string: read '%s', expected '%s'", read_string, test_string);
		++test_failed;  
	}
			
	
	VistaIODestroyAttrList(test_list);
	VistaIODestroyAttrList(read_list);

/*	
	if (remove( "test-attr.v" ) != 0) {
		VistaIOWarning("Unable to remove test file test-attr.v");
	}
*/	
	return test_failed;
}

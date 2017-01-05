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


int test_64bit_image_attribute(void)
{
	FILE *file;
	char header[8];
	int version = 0; 
	int test_failed = 0; 
	VistaIOAttrList test_list;
	test_list = VistaIOCreateAttrList();
	int64_t value = 200000000000LL;
	VistaIOImage image = VistaIOCreateImage(1,1,1, VistaIOUByteRepn);

	VistaIOSetAttr(VistaIOImageAttrList(image), "test-int64", NULL, VistaIOLong64Repn, value);
	
	VistaIOSetAttr(test_list, "image", NULL, VistaIOImageRepn, image);

	/* open file for reading */
	if ((file = fopen("test-imageattr-int64.v","w")) == NULL) {
		VistaIOError("Unable to open file test-attr-int64.v for writing");
		++test_failed;
	}
	VistaIOWriteFile(file,test_list);
	fclose(file);

	
	if ((file = fopen("test-imageattr-int64.v","r")) == NULL) {
		VistaIOError("Unable to open file test-attr-int64.v for reading");
		++test_failed;
	}

	if (fscanf(file, "%7s %d", header, &version) != 2) {
		VistaIOError("Unable to open file test-imageattr-int64.v for reading");
		++test_failed;
	}
	fclose(file);

	if (version != 3) {
		VistaIOWarning("Expect file type 3 got %d", version);
		++test_failed;
	}
		
	return test_failed;
	
}


int test_64bit_v3(void)
{
	int test_failed = 0; 
	VistaIOAttrList test_list;
	test_list = VistaIOCreateAttrList();
	int64_t value = 200000000000LL;
	long len; 

	FILE *file = NULL;
	char buffer[1024];
	const char expect[] = "V-data 3 {\n\ttest-int64: 200000000000\n}\n\f\n"; 
	
	VistaIOSetAttr(test_list, "test-int64", NULL, VistaIOLong64Repn, value);

	/* open file for reading */
	if ((file = fopen("test-attr-int64.v","w")) == NULL) {
		VistaIOError("Unable to open file test-attr-int64.v for writing");
		++test_failed;
	}

	VistaIOWriteFile(file,test_list);
	fclose(file);

	
	if ((file = fopen("test-attr-int64.v","r")) == NULL) {
		VistaIOError("Unable to open file test-attr-int64.v for reading");
		++test_failed;
	}

	
	buffer[0] = 0; 

	len = fread(buffer, 1, 42, file);
	fclose(file);
	if (len != 41) {
		VistaIOWarning(" Got: %ld bytes, expected 41\n", len);
		++test_failed;
	}	

	
	if (strcmp(buffer, expect)) {
		VistaIOWarning(" Got: \n'%s'\n Expect:\n'%s'\n", buffer, expect);
		++test_failed;
	}
	
	return test_failed;
	
}

int test_nested_list(void)
{

	int test_failed = 0; 
	VistaIOAttrList test_list;
	VistaIOAttrList test_list_nested; 
	VistaIODouble test_double = -3.0;
	VistaIODouble test_double2 = 3.0;

	VistaIODouble read_double = 0;
	VistaIODouble read_double2 = 0;

	
	VistaIOAttrList test_list_copy;
	VistaIOAttrList test_list_copy_nested;
	
	test_list = VistaIOCreateAttrList();

	VistaIOSetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, test_double);

	test_list_nested = VistaIOCreateAttrList();

	VistaIOSetAttr(test_list, "test-list", NULL, VistaIOAttrListRepn, test_list_nested);
	VistaIOSetAttr(test_list_nested, "test-double", NULL, VistaIODoubleRepn, test_double2);

	test_list_copy = VistaIOCopyAttrList(test_list);

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-double", NULL, VistaIODoubleRepn, &read_double)) {
		VistaIOWarning("test_nested_list: Attribute test-double not found");
		++test_failed; 
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list, "test-list", NULL, VistaIOAttrListRepn, &test_list_copy_nested)) {
		VistaIOWarning("test_nested_list: Attribute test-list not found");
		++test_failed; 
	}

	if (VistaIOAttrFound !=
	    VistaIOGetAttr(test_list_copy_nested, "test-double", NULL, VistaIODoubleRepn, &read_double2)) {
		VistaIOWarning("test_nested_list: Attribute test-list not found");
		++test_failed; 
	}

	
	VistaIODestroyAttrList(test_list);
	VistaIODestroyAttrList(test_list_copy);

	if (read_double != test_double) {
		VistaIOWarning("test_nested_list: Attribute read_double %g, expect %g", read_double, test_double);
		++test_failed; 
	}
	
	if (read_double2 != test_double2) {
		VistaIOWarning("test_nested_list: Attribute read_double %g, expect %g", read_double2, test_double2);
		++test_failed; 
	}

	return test_failed; 
	
}
	

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

	int test_failed = 0; 
	
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

	
	if (remove( "test-attr.v" ) != 0) {
		VistaIOWarning("Unable to remove test file test-attr.v");
	}

	
	test_failed += test_nested_list();
	test_failed += test_64bit_v3();
	test_failed += test_64bit_image_attribute(); 
		
	return test_failed;
}

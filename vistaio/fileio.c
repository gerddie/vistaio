/*
 *  Copyright 1993, 1994 University of British Columbia
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
 *  Author: Arthur Pope, UBC Laboratory for Computational Intelligence
 *  Maintainance and Fixes: Gert Wollny, UPM 
 */


#include <ctype.h>

#include "vista.h"

/*
 *  Local definitions.
 */

/* Macro used in WriteFile, WriteAttrList, etc.: */
#define FailTest(put)	    if ((put) == EOF) goto Fail

/*! \struct DataBlock
 *  \brief  Description of object with data block to be written later by VistaIOWriteFile
 */

typedef struct {
	VistaIOAttrListPosn posn;	/* identify of object's attribute */
	VistaIOAttrList list;		/* attr list value referring to data */
	int64_t length;		/* length of data block */
} DataBlock;

typedef struct {
	char *buf; 
	int64_t max_len; 
} ReadStringBuf; 


typedef struct {
	int64_t offset;		/* current offset into file's binary data */
	VistaIOList data_list;		/* list of data blocks to write later */
	VistaIOBoolean require_filetype_3; 
}SaveData ; 

/* Later in this file: */
static VistaIOBoolean ReadHeader (FILE *);
static VistaIOAttrList ReadAttrList (FILE *, ReadStringBuf *);
static char *ReadString (FILE *, char, VistaIOStringConst, ReadStringBuf *);
static VistaIOBoolean ReadDelimiter (FILE *);
static VistaIOBoolean ReadData (FILE *, VistaIOAttrList, VistaIOReadFileFilterProc *, int64_t *);
static VistaIOBoolean WriteAttrList (FILE *, VistaIOAttrList, int);
static VistaIOBoolean WriteAttr (FILE *, VistaIOAttrListPosn *, int);
static VistaIOBoolean WriteString (FILE *, const char *);
static VistaIOBoolean MySeek (FILE *, int64_t);



static void EmptyShowProgress(int UNUSED(pos), int UNUSED(length), void *UNUSED(data)) 
{
}

static VistaIOShowProgressFunc VistaIOShowReadProgress = EmptyShowProgress;
static VistaIOShowProgressFunc VistaIOShowWriteProgress = EmptyShowProgress;
static void *VistaIOProgressData = NULL; 


EXPORT_VISTA void VistaIOSetProgressIndicator(VistaIOShowProgressFunc show_read, VistaIOShowProgressFunc show_write, void *data)
{
	VistaIOShowReadProgress = show_read;
	VistaIOShowWriteProgress = show_write; 
	VistaIOProgressData = data; 
}

EXPORT_VISTA void VistaIOResetProgressIndicator(void)
{
	VistaIOShowReadProgress = EmptyShowProgress;
	VistaIOShowWriteProgress = EmptyShowProgress;
	VistaIOProgressData = NULL; 
}

/*! \brief Open an input or output file, with "-" representing stdin or stdout.
 *  
 *  \param  filename
 *  \param  nofail
 *  \return If nofail is TRUE, any failure is a fatal error.
 */

FILE *VistaIOOpenInputFile (VistaIOStringConst filename, VistaIOBoolean nofail)
{
	FILE *f;

	if (filename == NULL || strcmp (filename, "-") == 0)
		f = stdin;
	else if (!(f = fopen (filename, "r")))
		(nofail ? &VistaIOError : &VistaIOWarning)
			("Unable to open input file %s", filename);
	return f;
}

/*! \brief
 *  
 *  \param  filename
 *  \param  nofail
 *  \return FILE
 */

FILE *VistaIOOpenOutputFile (VistaIOStringConst filename, VistaIOBoolean nofail)
{
	FILE *f;

	if (filename == NULL || strcmp (filename, "-") == 0)
		f = stdout;
	else if (!(f = fopen (filename, "w")))
		(nofail ? &VistaIOError : &VistaIOWarning)
			("Unable to open output file %s", filename);
	return f;
}


/*! \brief Read a Vista data file, extract object of a specified type, and 
 *  return a vector of them plus a list of anything else found in the file.
 *
 *  \param  file
 *  \param  repn
 *  \param  attributes
 *  \param  objects
 *  \return int
 */

int VistaIOReadObjects (FILE * file, VistaIORepnKind repn, VistaIOAttrList * attributes,
		  VistaIOPointer ** objects)
{
	VistaIOAttrList list;
	VistaIOAttrListPosn posn;
	int i, nobjects = 0;
	VistaIOPointer *vector;

	/* Read the file's contents: */
	list = VistaIOReadFile (file, NULL);
	if (!list)
		return FALSE;

	/* Count the objects found: */
	for (VistaIOFirstAttr (list, &posn); VistaIOAttrExists (&posn); VistaIONextAttr (&posn))
		nobjects += (VistaIOGetAttrRepn (&posn) == repn);
	if (nobjects == 0) {
		VistaIOWarning ("VistaIOReadObjects: No %s objects present in stream",
			  VistaIORepnName (repn));
		VistaIODestroyAttrList (list);
		return FALSE;
	}

	/* Allocate a vector of that many object pointers: */
	vector = VistaIOMalloc (nobjects * sizeof (VistaIOPointer));

	/* Extract the objects from the attribute list and place them in the
	   vector: */
	for (VistaIOFirstAttr (list, &posn), i = 0; VistaIOAttrExists (&posn);)
		if (VistaIOGetAttrRepn (&posn) == repn) {
			VistaIOGetAttrValue (&posn, NULL, repn, vector + i);
			VistaIODeleteAttr (&posn);
			i++;
		} else
			VistaIONextAttr (&posn);

	/* Return the objects and the remaining attributes: */
	*attributes = list;
	*objects = vector;
	return nobjects;
}


/*! \brief Read a Vista data file, returning an attribute list of its contents.
 *
 *  \param  f
 *  \param  filter
 *  \return VistaIOAttrList
 */

EXPORT_VISTA VistaIOAttrList VistaIOReadFile (FILE * f, VistaIOReadFileFilterProc * filter)
{
	VistaIOAttrList list;
	int i;
 	ReadStringBuf sbuf = {0,0}; 
	int64_t read_offset; 
	
#ifdef SupportUbcIff

	/* If the first byte of the file is "I", it looks like a UBC IFF file: */
	{
		i = fgetc (f);
		ungetc (i, f);
		if (i == 'I') {
			VistaIOImage image = VistaIOReadUbcIff (f);

			if (!image)
				return FALSE;
			list = VistaIOCreateAttrList ();
			VistaIOSetAttr (list, "image", NULL, VistaIOImageRepn, image);
			return list;
		}
	}

#endif

	/* Ensure that the correct Vista data file header is there: */
	if (!ReadHeader (f))
		return NULL;

	list = ReadAttrList (f, &sbuf); 
	if (sbuf.max_len)
		VistaIOFree(sbuf.buf); 
	/* Read all attributes in the file: */
	if (!list)
		return NULL;
	
	/* Swallow the delimiter and read the binary data following it: */
	read_offset = 0;
	if (!ReadDelimiter (f) || !ReadData (f, list, filter, &read_offset)) {
		VistaIODestroyAttrList (list);
		return NULL;
	}

	/* Now we should be at the end of the file: */
	i = fgetc (f);
	if (i != EOF) {
		ungetc (i, f);
		VistaIOWarning ("VistaIOReadFile: File continues beyond expected EOF");
	}
	return list;
}

EXPORT_VISTA VistaIOBoolean VistaIOIsVistaFile (const char *filename)
{
	VistaIOBoolean result = FALSE; 
	FILE *f = fopen(filename, "r");
	
	if (!f)
		return result;

	result = ReadHeader (f);
	fclose(f);
	return result; 
	
}


/*! \brief Read a Vista data file header.
 *
 *  If the current version is 2 but the file is version 1, a warning message
 *  is produced but TRUE is still returned.
 *
 *  \param  f
 *  \return VistaIOBoolean
 */

static VistaIOBoolean ReadHeader (FILE * f)
{
	int version;

	if (fscanf (f, VistaIOFileHeader " %d", &version) != 1) {
		VistaIOWarning ("VistaIOReadFile: Vista data file header not found");
		return FALSE;
	}

	if (version >= VistaIOFileMinVersion)
		return TRUE;


	if (version == 1 && VistaIOFileVersion >= VistaIOFileMinVersion) {
		VistaIOWarning ("VistaIOReadFile: Obsolete data file -- pipe it thru v1to2");
		return TRUE;
	}

	VistaIOWarning ("VistaIOReadFile: Vista data file isn't version >= %d",
		  VistaIOFileMinVersion);
	return FALSE;
}


/*
 *  ReadAttrList
 *
 *  Read a list of attributes from a stream.
 */

static VistaIOAttrList ReadAttrList (FILE * f, ReadStringBuf *sbuf)
{
	VistaIOAttrList sublist, list = VistaIOCreateAttrList ();
	VistaIOAttrRec *a;
	int ch = 0;
	int64_t name_size;
	VistaIOBundle b;
	char buf[2], *str, name_buf[VistaIOMaxAttrNameLength + 1];

	/* Swallow a { marking the start of the attribute list: */
	if (fscanf (f, " %1s", buf) != 1 || buf[0] != '{') {
		VistaIOWarning ("VistaIOReadFile: Missing {");
		goto Fail;
	}

	/* For each attribute up to the next "}": */
	while (fscanf (f, " %255[^}: \t\n]", name_buf) == 1) {
		name_size = strlen (name_buf);
		/* Covertiy does not detect that strlen(name_buf) <= 255 */
		   
		
		/* Read a : */
		if (fscanf (f, " %1s", buf) != 1 || buf[0] != ':') {
			VistaIOWarning ("VistaIOReadFile: Invalid %s attribute", name_buf);
			goto Fail;
		}
		
		do {
			ch = fgetc(f);
		} while (ch == ' ');

		/* File terminated early? */ 
		if (ch == EOF)
			goto Fail;
		
		buf[0] = (char)ch;
		
		/* The first character of the value tells us whether its an attribute
		   list, quoted string, or unquoted string: */
		if (buf[0] == '{') {

			/* The attribute value is another list of attributes: */
			ungetc ('{', f);
			if (!(sublist = ReadAttrList (f, sbuf)))
				goto Fail;
			a = VistaIOMalloc (sizeof (VistaIOAttrRec) + name_size);
			a->value = sublist;
			a->repn = VistaIOAttrListRepn;

		} else if (buf[0] != '}' && buf[0] != '\n') {/* non-empty attr */

			/* The value doesn't start with '{' -- parse a word or string: */
			if (!(str = ReadString (f, buf[0], name_buf, sbuf))) 
				goto Fail;
			
			while ((ch = fgetc (f)) && (ch == ' ' || ch == '\t'));
			ungetc (ch, f);

			/* If the word is followed by an '{'... */
			if (ch == '{') {

				/* ...then it's a typed value -- the word is it's type name
				   and the { is the start of it's attribute list value. */
				b = VistaIOCreateBundle (str, NULL, 0, NULL);
				if (!(sublist = ReadAttrList (f, sbuf))) {
					VistaIOFree (b);
					goto Fail;
				}
				b->list = sublist;
				a = VistaIOMalloc (sizeof (VistaIOAttrRec) + name_size);
				a->repn = VistaIOBundleRepn;
				a->value = b;
			} else {

				/* ...otherwise store it as a simple string value: */
				/*coverity[INTEGER_OVERFLOW]   name_size is at most 255 (line 309)*/
				a = VistaIOMalloc (sizeof (VistaIOAttrRec) + name_size +
					     strlen (str) + 1);
				a->repn = VistaIOStringRepn;
				a->value = a->name + name_size + 1;
				strcpy (a->value, str);
			}
		}else{
			ungetc (buf[0], f);
			a = VistaIOMalloc (sizeof (VistaIOAttrRec) + name_size +
				     strlen ("") + 1);
			a->repn = VistaIOStringRepn;
			a->value = a->name + name_size + 1;
			strcpy (a->value, "");
		}

		/* Copy the attribute's name into the newly allocated node: */
		strcpy (a->name, name_buf);

		/* Place the new node on the end of the growing attribute list: */
		a->next = NULL;
		if ((a->prev = list->prev))
			a->prev->next = a;
		else
			list->next = a;
		list->prev = a;
	}

	/* Swallow the terminating "}": */
	if (fscanf (f, " %1s", buf) != 1 || buf[0] != '}') {
		VistaIOWarning ("VistaIOReadFile: Missing }");
	Fail:
		VistaIODestroyAttrList (list);
		return NULL;
	}
	return list;
}


/*
 *  ReadString
 *
 *  Read an word, string, or number that may or may not be quoted.
 *  If ch is " then  parse a quoted string, otherwise parse to the next
 *  whitespace character. The string is returned in a reusable buffer.
 *
 *  The first several characters are read into a fixed-sized buffer
 *  large enough to hold most values. When it overflows, we begin
 *  allocating and reallocating larger buffers as we continue to read
 *  the value.
 */

#define StringAllocIncrement	100	/* each round of buffer resizing */

static char *ReadString (FILE * f, char ch, VistaIOStringConst name, ReadStringBuf *buf)
{
	VistaIOBoolean escaped = (ch == '"');
	int64_t len = 0;
	char *cp;
	int in_char; 

	if (!buf->buf) {
		buf->buf = VistaIOMalloc (StringAllocIncrement);
		buf->max_len = StringAllocIncrement;
	}

	if (!escaped)
		ungetc (ch, f);

	cp = buf->buf;
	while (1) {
		in_char = fgetc (f);

		/* Check for premature EOF: */
		if (in_char == EOF) {
			VistaIOWarning ("VistaIOReadFile: EOF encountered in %s attribute", name);
			return NULL;
		}

		/* Check for closing " or escape sequence: */
		if (escaped) {
			if (in_char == '"')
				break;
			if (in_char == '\\')
				switch (in_char = fgetc (f)) {

				case '\n':
					continue;

				case 'n':
					in_char = '\n';
					break;
				case EOF:
					VistaIOWarning ("VistaIOReadFile: EOF encountered in %s attribute", name);
					return NULL;
				}
		} else if (isspace (in_char))
			break;

		/* If the buffer in which we're accumulating the value is full,
		   allocate a larger one: */
		if (++len == buf->max_len) {
			buf->buf = VistaIORealloc (buf->buf, buf->max_len += StringAllocIncrement);
			cp = buf->buf + len - 1;
		}

		/* Store the character in the current buffer: */
		*cp++ = (char)in_char;
	}
	*cp = 0;

	/* Allocate a node of the correct size, or trim one already allocated so
	   it is the correct size: */
	return buf->buf;
}


/*
 *  ReadDelimiter
 *
 *  Read a Vista data file delimiter.
 */

static VistaIOBoolean ReadDelimiter (FILE * f)
{
	int ch;
	const char *cp;
	const char *msg = "VistaIOReadFile: Vista data file delimiter not found";

	/* Skip whitespace up to the first character of the delimeter: */
	while ((ch = fgetc (f)) != VistaIOFileDelimiter[0])
		if (ch == EOF || !isspace (ch & 0x7F)) {
			VistaIOWarning (msg);
			return FALSE;
		}

	/* Swallow remaining characters of the delimiter: */
	for (cp = &VistaIOFileDelimiter[1]; *cp; cp++)
		if (*cp != fgetc (f)) {
			VistaIOWarning (msg);
			return FALSE;
		}

	return TRUE;
}


/*
 *  ReadData
 *
 *  Read the binary data accompanying attributes.
 */

static VistaIOBoolean ReadData (FILE * f, VistaIOAttrList list,
				VistaIOReadFileFilterProc * filter, int64_t *read_offset)
{
	VistaIOAttrListPosn posn, subposn;
	VistaIOAttrList sublist;
	VistaIOBundle b;
	VistaIORepnKind repn;
	VistaIOBoolean read_data, data_found, length_found;
	VistaIOLong64 data, length;
	VistaIOTypeMethods *methods;
	VistaIOPointer value;

	for (VistaIOFirstAttr (list, &posn); VistaIOAttrExists (&posn); VistaIONextAttr (&posn)) {
		switch (VistaIOGetAttrRepn (&posn)) {

		case VistaIOAttrListRepn:

			/* Recurse on nested attribute list: */
			VistaIOGetAttrValue (&posn, NULL, VistaIOAttrListRepn, &sublist);
			if (!ReadData (f, sublist, filter, read_offset))
				return FALSE;
			break;

		case VistaIOBundleRepn:
			VistaIOGetAttrValue (&posn, NULL, VistaIOBundleRepn, &b);
			repn = VistaIOLookupType (b->type_name);

			/* If a filter routine was supplied, ask it whether to bother
			   with the binary data: */
			read_data = !filter || (*filter) (b, repn);

			/* Extract any data and length attributes in the object's value: */
			if ((data_found =
			     VistaIOLookupAttr (b->list, VistaIODataAttr, &subposn))) {
				if (!VistaIOGetAttrValue
				    (&subposn, NULL, VistaIOLong64Repn, &data)) {
					VistaIOWarning ("VistaIOReadFile: "
						  "%s attribute's data attribute not found",
						  VistaIOGetAttrName (&posn));
					return FALSE;
				}
				VistaIODeleteAttr (&subposn);
			}
			if ((length_found =
			     VistaIOLookupAttr (b->list, VistaIOLengthAttr, &subposn))) {
				if (!(VistaIOGetAttrValue
				      (&subposn, NULL, VistaIOLong64Repn, &length))) {
					VistaIOWarning ("VistaIOReadFile: "
						  "%s attribute's length attribute incorrect",
						  VistaIOGetAttrName (&posn));
					return FALSE;
				}
				VistaIODeleteAttr (&subposn);
			}

			/* None or both must be present: */
			if (data_found ^ length_found) {
				VistaIOWarning ("VistaIOReadFile: %s attribute has %s but not %s", VistaIOGetAttrName (&posn), data_found ? "data" : "length", data_found ? "length" : "data");
				return FALSE;
			}

			/* Read the binary data associated with the object: */
			if (data_found) {
				if (data < *read_offset) {
					VistaIOWarning ("VistaIOReadFile: "
						  "%s attribute's data attribute incorrect: %ld",
							VistaIOGetAttrName (&posn), data);
					return FALSE;
				}

				if (!read_data)
					data += length;

				/* To seek forward to the start of the data block we first
				   try fseek. That will fail on a pipe, in which case we
				   seek by reading. */
				if (data != *read_offset &&
				    fseek (f, data - *read_offset,
					   SEEK_CUR) == -1 && errno == ESPIPE
				    && !MySeek (f, data - *read_offset)) {
					VistaIOSystemWarning
						("VistaIOReadFile: Seek within file failed");
					return FALSE;
				}

				if (read_data) {
					int64_t pos = 0;
					int64_t togo = length;
					
					b->data = VistaIOMalloc (b->length = length);
					if (!b->data) {
						VistaIOWarning ("VistaIOReadFile: unable to allocate memory");
						return FALSE;
					}
					
					
					while (togo) {
						int64_t read_length = togo < 100000 ? togo : 100000; 
						if (fread ((char *)(b->data) + pos, 1, 
							   read_length, f) != read_length) {
							VistaIOWarning ("VistaIOReadFile: Read from stream failed");
							return FALSE;
						}
						pos += read_length;
						togo -= read_length;
							
						VistaIOShowReadProgress(pos, length, VistaIOProgressData);
					}
					*read_offset = data + length;
				} else
					/* bug: read error occured when bundle was not read
					   by a filter function. FK 24/03/98 */
					*read_offset = data;
			}

			/* Recurse to read binary data for sublist attributes: */
			if (!ReadData (f, b->list, filter, read_offset))
				return FALSE;

			/* If the object's type is registered and has a decode method,
			   invoke it to decode the binary data: */
			if (read_data && repn != VistaIOUnknownRepn &&
			    (methods = VistaIORepnMethods (repn))
			    && methods->decode) {
				if (!
				    (value =
				     (methods->decode) (VistaIOGetAttrName (&posn),
							b)))
					return FALSE;

				/* Replace the old typed value with the newly decoded one: */
				VistaIOSetAttrValue (&posn, NULL, repn, value);
				VistaIODestroyBundle (b);
			}
			break;

		default:
			break;
		}
	}

	return TRUE;
}


/*! \brief Write a list of objects, plus some other attributes, to a Vista data file.
 *
 *  \param  file
 *  \param  repn
 *  \param  attributes
 *  \param  nobjects
 *  \param  objects
 *  \return VistaIOBoolean
 */

VistaIOBoolean VistaIOWriteObjects (FILE * file, VistaIORepnKind repn, VistaIOAttrList attributes,
			int nobjects, VistaIOPointer objects[])
{
	VistaIOAttrList list;
	VistaIOAttrListPosn posn;
	int i;
	VistaIOBoolean result;

	/* Create an attribute list if none was supplied: */
	list = attributes ? attributes : VistaIOCreateAttrList ();

	/* Prepend to the attribute list an attribute for each object: */
	for (i = nobjects - 1; i >= 0; i--)
		VistaIOPrependAttr (list, VistaIORepnName (repn), NULL, repn, objects[i]);

	/* Write the attribute list: */
	result = VistaIOWriteFile (file, list);

	/* Remove the attributes just prepended: */
	VistaIOFirstAttr (list, &posn);
	for (i = 0; i < nobjects; i++)
		VistaIODeleteAttr (&posn);
	if (list != attributes)
		VistaIODestroyAttrList (list);

	return result;
}

static void CollectAttrList (VistaIOAttrList list, SaveData *save_data); 

static void CollectAttr(VistaIOAttrListPosn * posn, SaveData *save_data)
{
	VistaIORepnKind repn;
	VistaIOAttrList sublist;
	VistaIOBundle b;
	DataBlock *db;
	VistaIOTypeMethods *methods;
	int64_t length;
	VistaIOPointer value;

	switch (repn = VistaIOGetAttrRepn (posn)) {

	case VistaIOStringRepn:
		break; 
	case VistaIOAttrListRepn:
		VistaIOGetAttrValue (posn, NULL, VistaIOAttrListRepn,
				     (VistaIOPointer) & sublist);
		CollectAttrList(sublist, save_data);
		break;
	case VistaIOBundleRepn:
		VistaIOGetAttrValue (posn, NULL, VistaIOBundleRepn, (VistaIOBundle) & b);

		/* If it's a typed value with binary data... */
		if (b->length > 0) {

			/* Include "data" and "length" attributes in its attribute list: */
			VistaIOPrependAttr (b->list, VistaIOLengthAttr, NULL, VistaIOLong64Repn,
				      (VistaIOLong64) b->length);
			VistaIOPrependAttr (b->list, VistaIODataAttr, NULL, VistaIOLong64Repn,
				      (VistaIOLong64) save_data->offset);

			/* Add it to the queue of binary data blocks to be written: */
			save_data->offset += b->length;
			db = VistaIONew (DataBlock);
			db->posn = *posn;
			db->list = b->list;
			db->length = b->length;
			VistaIOListAppend (save_data->data_list, db);
		}

		/* Write the typed value's attribute list: */
		CollectAttrList (b->list, save_data);
		break;

	default:
		/* check data types for graph and image */
		switch (repn) {
		case VistaIOImageRepn: {
			VistaIOImage image = NULL;
			VistaIOGetAttrValue (posn, NULL, VistaIOImageRepn, &image);
			if (image) {
				if (VistaIOPixelRepn (image) == VistaIOLong64Repn)
					save_data->require_filetype_3 = 1;
			}else{
				VistaIOWarning("Bogus attribute list: indicate image type but can't get it");
			}
			break; 
		}
			

		case VistaIOGraphRepn: {
			VistaIOGraph graph = NULL;
                               VistaIOGetAttrValue (posn, NULL, VistaIOGraphRepn, &graph);
			       if (graph) {
				       if (VistaIONodeRepn (graph) == VistaIOLong64Repn)
					       save_data->require_filetype_3 = 1;
			       }else{
				       VistaIOWarning("Bogus attribute list: indicate graph type but can't get it");
			       }
			       break; 
		}

		default:
			;
		}
		
		if (!(methods = VistaIORepnMethods (repn)) ||
		    !methods->encode_attr || !methods->encode_data) {
			VistaIOWarning ("VistaIOWriteFile: "
				  "%s attribute has unwriteable representation: %s",
				  VistaIOGetAttrName (posn), VistaIORepnName (repn));
			return;
		}

		/* Invoke the object type's encode_attr method to obtain an
		   attribute list: */
		VistaIOGetAttrValue (posn, NULL, repn, &value);
		sublist = (methods->encode_attr) (value, &length);

		/* If binary data is indicated... */
		if (length > 0) {

			/* Include "data" and "length" attributes in the attr list: */
			VistaIOPrependAttr (sublist, VistaIOLengthAttr, NULL, VistaIOLong64Repn,
				      (VistaIOLong64) length);
			VistaIOPrependAttr (sublist, VistaIODataAttr, NULL, VistaIOLong64Repn,
				      (VistaIOLong64) save_data->offset);

			save_data->offset += length;
		}

		/* Add the object to the queue of binary data blocks to be written: */
		db = VistaIONew (DataBlock);
		db->posn = *posn;
		db->list = sublist;
		db->length = length;
		VistaIOListAppend (save_data->data_list, db);

		/* Write the typed value's attribute list: */
		CollectAttrList (sublist, save_data);
	}
}

static void CollectAttrList (VistaIOAttrList list, SaveData *save_data)
{
	VistaIOAttrListPosn posn;
	VistaIOBoolean nv3 = 0; 

	/* check for the version-3 tag and remove it if available */
	if (VistaIOGetAttr(list, "private-require-v3", NULL, VistaIOBooleanRepn, &nv3) == VistaIOAttrFound) {
		VistaIOExtractAttr(list, "private-require-v3", NULL, VistaIOBooleanRepn, NULL, 0); 
		save_data->require_filetype_3 = nv3;
	}

	/* Collect each attribute in the list: */
	for (VistaIOFirstAttr (list, &posn); VistaIOAttrExists (&posn); VistaIONextAttr (&posn))
		CollectAttr (&posn, save_data);
}
/*! \brief VistaIOWriteFile
 *  
 *  \param  f
 *  \param  list
 *  \return VistaIOBoolean
 */

EXPORT_VISTA VistaIOBoolean VistaIOWriteFile (FILE * f, VistaIOAttrList list)
{
	DataBlock *db;
	VistaIOBundle b;
	VistaIOTypeMethods *methods;
	VistaIORepnKind repn;
	VistaIOPointer value, ptr;
	VistaIOBoolean free_it;
	VistaIOBoolean result = 0;
	uint64_t total_bloblength = 0; 
	SaveData save_data;
	
	save_data.offset = 0;
	save_data.data_list = VistaIOListCreate ();
	save_data.require_filetype_3 = 0; 
	
	/* Write the Vista data file header, attribute list, and delimeter
	   while queuing on data_list any binary data blocks to be written: */

	CollectAttrList (list, &save_data);

	int filetype = VistaIOFileMinVersion; 
	if (save_data.offset > 0x7FFFFFFF || save_data.require_filetype_3)
		filetype = VistaIOFileVersion; 
	
	FailTest (fprintf (f, "%s %d ", VistaIOFileHeader, filetype));
	
	if (!WriteAttrList (f, list, 1)) {
		VistaIOListDestroy (save_data.data_list, VistaIOFree);
		return FALSE;
	}
	FailTest (fputs ("\n" VistaIOFileDelimiter, f));
	fflush (f);

	/* Traverse data_list to write the binary data blocks: */
	for (db = VistaIOListFirst (save_data.data_list); db; db = VistaIOListNext (save_data.data_list)) {
		repn = VistaIOGetAttrRepn (&db->posn);
		if (repn == VistaIOBundleRepn) {

			/* A typed value includes its binary data block explicitly: */
			VistaIOGetAttrValue (&db->posn, NULL, VistaIOBundleRepn, &b);
			ptr = b->data;
			free_it = FALSE;

		} else {

			/* For any other representation, obtain the binary data block
			   from its encode_data method: */
			VistaIOGetAttrValue (&db->posn, NULL, repn, &value);
			methods = VistaIORepnMethods (repn);
			ptr = (methods->encode_data)
				(value, db->list, db->length, &free_it);
			if (!ptr)
				goto Fail;
		}

		/* Write the binary data and free the buffer containing it if it was
		   allocated temporarily by an encode_data method: */
		if (db->length > 0) {
			total_bloblength += db->length; 
			int64_t togo = db->length; 
			int64_t pos = 0; 
			while (togo) {
				int64_t write_length = togo < 100000  ? togo: 100000;
				result = fwrite (((char *)ptr) + pos, 1, write_length, f) == write_length;
				pos += write_length;
				togo -= write_length;
				VistaIOShowWriteProgress(pos, db->length, VistaIOProgressData);
			}
			if (free_it)
				VistaIOFree (ptr);
			if (!result)
				goto Fail;
		}
	}

	VistaIOListDestroy (save_data.data_list, VistaIOFree);
	return TRUE;

      Fail:
	VistaIOWarning ("VistaIOWriteFile: Write to stream failed");
	VistaIOListDestroy (save_data.data_list, VistaIOFree);
	return FALSE;
}


/*! \brief Write a list of attributes to a file. Lines are indented by indent tabs.
 *
 *  \param  f
 */

static VistaIOBoolean WriteAttrList (FILE * f, VistaIOAttrList list, int indent)
{
	VistaIOAttrListPosn posn;
	int i;
	
	/* Write the { marking the beginning of the attribute list: */
	FailTest (fputs ("{\n", f));

	/* Write each attribute in the list: */
	for (VistaIOFirstAttr (list, &posn); VistaIOAttrExists (&posn); VistaIONextAttr (&posn))
		if (!WriteAttr (f, &posn, indent))
			return FALSE;

	/* Write the } marking the end of the attribute list: */
	for (i = indent - 1; i > 0; i--)
		FailTest (fputc ('\t', f));
	FailTest (fputc ('}', f));
	return TRUE;

      Fail:
	VistaIOWarning ("VistaIOWriteFile: Write to stream failed");
	return FALSE;
}


/*
 *  WriteAttr
 *
 *  Write a single attribute to a file. If the attribute's value is
 *  itself an attribute list, it is indented by indent+1 tabs.
 */

static VistaIOBoolean WriteAttr (FILE * f, VistaIOAttrListPosn * posn, int indent)
{
	int i;
	char *str;
	VistaIORepnKind repn;
	VistaIOAttrList sublist;
	VistaIOBundle b;
	VistaIOTypeMethods *methods;
	int64_t length;
	VistaIOPointer value;
	VistaIOBoolean result;
	VistaIOAttrListPosn subposn;

	/* Indent by the specified amount: */
	for (i = 0; i < indent; i++)
		FailTest (fputc ('\t', f));
	indent++;

	/* Output the attribute's name: */
	FailTest (fprintf (f, "%s: ", VistaIOGetAttrName (posn)));

	/* Ouput its value: */
	switch (repn = VistaIOGetAttrRepn (posn)) {

	case VistaIOAttrListRepn:
		VistaIOGetAttrValue (posn, NULL, VistaIOAttrListRepn,
			       (VistaIOPointer) & sublist);
		result = WriteAttrList (f, sublist, indent);
		break;

	case VistaIOBundleRepn:
		VistaIOGetAttrValue (posn, NULL, VistaIOBundleRepn, (VistaIOBundle) & b);
		if (!WriteString (f, b->type_name))
			return FALSE;
		FailTest (fputc (' ', f));

		/* Write the typed value's attribute list: */
		result = WriteAttrList (f, b->list, indent);
		/* Remove the "data" and "length" attributes added earlier: */
		if (b->length > 0) {
			VistaIOFirstAttr (b->list, &subposn);
			VistaIODeleteAttr (&subposn);
			VistaIODeleteAttr (&subposn);
		}
		break;

	case VistaIOStringRepn:
		VistaIOGetAttrValue (posn, NULL, VistaIOStringRepn, (VistaIOPointer) & str);
		result = WriteString (f, str);
		break;
	default:
		if (!(methods = VistaIORepnMethods (repn)) ||
		    !methods->encode_attr || !methods->encode_data) {
			VistaIOWarning ("VistaIOWriteFile: "
				  "%s attribute has unwriteable representation: %s",
				  VistaIOGetAttrName (posn), VistaIORepnName (repn));
			return FALSE;
		}

		/* Write the type name: */
		if (!WriteString (f, VistaIORepnName (repn)))
			return FALSE;
		FailTest (fputc (' ', f));

		/* Invoke the object type's encode_attr method to obtain an
		   attribute list: */
		VistaIOGetAttrValue (posn, NULL, repn, &value);
		sublist = (methods->encode_attr) (value, &length);

		/* Write the typed value's attribute list: */
		result = WriteAttrList (f, sublist, indent);

		/* Remove the "data" and "length" attributes added earlier: */
		if (length > 0) {
			VistaIOFirstAttr (sublist, &subposn);
			VistaIODeleteAttr (&subposn);
			VistaIODeleteAttr (&subposn);
		}
	}

	/* Output a trailing newline: */
	if (result)
		FailTest (fputc ('\n', f));
	return result;

      Fail:
	VistaIOWarning ("VistaIOWriteFile: Write to stream failed");
	return FALSE;
}


/*
 *  WriteString
 *
 *  Write a string, quoting it if necessary.
 */

static VistaIOBoolean WriteString (FILE * f, const char *str)
{
	const char *cp;
	int ch;

	/* Test for the presence of funny characters in the string value: */
	for (cp = str; (ch = *cp); cp++)
		if (!(isalnum (ch)) && (ch != '.') && (ch != '-') && (ch != '+')
		    && (ch != '_'))
			break;

	/* If funny characters are present, output the string in quotes: */
	if (ch) {
		FailTest (fputc ('"', f));
		for (cp = (char *)str; (ch = *cp); cp++)
			switch (ch) {

			case '\n':
				FailTest (fputs ("\\n\\\n", f));
				break;

			case '"':
				FailTest (fputs ("\\\"", f));
				break;

			default:
				FailTest (fputc (ch, f));
			}
		FailTest (fputc ('"', f));
	} else
		FailTest (fputs (str, f));
	return TRUE;

      Fail:
	VistaIOWarning ("VistaIOWriteFile: Write to stream failed");
	return FALSE;
}


/*
 *  MySeek
 *
 *  Seek forward in a stream by reading it. Needed because fseek() won't
 *  work on pipes.
 */

static VistaIOBoolean MySeek (FILE * f, int64_t bytes)
{
	int64_t len;
	char buf[1000];

	while (bytes > 0) {
		len = VistaIOMin ((int64_t)bytes, sizeof (buf));
		if (fread (buf, 1, len, f) != len)
			return FALSE;
		bytes -= len;
	}
	return TRUE;
}

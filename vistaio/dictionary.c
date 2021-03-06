/*
 *  Copyright 1993, 1994 University of British Columbia
 *  Copyright (c) Leipzig, Madrid 2004 - 2016
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

#include "vistaio/vistaio.h"

/*
 *  Dictionaries for generic attributes and their values.
 */

/* Keywords for representing TRUE or FALSE: */
EXPORT_VISTA VistaIODictEntry VistaIOBooleanDict[] = {
	{"false", FALSE, NULL, 0, 0, 0.0}
	,
	{"true", TRUE, NULL, 0, 0, 0.0}
	,
	{"no", FALSE, NULL, 0, 0, 0.0}
	,
	{"yes", TRUE, NULL, 0, 0, 0.0}
	,
	{"off", FALSE, NULL, 0, 0, 0.0}
	,
	{"on", TRUE, NULL, 0, 0, 0.0}
	,
	{NULL, FALSE, NULL, 0, 0, 0.0}
};

/* Keywords for representing kinds of numeric representation: */
EXPORT_VISTA VistaIODictEntry VistaIONumericRepnDict[] = {
	{"bit", VistaIOBitRepn, NULL, 0, 0, 0.0}
	,
	{"double", VistaIODoubleRepn, NULL, 0, 0, 0.0}
	,
	{"float", VistaIOFloatRepn, NULL, 0, 0, 0.0}
	,
	{"long64", VistaIOLong64Repn, NULL, 0, 0, 0.0}
	,
	{"long", VistaIOLongRepn, NULL, 0, 0, 0.0}
	,
	{"sbyte", VistaIOSByteRepn, NULL, 0, 0, 0.0}
	,
	{"short", VistaIOShortRepn, NULL, 0, 0, 0.0}
	,
	{"ubyte", VistaIOUByteRepn, NULL, 0, 0, 0.0}
	,
	{NULL, 0, NULL, 0, 0, 0.0}
};


/*! \brief Look up an entry in an attribute value dictionary, by keyword.
 *  
 *  (It's assumed that dictionaries are pretty small -- a linear search
 *  is done.)
 *
 *  \param  dict
 *  \param  keyword
 *  \return VistaIODictEntry
 */

VistaIODictEntry *VistaIOLookupDictKeyword (VistaIODictEntry * dict, VistaIOStringConst keyword)
{
	if (dict)
		for (; dict->keyword; dict++)
			if (strcmp (keyword, dict->keyword) == 0)
				return dict;
	return NULL;
}


/*! \brief Look up an entry in an attribute dictionary, by value.
 *
 *  Calling sequence:
 *
 *	VistaIOLookupDictValue (VistaIODictEntry *dict, VistaIORepnKind repn, xxx value)
 *
 *  where xxx is a type that corresponds to repn.
 *
 *  \param  dict
 *  \param  repn
 *  \return VistaIODictEntry
 */

VistaIODictEntry *VistaIOLookupDictValue (VistaIODictEntry * dict, VistaIORepnKind repn, ...)
{
	va_list args;
	VistaIOLong i_value = 0;
	VistaIODouble f_value = 0.0;
	VistaIOString s_value = NULL;
	VistaIOBoolean i_valid;

	/* Unravel the arguments passed: */
	if (!dict)
		return NULL;
	va_start (args, repn);
	switch (repn) {
	case VistaIOBitRepn:
		i_value = va_arg (args, VistaIOBitPromoted);
		break;
	case VistaIOUByteRepn:
		i_value = va_arg (args, VistaIOUBytePromoted);
		break;
	case VistaIOSByteRepn:
		i_value = va_arg (args, VistaIOSBytePromoted);
		break;
	case VistaIOShortRepn:
		i_value = va_arg (args, VistaIOShortPromoted);
		break;
	case VistaIOLongRepn:
		i_value = va_arg (args, VistaIOLongPromoted);
		break;
	case VistaIOLong64Repn:
		i_value = va_arg (args, VistaIOLong64);
		break;
	case VistaIOFloatRepn:
		f_value = va_arg (args, VistaIOFloatPromoted);
		break;
	case VistaIODoubleRepn:
		f_value = va_arg (args, VistaIODoublePromoted);
		break;
	case VistaIOBooleanRepn:
		i_value = va_arg (args, VistaIOBooleanPromoted);
		break;
	case VistaIOStringRepn:
		s_value = va_arg (args, VistaIOString);
		break;
	default:
		VistaIOError ("VistaIOLookupDictValue: Can't lookup %s value",
			VistaIORepnName (repn));
	}
	va_end (args);

	/* Search the dictionary by value: */
	switch (repn) {

	case VistaIOBitRepn:
	case VistaIOUByteRepn:
	case VistaIOSByteRepn:
	case VistaIOShortRepn:
	case VistaIOLongRepn:
	case VistaIOLong64Repn:
	case VistaIOBooleanRepn:
		for (; dict->keyword; dict++) {

			/* Is the entry's value only stored as a string? */
			if (dict->svalue && !dict->icached) {

				/* Yes -- try to convert the string to an integer, and
				   cache that value: */
				if (!VistaIODecodeAttrValue
				    (dict->svalue, NULL, VistaIOLongRepn,
				     &dict->ivalue))
					break;
				dict->icached = TRUE;
			}

			/* Test against the integer value stored in the entry: */
			if (i_value == dict->ivalue)
				return dict;
		}
		break;

	case VistaIOFloatRepn:
	case VistaIODoubleRepn:
		for (; dict->keyword; dict++) {

			/* Does the entry include a cached floating point value? */
			if (!dict->fcached) {

				/* No -- obtain it from an integer or string value: */
				if (dict->svalue) {
					if (!VistaIODecodeAttrValue
					    (dict->svalue, NULL, VistaIODoubleRepn,
					     &dict->fvalue))
						break;
				} else
					dict->fvalue = dict->ivalue;
				dict->fcached = TRUE;
			}

			/* Test against the cached float value now stored in the entry: */
			if (f_value == dict->fvalue)
				return dict;
		}
		break;

	case VistaIOStringRepn:

		/* In case we're searching a dictionary with only integer values
		   stored, try to convert the supplied string value to an integer: */
		i_valid =
			VistaIODecodeAttrValue (s_value, NULL, VistaIOLongRepn, &i_value);

		for (; dict->keyword; dict++) {

			/* If the entry includes a string value, compare with it: */
			if (dict->svalue) {
				if (strcmp (s_value, dict->svalue) == 0)
					return dict;
			}

			/* Otherwise, compare with its integer value: */
			else if (i_valid && i_value == dict->ivalue)
				return dict;
		}
		break;

	default:
		break;
	}
	return NULL;
}

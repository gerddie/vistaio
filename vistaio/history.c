/*
* Copyrigh (C) 2004 Max-Planck-Institute of Cognitive Neurosience
*
* The origional VISTA library is copyrighted of University of British Columbia.
* Copyright © 1993, 1994 University of British Columbia.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software 
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* $Id: history.c 885 2006-02-27 10:13:57Z write1 $ */

/*! \file  history.c
 *  \brief basic support for an history object
 *  \remark This file is part of the lipsia and via library, too.
 *  \author Gabi Lohmann, lohmann@cbs.mpg.de, 2004
 */
#define _BSD_SOURCE 

/* From the Vista library: */
#include "vistaio/vistaio.h"

/* From the standard C libaray: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define snprintf _snprintf
#endif


#define MAXPARAMLENGTH 400

char* 
VistaIOGetOptionValue (VistaIOOptionDescRec *option)
{
  char *ret;
  int n, i;
  char *vp;
  VistaIODictEntry *dict;
  VistaIOLong ivalue;
  VistaIODouble fvalue = 0.0;
  VistaIOStringConst svalue;

  ret = (char*)malloc(MAXPARAMLENGTH);


  if (option->number == 0) {
    n = ((VistaIOArgVector *) option->value)->number;
    vp = (char *) ((VistaIOArgVector *) option->value)->vector;
  } else {
    n = option->number;
    vp = (char *) option->value;

  }
  for (i = 0; i < n; i++, vp += VistaIORepnSize (option->repn)) {

    switch (option->repn) {
            
    case VistaIOBitRepn:
      ivalue = * (VistaIOBit *) vp;
      goto PrintLong;
            
    case VUByteRepn:
      ivalue = * (VUByte *) vp;
      goto PrintLong;
            
    case VSByteRepn:
      ivalue = * (VSByte *) vp;
      goto PrintLong;
            
    case VistaIOShortRepn:
      ivalue = * (VistaIOShort *) vp;
      goto PrintLong;
            
    case VistaIOLongRepn:
      ivalue = * (VistaIOLong *) vp;
    PrintLong: 
      snprintf (ret, MAXPARAMLENGTH,"%d", ivalue); 
      break;
            
    case VistaIOFloatRepn:
      fvalue = * (VistaIOFloat *) vp;
      goto PrintDbl;
            
    case VistaIODoubleRepn:
      fvalue = * (VistaIODouble *) vp;
    PrintDbl:   
      snprintf (ret, MAXPARAMLENGTH, "%g", fvalue); 
      break;
            
    case VistaIOBooleanRepn:
	    snprintf (ret,MAXPARAMLENGTH, "%s", * (VistaIOBoolean *) vp ? "true" : "false");
	    break;
    case VistaIOStringRepn:
	    svalue = * (VistaIOString *) vp;
      if (! svalue)
	      svalue = "(none)";
      else if (option->dict &&
               (dict = VistaIOLookupDictValue (option->dict, VistaIODoubleRepn,
                                         svalue)))
	      svalue = dict->keyword;
      snprintf (ret, MAXPARAMLENGTH, "%s", svalue); 
      break;

    default:
      break;
    }
  }

  return ret;
}

char* 
VistaIOGetHistory(int noptions,VistaIOOptionDescRec *options,char *name) 
{
  int i,k;
  char *history;
  char *item;
  char *tok, *tname, *sversion = NULL, *sname;
  char cmd[]="; ";
  char *ignore[] = { "in", "out", "ref", NULL };

  tname = strdup(name);

  tok = strtok(tname, ":");
  sname = strdup(tok);
  if (tok != NULL) {
    tok = strtok(NULL, " ");
    if (tok != NULL) {
      tok = strtok(NULL, " ");
      sversion = strdup(tok);
    } 
    else VistaIOWarning("History function can not grab program version"); 
  }
  else VistaIOWarning("History function can not grab program name"); 
  

  history = (char*) malloc(sizeof(char*) * MAXPARAMLENGTH);
  history[0]='\0';
  strncat(history, sname, strlen(sname));
  strncat(history, " ", 1);
  strncat(history, sversion, strlen(sversion));

  strncat(history, cmd, strlen(cmd));

  for (i = 0; i < noptions; i++, options++) {
    for (k=0; ignore[k] != NULL; k++) {
      if (strncmp(options->keyword, ignore[k], strlen(ignore[k])) == 0) 
	goto out;
    }
    item = VistaIOGetOptionValue (options);
    strncat(history, "-", 1);
    strncat(history, options->keyword, strlen(options->keyword));
    strncat(history, " ", 1);
    strncat(history, item, strlen(item));
    if	(i+1<noptions)
      strncat(history, " ", 1);
  out:
    /* cannot happen */
    break;

  }

  return history;
}

VistaIOAttrList
VistaIOReadHistory(VistaIOAttrList *list)
{
  int i = 0;
  VistaIOStringConst temponame;
  VistaIOAttrListPosn posn;
  VistaIOAttrList history_list=NULL;
  char *temptok, history[]="history";
  VistaIOString str;

  for (VistaIOLastAttr((*list),&posn);VistaIOAttrExists(&posn);VistaIOPrevAttr(&posn)) {

    if (strncmp(VistaIOGetAttrName(&posn), history, strlen(history)) != 0 )
      continue;

    /* old history format by SH */
    if (VistaIOGetAttrRepn(&posn) == VistaIOStringRepn ) {  
      if (history_list==NULL) history_list = VistaIOCreateAttrList();
      VistaIOGetAttrValue(&posn, NULL, VistaIOStringRepn, &str);
      temptok=strtok(str, " ");
      temponame = strdup(temptok);
      temptok = strtok(NULL, "\0");
      VistaIOPrependAttr(history_list, temponame, NULL, VistaIOStringRepn, temptok);
      i++;
    }
    /* new history format */
    if (VistaIOGetAttrRepn(&posn) == VistaIOAttrListRepn ) {
      if (i>0) VistaIOError("type mismatch while reading history");
      VistaIOGetAttrValue(&posn, NULL, VistaIOAttrListRepn, &history_list);
      break;
    }  
  }

  /* Warning */
  if (history_list == NULL) 
    VistaIOWarning("VistaIOReadHistory: No history items found");

  return history_list;
}

void
VistaIOPrependHistory(int noptions,VistaIOOptionDescRec *options,char *name,VistaIOAttrList *list)
{
  char *tok;
  char *newhistory;
  VistaIOStringConst oname;

  /* Generate the new history entry */
  if ((newhistory = VistaIOGetHistory(noptions,options,name)) == NULL)
    VistaIOError("Error while building history string\n");

  tok = strtok(newhistory, " ");
  oname = strdup(tok);
  tok = strtok(NULL, "\0");

  /* Prepend history list */
  if ((*list) == NULL) (*list) = VistaIOCreateAttrList();
  VistaIOPrependAttr( (*list) ,oname, NULL, VistaIOStringRepn, tok); 
}

void  
VistaIOHistory(int noptions,VistaIOOptionDescRec *options,char *name,VistaIOAttrList *in_list,VistaIOAttrList *out_list) 
{
   VistaIOAttrList history_list=NULL;
  VistaIOAttrListPosn posn;
  /*VistaIOBoolean sw=FALSE;*/
  char history[]="history";
  /*VistaIOString str; */

  /* Read history from list */
  history_list = VistaIOReadHistory(in_list);

  /* Prepend new history entry */
  VistaIOPrependHistory(noptions,options,name,&history_list);

  /* DELETE ANY history attributes in dest */
  for (VistaIOLastAttr((*out_list),&posn);VistaIOAttrExists(&posn);VistaIOPrevAttr(&posn)) {
    if (strncmp(VistaIOGetAttrName(&posn), history, strlen(history)) == 0 )
      VistaIODeleteAttr(&posn);
  }

  /* Prepend history in dest */
  VistaIOPrependAttr( (*out_list),history,NULL,VistaIOAttrListRepn,history_list);

}

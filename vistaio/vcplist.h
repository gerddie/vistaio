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

#ifndef __VCPEList_h
#define __VCPEList_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vistaio/vista.h>



#ifdef __cplusplus
extern "C" {
#endif

	/** all eigenvalues are zero */
#define EV_ZERO              0
	/** all eigenvalues are real */
#define EV_REAL              1
	/** all eigenvalues are real and two are equal */
#define EV_REAL_TWO_EQUAL    2
	/** all eigenvalues are real and all are equal */
#define EV_REAL_THREE_EQUAL  3
	/** two eigenvalues are conjugated complex */
#define EV_COMPLEX	     4
	
	/*! \brief VCPERec is the structute to hold a critical point. */	
	typedef struct VCPERec {
		float location[3]; /*!< 3D coordiantes of the critical point */
		float portrait[9]; /*!< Matrix A of the critical point       */
		float eval1;       /*!< first  eigenvalue of critical point this on is always real  */
		float eval2;       /*!< second eigenvalue of critical point or if two eigenvalues are conjugated complex the real part */
		float eval3;       /*!< third  eigenvalue of critical point or if two eigenvalues are conjugated complex imaginary part */ 
		float evec1[3];    /*!< first eigenvector, always real */
		float evec2[3];    /*!< second eigenvector or if two complex eigenvalues then the real part of the eigenvector */ 
		float evec3[3];    /*!< second eigenvector or if two complex eigenvalues then the imaginary part of the eigenvector */
		unsigned char typeM; /*!< type of the eigenvalues */
	}VCPERec;

	/*! \brief A pointer to a single critical point. */	
	typedef  VCPERec *VCPE;
	
	
	/*! \brief Vista data type to hold the list of critical points */	
	typedef struct VFieldCPListRec {
		VLong n_length;  /*!< number of critical points in list */
		VAttrList attr;  /*!< vista attribute list */
		VPointer data;   /*!< pointer to the actual data */
		VLong nsize;     /*!< size of data block in bytes \a data points to */
	} VCPEListRec;
	
	/*! \brief  Definition of the pointer how it is used in the vista library.*/ 
	typedef VCPEListRec *VCPEList;
	
	/** This function creates a critical point list. 
	    \param _n_element number of elements in the list
	    \retval An empty VCPEList with space for \a _n_element critical points
	*/
	VCPEList VCreateCPEList(VLong _n_element);
	
	/** name of the length attribute in the vista file */
#define CPLIST_LENGTH  "n_length"
	/** This function has to be called before using VCPEList objects with original vista functions, 
	    to make the data type available.
	    \remark as a side effect VCPEListRepn is set. 
	*/
	void VRegisterCPEList();

#ifdef __cplusplus
}
#endif
			   
#endif


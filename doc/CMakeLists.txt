
#
#  Copyright 1993, 1994 University of British Columbia
#  Copyright (c) Leipzig, Madrid 2004 - 2012
#  Max-Planck-Institute for Human Cognitive and Brain Science	
#  Max-Planck-Institute for Evolutionary Anthropology 
#  BIT, ETSI Telecomunicacion, UPM
#
#  Permission to use, copy, modify, distribute, and sell this software and its
#  documentation for any purpose is hereby granted without fee, provided that
#  the above copyright notice appears in all copies and that both that
#  copyright notice and this permission notice appear in supporting
#  documentation. UBC makes no representations about the suitability of this
#  software for any purpose. It is provided "as is" without express or
#  implied warranty.
#
#  Maintainance and Fixes: Gert Wollny gw.fossdev@gmail.com , UPM, Madrid  
#

SET(man3pages 
  VistaIOBandInterp.3
  VistaIOBundle.3
  VistaIOCombineBands.3
  VistaIOConst.3
  VistaIOCopy.3
  VistaIOCopyBand.3
  VistaIOCopyImage.3
  VistaIOCreateImage.3
  VistaIODecodeAttrValue.3
  VistaIOEdges.3
  VistaIOError.3
  VistaIOExtractAttr.3
  VistaIOGetAttr.3
  VistaIOGetPixel.3
  VistaIOIdentifyFiles.3
  VistaIOImage.3
  VistaIOInsertAttr.3
  VistaIOList.3
  VistaIOMalloc.3
  VistaIOMax.3
  VistaIONew.3
  VistaIONewString.3
  VistaIONumber.3
  VistaIOOffset.3
  VistaIOOpenFile.3
  VistaIOPackData.3
  VistaIOParseCommand.3
  VistaIOParseFilterCmd.3
  VistaIOPrintOptions.3
  VistaIOPrintParameters.3
  VistaIOReadEdges.3
  VistaIOReadFile.3
  VistaIOReadImages.3
  VistaIOReadObjects.3
  VistaIORepn.3
  VistaIOReportBadArgs.3
  VistaIOReportUsage.3
  VistaIOSameImage.3
  VistaIOSelectBand.3
  VistaIOSelectDestImage.3
  VistaIOSetAttr.3
  VistaIOSetBandInterp.3
  VistaIOSetErrorHandler.3
  VistaIOSetProgramName.3
  VistaIOSetWarningHandler.3
  VistaIOWarning.3
  VistaIOWriteEdges.3
  VistaIOWriteFile.3
  VistaIOWriteImages.3
  VistaIOWriteObjects.3
  VistaIOZeroCrossings.3
  VistaIOattribute.3
  VistaIOdictionary.3
  VistaIOoption.3
  VistaIOtype.3
)

INSTALL(FILES ${man3pages} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man3)

/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLTKDCUCONVERSION_H
#define XMLTKDCUCONVERSION_H

#ifdef DATABASE
#include "DbTkDcuConversionAccess.h"
#endif

#include "XMLCommonFec.h"
//#include "TkDcuConversionMemParseHandlers.h"
#include "MemBufOutputSource.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 */
class XMLTkDcuConversion : public XMLCommonFec {
 private:
  //
  // private attributes
  //
  /**
   * Vector of deviceDescriptions
   */
  dcuConversionVector dVector_;

  /** count the number of conversion factors found
   */
  unsigned int countDCUConversion_ ;

  /** Parameter name's for the parsing
   */
  parameterDescriptionNameType *parameterNames_ ;

  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLTkDcuConversion () noexcept(false);

  /** \brief Constructor with xml buffer
   */
  XMLTkDcuConversion ( const XMLByte* xmlBuffer ) noexcept(false);

#ifdef DATABASE
  /** \brief Constructor with xml buffer
   */
  XMLTkDcuConversion ( DbTkDcuConversionAccess *dbAccess ) noexcept(false);
#endif

  /** \brief Constructor with file access
   */
  XMLTkDcuConversion ( std::string xmlFileName ) noexcept(false);
  
  /** \brief Deletes the XMLTkDcuConversion
   */
  ~XMLTkDcuConversion ();

  /** \brief return the number of CCU found in the buffer XML
   */
  inline unsigned int getCountDCUConversion ( ) { return countDCUConversion_ ; }

  /** \brief Gets a pointer on the device vector private attribute
   */
  dcuConversionVector getDcuConversionVector ( );

  /** \brief Sets the device vector private attribute
   */
  void setDcuConversionVector (dcuConversionVector) noexcept(false);

  /** \brief parse the buffer
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n );

  /** \brief Parse the inputSource and gets a pointer on the device vector private attribute
   */
  dcuConversionVector getDcuConversions ( ) noexcept(false);

  /** Clear the vector
   */
  void clearVector();

#ifdef DATABASE
  void parseDatabaseResponse(tscType32 dcuHardId) noexcept(false);
  void parseDatabaseResponse(std::string partitionName) noexcept(false);

  dcuConversionVector getDcuConversions (tscType32 dcuHardId) noexcept(false);
  dcuConversionVector getDcuConversions (std::string partitionName) noexcept(false);
#endif

  /** \brief Writes the device vector parameter in a ASCII file
   */
  void setDcuConversions ( dcuConversionVector dVector, std::string outputFileName ) noexcept(false);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  void setDcuConversions ( std::string outputFileName ) noexcept(false);

  /** \brief Displays the contents of the device vector attribute
   */
  //void display() noexcept(false);

#ifdef DATABASE
  /** \brief Sends the parameter value from device vector to the database
   */
  void setDcuConversions ( dcuConversionVector dVector ) noexcept(false);

  /** \brief Sends the parameter value from device vector to the database
   */
  void setDcuConversions ( ) noexcept(false);
 
  /** \brief Configures the database
   */
  void dbConfigure() noexcept(false);
#endif


} ;

#endif

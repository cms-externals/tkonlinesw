/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
*/

#ifndef XMLTKIDVSHOSTNAME_H
#define XMLTKIDVSHOSTNAME_H

#ifdef DATABASE
#include "DbTkIdVsHostnameAccess.h"
#endif

#include "deviceType.h"
#include "XMLCommonFec.h"
#include "MemBufOutputSource.h"
#include "ParameterDescription.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).
 *
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 * \warning the pointers allocated by the class should be deleted
 */
class XMLTkIdVsHostname: public XMLCommonFec {

 private:

  //
  // private attributes
  //
  /**
   * Vector of deviceDescriptions
   */
  TkIdVsHostnameVector dVector_;

  /** Count the number of Id VS hostname found
   */
  unsigned int countTkIdVsHostname_ ;

  /** Parameter names for the parsing
   */
  parameterDescriptionNameType *parameterNames_ ;

  public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  XMLTkIdVsHostname () noexcept(false);

  /** \brief Constructor with xml buffer
   */
  XMLTkIdVsHostname ( const XMLByte* xmlBuffer ) noexcept(false);

#ifdef DATABASE
  /** \brief Constructor with database access
   */
  XMLTkIdVsHostname ( DbTkIdVsHostnameAccess *dbAccess )noexcept(false) ;
#endif

  /** \brief Constructor with file access
   */
  XMLTkIdVsHostname ( std::string xmlFileName ) noexcept(false);
  
  /** \brief Deletes the XMLTkIdVsHostname
   */
  ~XMLTkIdVsHostname ();

  /** \brief return the number of DCU Info found in the XML buffer
   */
  inline unsigned int getCountTkIdVsHostname() { return countTkIdVsHostname_ ; }

  /** \brief Gets a pointer on the device vector private attribute
   */
  TkIdVsHostnameVector getTkIdVsHostnameVectorAttribut ( );

  /** \brief Sets the device vector private attribute
   */
  void setTkIdVsHostnameVector (TkIdVsHostnameVector) noexcept(false);

  /** \brief Parse the XML buffer
   */
  unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) ;

  /** \brief clear the vector of elements
   */
  void clearVector();

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  TkIdVsHostnameVector getAllTkIdVsHostnameFromFile (  ) noexcept(false) ;

#ifdef DATABASE
  void parseDatabaseResponse( unsigned int versionMajor, unsigned int versionMinor ) noexcept(false);
  void parseDatabaseResponse( std::string hostname, unsigned int versionMajor, unsigned int versionMinor ) noexcept(false);
  void parseDatabaseResponse( std::string hostname, unsigned int slot, unsigned int versionMajor, unsigned int versionMinor ) noexcept(false);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  TkIdVsHostnameVector getAllTkIdVsHostname ( unsigned int versionMajor = 0, unsigned int versionMinor = 0  ) noexcept(false);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  TkIdVsHostnameVector getAllTkIdFromHostname ( std::string hostname, unsigned int versionMajor = 0, unsigned int versionMinor = 0 ) noexcept(false);

  /** \brief Gets a pointer on the device vector private attribute from database
   */
  TkIdVsHostnameVector getTkIdFromHostnameSlot ( std::string hostname, unsigned int slot, unsigned int versionMajor = 0, unsigned int versionMinor = 0 ) noexcept(false);

#endif


/** \brief Writes the device vector parameter in a ASCII file
   */
  void setTkIdVsHostnameVector ( TkIdVsHostnameVector dVector, std::string outputFileName ) noexcept(false);

  /** \brief Writes the device vector attribute in a ASCII file
   */
  void setTkIdVsHostnameVector ( std::string outputFileName ) noexcept(false);

#ifdef DATABASE
  /** \brief Sends the parameter value from device vector to the database
   */
  void setTkIdVsHostnameVector ( TkIdVsHostnameVector dVector, bool major ) noexcept(false);
#endif
} ;

#endif

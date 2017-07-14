/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3


*/


#ifndef DBACCESS_H
#define DBACCESS_H
#include <cstring>
#include <occi.h>

#include "DbCommonAccess.h"
#include "FecExceptionHandler.h"

/** \brief This virtual class is implemented to handle the communication between the supervisor software and the database.
 *
 * This class provides some features like :
 *  - OCCI environnement and database connection opening and closing 
 *  - download and upload execution
 *  - database version request
 *
 * Private attributes :
 *  - dbEnvironment_ : pointer on an OCCI environnement descriptor
 *  - dbConnection_ : pointer on a database connection descriptor
 *  - xmlClob_ : pointer on a Clob
 */
class DbAccess : public DbCommonAccess {
  // private: 
  //   /** \brief Set the currentStateId as the stateId in the table run */
  //    */ 
  //   virtual void setRun(unsigned int, std::string, unsigned int, std::string) noexcept(false); */

 public:
  //
  // public functions
  //
  /** \brief Default constructor
   */
  DbAccess(bool threaded = false) noexcept(false);

  /** \brief Creates the access to the database
   */
  DbAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) noexcept(false);
  
  /** \brief Closes the connection with the database
   */
  virtual ~DbAccess ( ) noexcept(false);

  /** \brief Get version number
   */
  virtual unsigned int getVersion(std::string sqlQuery) noexcept(false);

  /** \brief Retrives the partitionName from the partition identifier
   */
  virtual std::string getPartitionName ( unsigned int partitionId ) noexcept(false);

  /** \brief Retrives the partition id from a partition name
   */
  virtual unsigned int getPartitionId ( std::string partitionName ) noexcept(false);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClob (std::string partitionName) noexcept(false) = 0 ;

  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) noexcept(false) = 0;

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClob (std::string partitionName, std::string hardId) noexcept(false) = 0;

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobWithVersion (std::string partitionName, std::string hardId, unsigned int versionMajorId, unsigned int versionMinorId) noexcept(false) = 0;

  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName) noexcept(false);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId, unsigned int maskVersionMajor, unsigned int maskVersionMinor) noexcept(false);

  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQueryWithoutMask (std::string readString, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) noexcept(false);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id) noexcept(false);
  
  /** \brief Download parameters from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id, unsigned int versionMajorId, unsigned int versionMinorId) noexcept(false);
  
  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString) noexcept(false);

  /** \brief Download a Clob from the database
   */
  virtual oracle::occi::Clob *getXMLClobFromQuery (std::string readString, std::string partitionName, std::string id, unsigned int ri) noexcept(false);

  /** \brief Upload a Clob from the database
   */
  //  virtual void setXMLClobWithVersion (std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) noexcept(false);

  /** \brief Upload a Clob from the database
   */
  //  virtual void setXMLClob (std::string buffer, std::string partitionName, unsigned int versionUpdate) noexcept(false);

  /** \brief Upload parameters to the database    
   */
  virtual void setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName, unsigned int versionMajorId, unsigned int versionMinorId) noexcept(false);
  
  /** \brief Upload parameters to the database    
   */
  virtual void setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName, int versionUpdate) noexcept(false);
  
  /** \brief Upload parameters to the database    
   */
  virtual unsigned int setXMLClobFromQuery (std::string writeString, std::string buffer, std::string partitionName) noexcept(false);

  /** \brief Upload parameters to the database    
   */
  virtual unsigned int setXMLClobFromQuery (std::string writeString, std::string *buffer, std::string partitionName, bool newPartition) noexcept(false);
  
  /** for SQL tracing
   */
  void traceSql(boolean start);

};
#endif


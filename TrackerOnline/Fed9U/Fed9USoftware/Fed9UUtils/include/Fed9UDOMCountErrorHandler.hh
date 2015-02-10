

/*
 * The Apache Software License, Version 1.1
 * 
 * Copyright (c) 2002 The Apache Software Foundation.  All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The names "Xerces" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache\@apache.org.
 * 
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation, and was
 * originally based on software copyright (c) 1999, International
 * Business Machines, Inc., http://www.ibm.com .  For more information
 * on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef _Fed9UDOMCountErrorHandler_H_
#define _Fed9UDOMCountErrorHandler_H_

//Check if using Xerces version 1
#if XERCES > 1

//Check Xerces version
#include "xercesc/util/XercesVersion.hpp"
#if _XERCES_VERSION >= 20300

#include <xercesc/dom/DOM.hpp>
using xercesc::DOMErrorHandler;
using xercesc::DOMError;

/**
 * \brief Inherited from the DOMErrorHandler this implements the XERCES error handling that is specific
 *        to the Fed9U XML/C++ interface.
 */
class Fed9UDOMCountErrorHandler : public DOMErrorHandler
{
public:
  /**
   * \brief Default constructor. Initialises the class with no errors seen.
   */
  Fed9UDOMCountErrorHandler();

  /**
   * \brief Destructor.
   */
  ~Fed9UDOMCountErrorHandler();

  /**
   * \brief  Used to determine if an error has been seen.
   * \return bool True if an error has been seen.
   */
  bool getSawErrors() const;

  /**
   * \brief  Prints the error message contained within a DOMError.
   * \param  domError DOMError class that is to be printed.
   * \return bool Returns false.
   */
  bool handleError(const DOMError& domError);

  /**
   * \brief Clears the saw errors status.
   */
  void resetErrors();


private :

  /**
   * \name Unimplemented constructors and operators.
   *
   * Copy construction and assignment declared private and unimplemented to override default implementation and
   * to prevent copying or assignment.
   */
  //@{
  Fed9UDOMCountErrorHandler(const Fed9UDOMCountErrorHandler&);
  void operator=(const Fed9UDOMCountErrorHandler&);
  //@}

  bool fSawErrors; ///< This is set if we get any errors, and is queryable via a getter method.
                   ///< It is used by the main code to suppress output if there are errors.
};

#endif

#endif

#endif

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    wrapInstanceTable.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "wrapInstanceTable.h"
#include "wrapException.h"

namespace _wrap_
{


/**
 * Constructor takes interpreter to which the InstanceTable will be
 * attached.  It also initializes the temporary object number to zero.
 */
InstanceTable::InstanceTable(Tcl_Interp* interp):
  m_Interpreter(interp),
  m_TempNameNumber(0)
{
}


/**
 * Set a mapping from "name" to object "object" with type "type".
 * This deletes any instance of an object already having the given name.
 */
void InstanceTable::SetObject(const String& name, void* object,
                              const CvQualifiedType& type)
{
  if(this->Exists(name))
    {
    this->DeleteObject(name);
    }
  m_InstanceMap[name] = Reference(object, type);
  m_AddressToNameMap[object] = name;  
}
  
  
/**
 * Delete the object corresponding to the given name.
 * This looks up the function pointer that was registered for the object's
 * type, and calls it to do the actual deletion.
 */
void InstanceTable::DeleteObject(const String& name)
{
  this->CheckExists(name);
  
  const Type* type = m_InstanceMap[name].GetReferencedType().GetType();
  void* object = m_InstanceMap[name].GetObject();

  // Make sure we know how to delete this object.
  if(m_DeleteFunctionMap.count(type) < 1)
    {
    throw _wrap_UndefinedObjectTypeException(type->Name());
    }
  
  // Remove the object's address from our table.
  m_AddressToNameMap.erase(object);

  // Call the registered delete function.
  m_DeleteFunctionMap[type](object);
  
  // Remove from the instance table.
  m_InstanceMap.erase(name);
 
  // Remove the Tcl command for this instance.
  Tcl_DeleteCommand(m_Interpreter, const_cast<char*>(name.c_str()));
}
  
  
/**
 * Check if there is an object with the given name.
 */
bool InstanceTable::Exists(const String& name) const
{
  return (m_InstanceMap.count(name) > 0);
}


/**
 * Get the Reference holding the object with the given name.
 */
const Reference& InstanceTable::GetEntry(const String& name) const
{
  this->CheckExists(name);
  InstanceMap::const_iterator i = m_InstanceMap.find(name);
  return i->second;
}
 

/**
 * Get a pointer to the object with the given name.
 */
void* InstanceTable::GetObject(const String& name)
{
  this->CheckExists(name);
  return m_InstanceMap[name].GetObject();
}

  
/**
 * Get the type string for the object with the given name.
 */
const CvQualifiedType& InstanceTable::GetType(const String& name)
{
  this->CheckExists(name);
  return m_InstanceMap[name].GetReferencedType();
}

  
/**
 * Allow object type deletion functions to be added.
 */
void InstanceTable::SetDeleteFunction(const Type* type,
                                           DeleteFunction func)
{
  m_DeleteFunctionMap[type] = func;
}


/**
 * Create a unique name for a temporary object, and set the given object
 * to have this name.  The name chosen is returned.
 */
String InstanceTable::CreateTemporary(void* object,
                                      const CvQualifiedType& type)
{
  char tempName[15];
  sprintf(tempName, "__temp%x", m_TempNameNumber++);
  String name = tempName;
  this->SetObject(name, object, type);
  return name;
}


/**
 * If the given object name was generated by InsertTemporary(), the object
 * is deleted.
 */
void InstanceTable::DeleteIfTemporary(const String& name)
{
  this->CheckExists(name);
  if(name.substr(0,6) == "__temp")
    {
    this->DeleteObject(name);
    }
}


/**
 * When an instance deletes itself, this callback is made to remove it from
 * the instance table.
 */
void InstanceTable::DeleteCallBack(void* object)
{
  if(m_AddressToNameMap.count(object) > 0)
    {
    String name = m_AddressToNameMap[object];
    this->DeleteObject(name);
    }
}


/**
 * Make sure an object with the given name exists.
 * Throw an exception if it doesn't exist.
 */
void InstanceTable::CheckExists(const String& name) const
{
  if(!this->Exists(name))
    {
    throw _wrap_UndefinedInstanceNameException(name);
    }
}


} // namespace _wrap_

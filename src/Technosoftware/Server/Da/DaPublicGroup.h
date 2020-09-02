/*
 * Copyright (c) 2020 Technosoftware GmbH. All rights reserved
 * Web: https://technosoftware.com 
 * 
 * The source code in this file is covered under a dual-license scenario:
 *   - Owner of a purchased license: RPL 1.5
 *   - GPL V3: everybody else
 *
 * RPL license terms accompanied with this source code.
 * See https://technosoftware.com/license/RPLv15License.txt
 *
 * GNU General Public License as published by the Free Software Foundation;
 * version 3 of the License are accompanied with this source code.
 * See https://technosoftware.com/license/GPLv3License.txt
 *
 * This source code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef __PUBLICGROUP_H_
#define __PUBLICGROUP_H_

//DOM-IGNORE-BEGIN

#include "DaGenericItem.h"

/////////////////////////////////////////////////////////////////
// The DaPublicGroup class is built to work with the
// DaPublicGroupManager class ... that is once DaPublicGroup is added
// to the DaPublicGroupManager it is frozen and cannot be changed
// anymore ( AddItem will return error )
/////////////////////////////////////////////////////////////////


class DaPublicGroup {
public:

   friend class DaPublicGroupManager;
   friend class DaGenericGroup;
   friend class DaGenericServer;

      // constructor
   DaPublicGroup( void ); 

      // initialize instance with items 
      // the given OPCITEMDEF array must be freed by the caller because
      // the public group makes an internal copy of it
   HRESULT Create( 
               LPWSTR      Name,
               float       PercentDeadband,
               long        LCID,
               BOOL        InitialActive,
               long        *pInitialTimeBias,
               long        InitialUpdateRate,
               DWORD       NumItems,
               OPCITEMDEF  *ItemDefs,
               ITEMDEFEXT  *ExtItemDefs = NULL
      );

      // initialize instance with items
   HRESULT Create( 
               LPWSTR      Name,
               float       PercentDeadband,
               long        LCID,
               BOOL        InitialActive,
               long        *pInitialTimeBias,
               long        InitialUpdateRate
      );

      // destructor
   ~DaPublicGroup( void );

      // add an item to the public group
      // the ItemID must freed by the caller
      // because the server makes an internal copy
   HRESULT AddItem( 
               LPWSTR      ItemID,
               LPWSTR      InitialAccessPath,
               BOOL        InitialActive,
               OPCHANDLE   InitialClientHandle,
               DWORD       InitialBlobSize,
               BYTE       *pInitialBlob,
               VARTYPE     InitialRequestedDataType
      );


// Attributes
public:
      // Do not store the returned pointer !
   inline LPWSTR Name( void ) const {return m_Name;}


private:
      // tells whether the Create method was successfully 
   BOOL m_Created;

      // used from both create functions during initialization
   HRESULT CommonInit(
            LPWSTR      Name,
            float       PercentDeadband,
            long        LCID,
            BOOL        InitialActive,
            long        *pInitialTimeBias,
            long        InitialUpdateRate
         );

   //--------------------------------------------------------------
   //  Attach to class by incrementing the RefCount
   //  Returns the current RefCount
   //--------------------------------------------------------------
   int  Attach( void ); 

   //--------------------------------------------------------------
   //  Detach from class by decrementing the RefCount.
   //  Kill the class instance if request pending.
   //  Returns the current RefCount or -1 if it was killed.
   //--------------------------------------------------------------
   int  Detach( void );

   //--------------------------------------------------------------
   // Kill the public group instance
   // if some thread attached to the public group then
   // the instance will be deleted only when the thread detaches
   //--------------------------------------------------------------
   int  Kill( BOOL WithDetach );

   //--------------------------------------------------------------
   // Tells whether Kill() was called on this instance
   //--------------------------------------------------------------
   BOOL  Killed( void );

   //================================= Members ============================

               /** must be unique among public groups,
                   if an empty string is passed a unique name
                   is generated by the server **/
   LPWSTR m_Name;

               /** see EUType ...**/
   float  m_PercentDeadband;
   
               /** Local Id(iom?) **/
   long   m_LCID;
   
                  /** Fields used for initialisation of client specific
                     COM Group objects! **/


               /** tells whether group is active or not,
                  inactive -> all callbacks stopped 
                  reads return BAD/Out of service **/ 
   BOOL  m_InitialActive;

               /** in minutes? **/
   long  m_InitialTimeBias;

               /** the update rate at which the server
                  can refresh the group data */
               /** in msec (i guess) **/
   long  m_InitialUpdateRate;

               /** this fields are set when a public group
                   is created and do not change during the lifetime
                   of a public group; they are used to execute
                   Server->AddItems for the COM Group **/

               /** the number of item IDs of the group **/
   DWORD m_TotItems;
               // the allocated space for incoming items
   DWORD m_AllocItems;

               /** the array of item Definitions the group contains 
                   it's used for initializing the state of the attaching
                   client-groups (DaGenericGroup) **/
   OPCITEMDEF *m_ItemDefs;

               /** Extension of Item definitions.
                   The array of additional item Definitions the group contains 
                   it's used for initializing the state of the attaching
                   client-groups (DaGenericGroup) **/
   ITEMDEFEXT  *m_ExtItemDefs;

               /** these fields are protected by the critical section
                   of the public group handler **/

               /** how many threads are accessing the public group? **/
   long  m_RefCount;
               /** flag that says that the public group is to kill ...
                   so no new access is accepted and the last thread which
                   releases the object causes its destruction ... **/
   BOOL  m_ToKill;


      //--------------------------------------------------------------
      // The critical section for accessing members of this class  
      //--------------------------------------------------------------
   CRITICAL_SECTION     m_CritSec;

      //--------------------------------------------------------------
      // is not NULL if this public group belongs to a DaPublicGroupManager
      // a public group cannot be added twice to one or more Handlers,
      // also no add item is allowed after group was added to handler
      //--------------------------------------------------------------
   DaPublicGroupManager *m_pOwner;

      //--------------------------------------------------------------
      // The handle contains
      //--------------------------------------------------------------
   long                 m_Handle;

};
//DOM-IGNORE-END

#endif // __PUBLICGROUP_H_
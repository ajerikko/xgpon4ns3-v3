/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University College Cork (UCC), Ireland
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Xiuchao Wu <xw2@cs.ucc.ie>
 */

#ifndef XGPON_SERVICE_RECORD_H
#define XGPON_SERVICE_RECORD_H

#include <cstdlib>
#include <stack>

#include "ns3/simple-ref-count.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The service record of a connection/tcont, such as the time that it was served and the amount of transmitted or granted bandwidth.
 *        Note that this structure is allocated in a pool to avoid many calls of the cpu-intensive malloc.
 *
 */
class XgponServiceRecord : public SimpleRefCount<XgponServiceRecord>
{
  //used to allocate/free this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  /**
   * \brief Constructor
   */
  XgponServiceRecord ();
  virtual ~XgponServiceRecord ();


  /////////////////////////////////////////////////Member variable accessors
  /**
   * \brief set the time that a connection/tcont is served. Unit: nanosecond (since the beginning of the simulation)
   */
  void SetServedTime (uint64_t time);
  /**
   * \brief get the time that a connection/tcont is served. Unit: nanosecond (since the beginning of the simulation)
   */
  uint64_t GetServedTime () const;

  /**
   * \brief set the amount of bytes of a connection/tcont is served. 
   */
  void SetServedBytes (uint32_t bytes);

  /**
   * \brief get the amount of bytes of a connection/tcont is served. 
   */
  uint32_t GetServedBytes () const;






  //////////////////////////////////////Override new and delete to use a pool for avoiding to call malloc/free many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();



private:
  uint64_t  m_servedTime;       //the last time that this connection is served. unit: nanosecond
  uint32_t  m_servedBytes;      //the number of served bytes (bytes transmitted or bytes granted to one connection / tcont) in the last service.


  /* more variables may be needed */




  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponServiceRecord::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }
};





//////////////////////////////////////////////////////INLINE Functions
inline void 
XgponServiceRecord::SetServedTime (uint64_t time)
{
  m_servedTime = time;
}
inline uint64_t 
XgponServiceRecord::GetServedTime () const
{
  return m_servedTime;
}

inline void 
XgponServiceRecord::SetServedBytes (uint32_t bytes)
{
  m_servedBytes = bytes;
}
inline uint32_t
XgponServiceRecord::GetServedBytes () const
{
  return m_servedBytes;
}




}; // namespace ns3

#endif // XGPON_SERVICE_RECORD_H

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

#ifndef XGPON_XGTC_DBRU_H
#define XGPON_XGTC_DBRU_H

#include <cstdlib>
#include <stack>

#include "ns3/simple-ref-count.h"
#include "ns3/buffer.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The buffer occupancy report used by XG-PON for DBA.
 *
 * This class has fields corresponding to those in a buffer occupancy report structure
 * used by XG-PON's DBA scheme. This class also includes the methods for serialization 
 * to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 *
 * Although there is no Alloc-ID in this class, OLT parses it according to the corresponding 
 * bandwidth allocation map, such as the timeslot for this Alloc-ID and the flag for asking 
 * queue occupancy report from the ONU.
 */


class XgponXgtcDbru : public SimpleRefCount<XgponXgtcDbru>
{
  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  const static uint16_t XGPON_XGTC_DBRU_LENGTH = 4;           //unit: byte

  /**
   * \brief Constructor
   */
  XgponXgtcDbru ();
  XgponXgtcDbru (uint32_t bufOcc);
  virtual ~XgponXgtcDbru ();


  ///////////////////////////////////member variables accessors
  void SetBufOcc (uint32_t bufOcc);
  uint32_t GetBufOcc () const;


  void CalculateCrc ();
  bool VerifyCrc () const;


  //////////Copy Constructor
  XgponXgtcDbru& operator=(const XgponXgtcDbru &rhs);





  /////////////////////////////////////////META-data; time related; inline function
  void SetCreateTime (uint64_t time);  //set by ONU
  uint64_t GetCreateTime () const;

  void SetReceiveTime (uint64_t time); //set by OLT
  uint64_t GetReceiveTime () const;




  ///////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();




  ////////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);



private:
  uint32_t  m_bufOcc;         //buffer occupancy. Len: 24 bits or 3 bytes.
  uint8_t   m_crc;            //for the purpose of error detection and correction purpose. Len: 1 byte


  //////////////META-data. unit: nanosecond
  uint64_t  meta_createTime;  //used at ONU-side
  uint64_t  meta_receiveTime; //used at OLT-side




  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponXgtcDbru::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }

};






///////////////////////////////////////////////INLINE Functions

inline void 
XgponXgtcDbru::SetBufOcc (uint32_t bufOcc)
{
  m_bufOcc = bufOcc & 0x00ffffff;  //24bits
}
inline uint32_t 
XgponXgtcDbru::GetBufOcc () const
{
  return m_bufOcc;
}

inline void 
XgponXgtcDbru::CalculateCrc ()
{
  //leave alone for saving CPU
  return;
}
inline bool 
XgponXgtcDbru::VerifyCrc () const
{
  return true;
}






inline void 
XgponXgtcDbru::SetCreateTime (uint64_t time)
{
  meta_createTime = time;
}
inline uint64_t 
XgponXgtcDbru::GetCreateTime () const
{
  return meta_createTime;
}

inline void 
XgponXgtcDbru::SetReceiveTime (uint64_t time)
{
  meta_receiveTime = time;
}
inline uint64_t 
XgponXgtcDbru::GetReceiveTime () const
{
  return meta_receiveTime;
}




}; // namespace ns3

#endif // XGPON_XGTC_DBRU_H

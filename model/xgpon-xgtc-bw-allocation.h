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

#ifndef XGPON_XGTC_BW_ALLOCATION_H
#define XGPON_XGTC_BW_ALLOCATION_H

#include <cstdlib>
#include <stack>

#include "ns3/simple-ref-count.h"
#include "ns3/buffer.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The per T-CONT bandwidth allocation structure used by XG-PON.
 *
 * This class has fields corresponding to those in a bandwidth allocation structure
 * used by XG-PON's DBA scheme. This class also includes the methods for serialization 
 * to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */

#define XGPON_XGTC_BW_ALLOCATION_LENGTH        8           //unit: byte

class XgponXgtcBwAllocation : public SimpleRefCount<XgponXgtcBwAllocation>
{
  //static uint64_t CREATED_BWALLOC_NUM4DEBUG;
  //static uint64_t DELETED_BWALLOC_NUM4DEBUG;

  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  static const uint8_t FLAG_ON = 1;
  static const uint8_t FLAG_OFF = 0;

  /**
   * \brief Constructor
   */
  XgponXgtcBwAllocation ();
  XgponXgtcBwAllocation (uint16_t allocId, bool dbru, bool ploamu, uint16_t startTime, uint16_t grantSize, uint8_t fwi, uint8_t burstProfile);
  virtual ~XgponXgtcBwAllocation ();


  //////////////////////////////////////////////////////member variables accessors (inline functions)
  void SetAllocId (uint16_t allocId);
  uint16_t GetAllocId () const;

  void SetDbruFlag (uint8_t dbru);
  uint8_t GetDbruFlag () const;

  void SetPloamuFlag (uint8_t ploamu);
  uint8_t GetPloamuFlag () const;



  void SetStartTime (uint16_t startTime);
  uint16_t GetStartTime () const;

  void SetGrantSize (uint16_t grantSize);
  uint16_t GetGrantSize () const;


  void SetFwi (uint8_t fwi);
  uint8_t GetFwi () const;

  void SetBurstProfileIndex (uint8_t burstProfile);
  uint8_t GetBurstProfileIndex () const;


  void CalculateHec ();
  bool VerifyHec () const;





  /////////////////////////////////////////META-data; time related; inline function
  void SetCreateTime (uint64_t time);       //set by OLT
  uint64_t GetCreateTime () const;

  void SetReceiveTime (uint64_t time);      //set by ONU
  uint64_t GetReceiveTime () const;




  /////////////////////////////////////////convertion between one allocation and one uint64_t
  uint64_t GetSerializedAllocation (void);
  void DeserializeAllocation (uint64_t sAlloc);





  ////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();

  static uint32_t GetPoolSize() { return m_pool.size(); }






  /////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  uint16_t  m_allocId;        //alloc-id. Len: 14bits.
  uint8_t   m_dbru;           //to indicate whether the ONU should send queue status/ bandwidth request in this allocation. Len: 1 bit
  uint8_t   m_ploamu;         //to indicate whether the ONU should send one PLOAM message in this allocation. Len: 1 bit

  uint16_t  m_startTime;      //start time of this allocation (relative to the start of the upstream frame). Len: 2 bytes. 
  uint16_t  m_grantSize;      //the size of this allocation. Len: 2 bytes. The unit of both start time and grant size is one word (4 bytes).

  uint8_t   m_fwi;            //forward wakeup indication. Used for power saving at ONUs. Len: 1 bit
  uint8_t   m_burstProfile;   //the index of burst profile used by physical adaptation sub-layer. Len: 2 bits
  uint16_t  m_hec;            //for the purpose of error detection and correction purpose. Len: 13 bits


  //////////////META-data. unit: nanosecond. used to clear history. 
  //Note that receive time may be different for various ONUs and is changed multiple times. 
  //But the difference is small and the relative sequence is the same. 
  //It should be fine even when different propagation delays are simulated..
  uint64_t  meta_createTime;  //used at OLT-side
  uint64_t  meta_receiveTime; //used at ONU-side




  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponXgtcBwAllocation::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }


};





///////////////////////////////////////////////INLINE Functions
inline void 
XgponXgtcBwAllocation::SetAllocId (uint16_t allocId)
{
  m_allocId = allocId & 0x3fff;  //14bits
}
inline uint16_t 
XgponXgtcBwAllocation::GetAllocId () const
{
  return m_allocId;
}

inline void 
XgponXgtcBwAllocation::SetDbruFlag (uint8_t dbru)
{
  m_dbru = dbru & 0x01;  //1bits
}
inline uint8_t 
XgponXgtcBwAllocation::GetDbruFlag () const
{
  return m_dbru;
}

inline void 
XgponXgtcBwAllocation::SetPloamuFlag (uint8_t ploamu)
{
  m_ploamu = ploamu & 0x01;  //1bits
}
inline uint8_t 
XgponXgtcBwAllocation::GetPloamuFlag () const
{
  return m_ploamu;
}

inline void 
XgponXgtcBwAllocation::SetStartTime (uint16_t startTime)
{
  m_startTime = startTime;  //16bits
}
inline uint16_t 
XgponXgtcBwAllocation::GetStartTime () const
{
  return m_startTime;
}

inline void 
XgponXgtcBwAllocation::SetGrantSize (uint16_t grantSize)
{
  m_grantSize = grantSize; //16bits
}
inline uint16_t 
XgponXgtcBwAllocation::GetGrantSize () const
{
  return m_grantSize;
}

inline void 
XgponXgtcBwAllocation::SetFwi (uint8_t fwi)
{
  m_fwi = fwi * 0x01;  //1bits
}
inline uint8_t 
XgponXgtcBwAllocation::GetFwi () const
{
  return m_fwi;
}

inline void 
XgponXgtcBwAllocation::SetBurstProfileIndex (uint8_t burstProfile)
{
  m_burstProfile = burstProfile & 0x03;  //2bits
}
inline uint8_t 
XgponXgtcBwAllocation::GetBurstProfileIndex () const
{
  return m_burstProfile;
}


inline void 
XgponXgtcBwAllocation::CalculateHec ()
{
  //leave blank for saving CPU.
}
inline bool 
XgponXgtcBwAllocation::VerifyHec () const
{
  return true;
}






inline void 
XgponXgtcBwAllocation::SetCreateTime (uint64_t time)
{
  meta_createTime = time;
}
inline uint64_t 
XgponXgtcBwAllocation::GetCreateTime () const
{
  return meta_createTime;
}

inline void 
XgponXgtcBwAllocation::SetReceiveTime (uint64_t time)
{
  meta_receiveTime = time;
}
inline uint64_t 
XgponXgtcBwAllocation::GetReceiveTime () const
{
  return meta_receiveTime;
}



}; // namespace ns3

#endif // XGPON_XGTC_BW_ALLOCATION_H

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

#ifndef XGPON_XGTC_BWMAP_H
#define XGPON_XGTC_BWMAP_H

#include <cstdlib>
#include <stack>
#include <deque>

#include "ns3/ptr.h"
#include "ns3/simple-ref-count.h"

#include "xgpon-xgtc-bw-allocation.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The BW-MAP structure used by XG-PON.
 *
 * This class mainly contains a list of XgponXgtcBwAllocation.
 * used by XG-PON's DBA scheme. This class also includes the methods for serialization 
 * to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */

class XgponXgtcBwmap : public SimpleRefCount<XgponXgtcBwAllocation>
{
  //static uint64_t CREATED_BWMAP_NUM4DEBUG;
  //static uint64_t DELETED_BWMAP_NUM4DEBUG;

  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  /**
   * \brief Constructor
   */
  XgponXgtcBwmap ();
  virtual ~XgponXgtcBwmap ();


  //////////////////////////////////////////////////////////////////Main Functions
  void AddOneBwAllocation (const Ptr<XgponXgtcBwAllocation>& bwAlloc);
  void AddOneSerializedBwAllocation (uint64_t sBwAlloc);

  uint16_t GetNumberOfBwAllocation ( ) const;
  const Ptr<XgponXgtcBwAllocation>& GetBwAllocationByIndex (uint16_t index) const;




  //creation time used by OLT to accept bursts based on time
  void SetCreationTime (uint64_t time);
  uint64_t GetCreationTime ( ) const;


  //called by the receiver (ONU) to carry out deserialization
  void SetNumberOfBwAllocation (uint16_t num); 
  


  ///////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();





  //general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  std::deque< Ptr<XgponXgtcBwAllocation> > m_bwAllocations;


  uint16_t meta_allocationNumber;  //META-data: the number of allocations in this map. It is set by a receiver based on one field in the downstream frame header.
                                   //It is mainly used when the receiver needs to deserialize the map.

  uint64_t meta_creationTime;      //META-data: The time that it is created and sent by OLT. It will not be serialized. 
                                   //In LR-PON / XG-PON, multiple downstream frames, hence multiple BW-MAP will be sent during one RTT.
                                   //This field is used by OLT to associate the received upstream burst and its corresponding BW-MAP.
                                   //OLT needs the information in BW-MAP to parse the received burst, such as whether PLOAM message and DBRu exist.




  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponXgtcBwmap::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }

};




///////////////////////////////////////////////INLINE Functions
inline void 
XgponXgtcBwmap::SetCreationTime (uint64_t time)
{
  meta_creationTime = time;
}
inline uint64_t 
XgponXgtcBwmap::GetCreationTime ( ) const
{
  return meta_creationTime;
}



inline void 
XgponXgtcBwmap::SetNumberOfBwAllocation (uint16_t num)
{
  meta_allocationNumber = num;
}
 





}; // namespace ns3

#endif // XGPON_XGTC_BWMAP_H

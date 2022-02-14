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

#ifndef XGPON_XGTC_US_Allocation_H
#define XGPON_XGTC_US_Allocation_H

#include <cstdlib>
#include <stack>
#include <vector>

#include "ns3/simple-ref-count.h"

#include "xgpon-xgem-frame.h"
#include "xgpon-xgtc-dbru.h"

namespace ns3 {


/**
 * \ingroup xgpon
 * \brief The per T-CONT payload in the upstream XGTC burst.
 *
 * This class has fields corresponding to those in the per T-CONT payload of the upstream XGTC burst. 
 * (the buffer occupancy report and an array of XGEM frames that belong to this T-CONT) 
 * as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponXgtcUsAllocation : public SimpleRefCount<XgponXgtcUsAllocation>
{
  //the largest burst size in XG-PON1 is 40Kbytes. Thus, 1000 should be enough even when packet size is very small.
  const static uint32_t XGPON1_MAX_XGEM_FRAMES_PER_US_ALLOCATION = 1000; 

  //static uint64_t CREATED_USALLOC_NUM4DEBUG;
  //static uint64_t DELETED_USALLOC_NUM4DEBUG;

  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  /**
   * \brief Constructor
   */
  XgponXgtcUsAllocation ();
  virtual ~XgponXgtcUsAllocation ();





  /////////////////////////////////////////////////////Main Functions
  /**
   * \brief add a frame to the list
   * \param frame the Xgem frame to be added
   */
  void AddXgemFrame (const Ptr<XgponXgemFrame>& frame);


  /**
   * \return the array (reference) of xgem frames that are payload of this US-ALLOCATION
   */
  std::vector<Ptr<XgponXgemFrame> >& GetXgemFrames (void);


  /**
   * \return the number of xgem frames in this US-ALLOCATION
   */
  uint32_t GetNXgemFrames (void) const;





  //////////////////////////////////////////////member variables accessors
  void SetDbru (const Ptr<XgponXgtcDbru>& dbru);
  void DeleteDbru ( );
  const Ptr<XgponXgtcDbru>& GetDbru () const;
  bool DoesDbruExist () const;


  //called by the receiver (OLT) based on the corresponding BW_MAP for the purpose of deserialization.
  //OLT determines its existence based on the corresponding bwalloc.
  void SetDbruExist (bool exist);





  ///////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();




  ///////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  std::vector<Ptr<XgponXgemFrame> > m_burst;  //The list of packets

  Ptr<XgponXgtcDbru> m_dbru;   //Buffer occupancy report from ONU for the corresponding alloc-id.
                               //the existence of this reported is determined by BW map in the corresponding downstream frame.

  bool meta_dbruExist;         //Whether Dbru exist in this allocation; used for desrialization



  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponXgtcUsAllocation::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }

};




////////////////////////////////////////////////////////INLINE Functions

inline void 
XgponXgtcUsAllocation::AddXgemFrame (const Ptr<XgponXgemFrame>& frame)
{
  m_burst.push_back (frame);
}

inline std::vector<Ptr<XgponXgemFrame> >& 
XgponXgtcUsAllocation::GetXgemFrames (void)
{
  return m_burst;
}

inline uint32_t 
XgponXgtcUsAllocation::GetNXgemFrames (void) const
{
  return m_burst.size();
}




inline void
XgponXgtcUsAllocation::SetDbru (const Ptr<XgponXgtcDbru>& dbru)
{
  m_dbru = dbru;
  meta_dbruExist = true;
}
inline void
XgponXgtcUsAllocation::DeleteDbru ()
{
  m_dbru = 0;
  meta_dbruExist = false;
}
inline const Ptr<XgponXgtcDbru>& 
XgponXgtcUsAllocation::GetDbru () const
{
  return m_dbru;
}
inline bool 
XgponXgtcUsAllocation::DoesDbruExist () const
{
  return meta_dbruExist;
}


inline void 
XgponXgtcUsAllocation::SetDbruExist (bool exist)
{
  meta_dbruExist = exist;
}







}; // namespace ns3

#endif // XGPON_XGTC_US_Allocation_H

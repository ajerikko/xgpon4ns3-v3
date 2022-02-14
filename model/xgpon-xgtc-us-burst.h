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

#ifndef XGPON_XGTC_US_BURST_H
#define XGPON_XGTC_US_BURST_H

#include <deque>

#include "xgpon-xgtc-us-header.h"
#include "xgpon-xgtc-us-allocation.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The upstream XGTC burst of XG-PON.
 *
 * This class has fields corresponding to those in an XGTC upstream burst
 * (the header, the trailer, and an array of allocation that is an array of XGEM frames) 
 * as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponXgtcUsBurst
{
public:

  /**
   * \brief Constructor
   */
  XgponXgtcUsBurst ();
  virtual ~XgponXgtcUsBurst ();



  /*Operations related with US Allocations*/
  void AddUsAllocation (const Ptr<XgponXgtcUsAllocation>& alloc);
  uint16_t GetUsAllocationCount () const;
  const Ptr<XgponXgtcUsAllocation>& GetUsAllocationByIndex (uint16_t index);


  /**
   * \brief get XgtcUsHeader of the upstream burst
   */
  XgponXgtcUsHeader& GetHeader ();   


  /**
   * \brief trailer related operations. leave BLANK for saving CPU.
   */  
  void CalculateTrailer ();
  uint32_t GetTrailer () const;



  ////////////////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);



private:
  XgponXgtcUsHeader m_header;
  std::deque< Ptr<XgponXgtcUsAllocation> > m_allocations;

  uint32_t m_trailer;  //used to estimate BER; when FEC is used, BER should be estimated based on FEC decoding result.
};







//////////////////////////////////////////////////INLINE Functions

inline void 
XgponXgtcUsBurst::AddUsAllocation (const Ptr<XgponXgtcUsAllocation>& alloc)
{
  m_allocations.push_back(alloc);
}
inline uint16_t 
XgponXgtcUsBurst::GetUsAllocationCount () const
{
  return  m_allocations.size();
}
inline const Ptr<XgponXgtcUsAllocation>& 
XgponXgtcUsBurst::GetUsAllocationByIndex (uint16_t index)
{
  NS_ASSERT_MSG((index < m_allocations.size()), "index is too large!!!");
  return m_allocations[index];
}



inline XgponXgtcUsHeader& 
XgponXgtcUsBurst::GetHeader ()
{
  return m_header;
}

inline void 
XgponXgtcUsBurst::CalculateTrailer ()
{
  //leave alone for saving CPU
  return;
}
inline uint32_t 
XgponXgtcUsBurst::GetTrailer () const
{
  return m_trailer;
}






}; // namespace ns3

#endif // XGPON_XGTC_US_Burst_H

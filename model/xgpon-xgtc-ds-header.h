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

#ifndef XGPON_XGTC_DS_HEADER_H
#define XGPON_XGTC_DS_HEADER_H

#include "ns3/buffer.h"

#include "xgpon-xgtc-bwmap.h"
#include "xgpon-xgtc-ploam.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The header for the XGTC downstream frame used by XG-PON.
 *
 * This class has fields corresponding to those in an XGTC downstream frame header as well as
 * methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponXgtcDsHeader
{
public:

  /**
   * \brief Constructor
   */
  XgponXgtcDsHeader ();
  virtual ~XgponXgtcDsHeader ();


  ///////////////////////////////////////////////////Member variable accessors
  bool AddPloam (const Ptr<XgponXgtcPloam>& ploam);
  const Ptr<XgponXgtcPloam>& GetPloamByIndex (uint16_t index) const;
  uint32_t GetPloamCount () const;

  
  void SetBwmap (const Ptr<XgponXgtcBwmap>& map);
  const Ptr<XgponXgtcBwmap>& GetBwmap ( ) const;
  uint32_t GetBwMapLen () const;
 

  void CalculateHec ();
  bool VerifyHec () const;



  //////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  //The following two variables exist in the real header. For simulation, we may refer to the corresponding vectors directly
  //In simulation, they are mainly used for deserialization
  uint32_t  m_bwmapLen;         //the number of allocation structures in BwMap. Len: 11bits
  uint32_t  m_ploamCount;       //the number of PLOAM messages in this frame. Len: 8 bits

  uint32_t  m_hec;              //for the purpose of error detection and correction. Len: 13 bits
                                //4 bytes overall


  Ptr<XgponXgtcBwmap> m_bwmap;  //BW map for DBA

  std::vector< Ptr<XgponXgtcPloam> > m_ploams;        //PLOAM messages
};




////////////////////////////////////////////////////INLINE Functions
inline bool 
XgponXgtcDsHeader::AddPloam (const Ptr<XgponXgtcPloam>& ploam)
{
  NS_ASSERT_MSG((m_ploams.size()<256), "the number of PLOAM message exceeds 256!!!");  
  m_ploams.push_back(ploam);
  return true;
}
inline const Ptr<XgponXgtcPloam>& 
XgponXgtcDsHeader::GetPloamByIndex (uint16_t index) const
{
  NS_ASSERT_MSG((index < m_ploams.size()), "the index is too large!!!");  
  return m_ploams[index];
}
inline uint32_t 
XgponXgtcDsHeader::GetPloamCount () const
{
  return m_ploams.size();
}




inline void 
XgponXgtcDsHeader::SetBwmap (const Ptr<XgponXgtcBwmap>& map)
{
  m_bwmap = map;
}
inline const Ptr<XgponXgtcBwmap>&
XgponXgtcDsHeader::GetBwmap ( ) const
{
  return m_bwmap;
}

inline uint32_t 
XgponXgtcDsHeader::GetBwMapLen () const
{
  return m_bwmap->GetNumberOfBwAllocation();
}





inline void 
XgponXgtcDsHeader::CalculateHec ()
{
  //leave blank for saving CPU
  return;
}
inline bool 
XgponXgtcDsHeader::VerifyHec () const
{
  return true;
}







}; // namespace ns3

#endif // XGPON_XGTC_DS_HEADER_H

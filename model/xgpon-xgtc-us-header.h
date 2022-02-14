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

#ifndef XGPON_XGTC_US_HEADER_H
#define XGPON_XGTC_US_HEADER_H

#include "ns3/ptr.h"

#include "ns3/xgpon-xgtc-ploam.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The header for the XGTC upstream burst used by XG-PON.
 *
 * This class has fields corresponding to those in an XGTC upstream burst header as well as
 * methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponXgtcUsHeader
{
public:

  /**
   * \brief Constructor
   */
  XgponXgtcUsHeader ();
  virtual ~XgponXgtcUsHeader ();


  ///////////////////////////////////////////////////Member variables accessors
  void SetOnuId (uint16_t onuId);
  uint16_t GetOnuId() const;

  void SetPloamQueueStatus (bool nonEmpty); //The MSB bit of m_ind. False---empty, true---nonEmpty
  bool GetPloamQueueStatus () const;

  void SetDgFlag (bool error); //The LSB bit of m_ind, dying gasp. true---error. false---normal
  bool GetDgFlag () const;



  /**
   * \brief Set Ploam message in the header. Note that there is at most one PLOAM message per upstream burst
   * \param ploam the Ploam messge in this header
   */
  void SetPloam (const Ptr<XgponXgtcPloam>& ploam); 

  /**
   * \brief Get Ploam message in the header.
   * \return the Ploam messge in this header (0 indicates that there is no Ploam message)
   */
  const Ptr<XgponXgtcPloam>& GetPloam () const; 


  //meta-info used for deserialization
  bool DoesPloamExist () const;
  void SetPloamExist (bool exist);


 
  //HEC related operations. leave BLANK for saving CPU
  void CalculateHec ();
  bool VerifyHec () const;





  /////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  uint16_t  m_onuId;             //ONU-ID. Len: 10bits
  uint16_t  m_ind;               //Flags. Len: 9 bits; MSB---ploam exist at ONU; LSB---dying gasp status at ONU.

  uint16_t  m_hec;               //for the purpose of error detection and correction. Len: 13 bits
                                 //4 bytes overall


  Ptr<XgponXgtcPloam>  m_ploam;  //PLOAM message
  bool meta_ploamExist;          //META-data: whether one PLOAM message is included in this upstream burst. used for deserialization at OLT-side.
                                 //At OLT side, the existence of PLOAM is determined by BW Map of the corresponding downstream frame

};







////////////////////////////////////////////////////////////INLINE Functions

inline void 
XgponXgtcUsHeader::SetOnuId (uint16_t onuId)
{
  m_onuId = onuId & 0x03ff;  //10bits
}
inline uint16_t
XgponXgtcUsHeader::GetOnuId() const
{
  return m_onuId;
}

inline void 
XgponXgtcUsHeader::SetPloamQueueStatus (bool nonEmpty) //The MSB bit of m_ind
{
  if(nonEmpty) m_ind = 0x0100 | m_ind;
  else m_ind = 0x0000 | m_ind;
}
inline bool 
XgponXgtcUsHeader::GetPloamQueueStatus () const
{
  uint16_t tmp;
  tmp = m_ind & 0x0100;
  if(tmp == 0) return false;
  else return true;
}

inline void 
XgponXgtcUsHeader::SetDgFlag (bool error) //The LSB bit of m_ind
{
  if(error) m_ind = 0x0001 | m_ind;
  else m_ind = 0x0000 | m_ind;
}
inline bool 
XgponXgtcUsHeader::GetDgFlag () const
{
  uint16_t tmp;
  tmp = m_ind & 0x0001;
  if(tmp == 0) return false;
  else return true;
}


inline void 
XgponXgtcUsHeader::SetPloam (const Ptr<XgponXgtcPloam>& ploam)
{
  m_ploam = ploam;
  meta_ploamExist = true;
}
inline const Ptr<XgponXgtcPloam>& 
XgponXgtcUsHeader::GetPloam () const
{
  return m_ploam;
}


inline bool 
XgponXgtcUsHeader::DoesPloamExist () const
{
  return meta_ploamExist;
}
inline void XgponXgtcUsHeader::SetPloamExist (bool exist)
{
  meta_ploamExist = exist;
}




inline void 
XgponXgtcUsHeader::CalculateHec () 
{
  //leave blank for saving CPU.
}
inline bool 
XgponXgtcUsHeader::VerifyHec () const
{
  return true;
}





}; // namespace ns3

#endif // XGPON_XGTC_US_HEADER_H

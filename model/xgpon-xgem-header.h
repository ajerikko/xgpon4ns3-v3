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

#ifndef XGPON_XGEM_HEADER_H
#define XGPON_XGEM_HEADER_H

#include "ns3/buffer.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The header for the XGEM frame used by XG-PON for encapsulating SDU from upper layers.
 *
 * This class has fields corresponding to those in an XGEM header as well as
 * methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */

class XgponXgemHeader
{
public:
  static const uint8_t XGPON_XGEM_HEADER_LENGTH = 8;           //unit: byte

  /**
   * \brief Constructor
   */
  XgponXgemHeader ();
  XgponXgemHeader (uint16_t pli, uint8_t keyIndex, uint16_t xgemPortId, uint32_t options, uint8_t lastFragment);
  virtual ~XgponXgemHeader ();


  //////////////////////////////////member variable accessors (inline functions)
  void SetPli (uint16_t payloadLen);
  uint16_t GetPli () const;

  void SetKeyIndex (uint8_t keyIndex);
  uint8_t GetKeyIndex () const;

  void SetXgemPortId (uint16_t xgemPortId);
  uint16_t GetXgemPortId () const;

  void SetOptions (uint32_t options);
  uint32_t GetOptions () const;

  void SetLastFragmentFlag (uint8_t lastFragment);
  uint8_t GetLastFragmentFlag () const;

  //inline functions since they will be empty for saving CPU.
  void CalculateHec ();
  bool VerifyHec () const;



  /*Functions for (de)serialization. We can just implement "GetSerializedSize" in the first phase.*/
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  uint16_t  m_pli;         //payload length indicator. Len: 14bits
  uint8_t   m_keyIndex;    //the index of the encryption key. Len: 2 bits
  uint16_t  m_xgemPortId;  //XGEM port id. Len: 16 bits
  uint32_t  m_options;     //Options. Len: 18 bits; default value is 0x00000
  uint8_t   m_lastFragment;//Last Fragment. Len: 1 bit;  1: The last Fragment or a complete SDU; 0: Fragment
  uint16_t  m_hec;         //Hybrid error correction. Len: 13 bits

};





////////////////////////////////////////////////////////INLINE Functions

inline void 
XgponXgemHeader::SetPli (uint16_t payloadLen)
{
  m_pli = payloadLen & 0x3fff;  //14bits
}
inline uint16_t 
XgponXgemHeader::GetPli () const
{
  return m_pli;
}

inline void 
XgponXgemHeader::SetKeyIndex (uint8_t keyIndex)
{
  m_keyIndex = keyIndex & 0x03;  //2bits
}
inline uint8_t 
XgponXgemHeader::GetKeyIndex () const
{
  return m_keyIndex;
}

inline void 
XgponXgemHeader::SetXgemPortId (uint16_t xgemPortId)
{
  m_xgemPortId = xgemPortId;  //16bits
}
inline uint16_t 
XgponXgemHeader::GetXgemPortId () const
{
  return m_xgemPortId;
}

inline void 
XgponXgemHeader::SetOptions (uint32_t options)
{
  m_options = options & 0x3ffff;  //18bits
}
inline uint32_t 
XgponXgemHeader::GetOptions () const
{
  return m_options;
}

inline void 
XgponXgemHeader::SetLastFragmentFlag (uint8_t lastFragment)
{
  m_lastFragment = lastFragment & 0x01;  //1bit
}
inline uint8_t 
XgponXgemHeader::GetLastFragmentFlag () const
{
  return m_lastFragment;
}


inline void 
XgponXgemHeader::CalculateHec ()
{
  //won't calculate for simulation speed 
  //uint16_t hec;
  //m_hec = hec & 0x1fff;  //13its
}
inline  bool 
XgponXgemHeader::VerifyHec () const
{
  return true;
}




}; // namespace ns3

#endif // XGPON_XGEM_HEADER_H

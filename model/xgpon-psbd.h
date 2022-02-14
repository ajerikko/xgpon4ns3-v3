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

#ifndef XGPON_PSBD_H
#define XGPON_PSBD_H

#include "ns3/buffer.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The physical synchronization block of the XGPON downstream frame.
 *
 * This class has the corresponding fields as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */


class XgponPsbd
{
public:

  /**
   * \brief Constructor
   */
  XgponPsbd ();
  virtual ~XgponPsbd ();


  ////////////////////////////////////////member variables operators
  void SetPsync (uint64_t psync);
  uint64_t GetPsync () const;

  void SetSfc (uint64_t sfc);
  uint64_t GetSfc () const;

  void SetPonId (uint64_t ponId);
  uint64_t GetPonId () const;

  void CalculateSfcHec (uint64_t sfc);
  bool VerifySfcHec ( ) const;

  void CalculatePonIdHec (uint64_t ponId);
  bool VerifyPonIdHec ( ) const;



  ///////////////////////////////////Copy
  XgponPsbd& operator=(const XgponPsbd &rhs);



  ///////////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);


private:
  uint64_t  m_psync;       //64bits
  uint64_t  m_sfc;         //51bits
  uint64_t  m_sfcHec;      //13bits
  uint64_t  m_ponId;       //51bits
  uint64_t  m_ponIdHec;    //13bits

};





///////////////////////////////////////////INLINE functions

inline void 
XgponPsbd::SetPsync (uint64_t psync)
{
  m_psync = psync;
}
inline uint64_t 
XgponPsbd::GetPsync ( ) const
{
  return m_psync;
}

inline void 
XgponPsbd::SetSfc (uint64_t sfc)
{
  m_sfc = sfc & 0x0007ffffffffffff;
}
inline uint64_t 
XgponPsbd::GetSfc ( ) const
{
  return m_sfc;
}

inline void 
XgponPsbd::SetPonId (uint64_t ponId)
{
  m_ponId = ponId & 0x0007ffffffffffff;
}
inline uint64_t 
XgponPsbd::GetPonId ( ) const
{
  return m_ponId;
}

inline void 
XgponPsbd::CalculateSfcHec (uint64_t sfc)
{
  m_sfcHec=0;
  return;
}
inline bool 
XgponPsbd::VerifySfcHec ( ) const
{
  return true;
}

inline void 
XgponPsbd::CalculatePonIdHec (uint64_t ponId)
{
  return;
}
inline bool 
XgponPsbd::VerifyPonIdHec ( ) const
{
  return true;
}





}; // namespace ns3

#endif // XGPON_PSBU_H

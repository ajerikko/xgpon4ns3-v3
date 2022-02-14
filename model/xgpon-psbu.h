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

#ifndef XGPON_PSBU_H
#define XGPON_PSBU_H

#include "ns3/buffer.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The upstream physical synchronization block the XGPON upstream burst. The length of preamble and delimiter will be determined by the corresponding burst profile.
 *
 * This class has the corresponding fields as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */


class XgponPsbu
{
public:

  static const uint8_t XGPON_PSBU_MAX_LEN             = 100;  
  static const uint8_t XGPON_PSBU_PREAMBLE_DEFAULT_LEN = 20;   //160bits = 20bytes
  static const uint8_t XGPON_PSBU_DELIMITER_DEFAULT_LEN = 4;   //32bits = 4bytes

  /**
   * \brief Constructor
   */
  XgponPsbu ();
  virtual ~XgponPsbu ();


  /////////////////////////////////////////////////////////Main Functions

  //fill preamble
  void SetPreamble (const uint8_t *preamble, uint8_t len);

  //copy preamble out and return the length of preamble 
  uint8_t GetPreamble (uint8_t *dst) const;  //return the len


  //fill delimiter
  void SetDelimiter (const uint8_t *delimiter, uint8_t len);

  //copy delimiter out and return the length of delimiter 
  uint8_t GetDelimiter (uint8_t *dst) const;


  uint8_t GetPreambleLength ( ) const;
  uint8_t GetDelimiterLength ( ) const;



  XgponPsbu & operator=(const XgponPsbu &rhs);




  /////////////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t  meta_preambleLen;                  //META-data: preamble length
  uint8_t  meta_delimiterLen;                 //META-data: delimiter length

  uint8_t  m_preamble[XGPON_PSBU_MAX_LEN];
  uint8_t  m_delimiter[XGPON_PSBU_MAX_LEN];

};



////////////////////////////////////INLINE Functions
inline uint8_t 
XgponPsbu::GetPreambleLength ( ) const
{
  return meta_preambleLen;
}

inline uint8_t 
XgponPsbu::GetDelimiterLength ( ) const
{
  return meta_delimiterLen;
}



}; // namespace ns3

#endif // XGPON_PSBU_H

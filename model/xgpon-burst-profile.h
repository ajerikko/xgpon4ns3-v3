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

#ifndef XGPON_BURST_PROFILE_H
#define XGPON_BURST_PROFILE_H

#include "ns3/object.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The burst profile (physical layer related information) that is used by ONU to transmit upstream bursts.
 */

class XgponBurstProfile : public Object
{
public:

  static const uint8_t PSBU_PREAMBLE_DEFAULT_LEN = 20;   //160bits = 20bytes
  static const uint8_t PSBU_DELIMITER_DEFAULT_LEN = 4;   //32bits = 4bytes

  /**
   * \brief Constructor
   */
  XgponBurstProfile ();
  virtual ~XgponBurstProfile ();


  ///////////////////////////////////////////member variable accessors
  uint8_t GetPreambleLen ();
  void SetPreambleLen (uint8_t len);

  uint8_t GetDelimiterLen ();
  void SetDelimiterLen (uint8_t len);

  bool GetFec ();
  void SetFec (bool fecOrNot);



  //called by the helper to configure the same profile at both OLT and ONU.
  void DeepCopy(const Ptr<XgponBurstProfile>& profile);



  ///////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

private:
  uint8_t  m_preambleLen;                //preamble length. unit: byte;
  uint8_t  m_delimiterLen;               //delimiter length. unit: byte
  bool     m_fec;                        //whether to enanle FEC
};










/////////////////////////////////////////////////////////INLINE Functions
inline uint8_t 
XgponBurstProfile::GetPreambleLen ()
{
  return m_preambleLen;
}
inline void 
XgponBurstProfile::SetPreambleLen (uint8_t len)
{
  m_preambleLen = len;
}

inline uint8_t 
XgponBurstProfile::GetDelimiterLen ()
{
  return m_delimiterLen;
}
inline void 
XgponBurstProfile::SetDelimiterLen (uint8_t len)
{
  m_delimiterLen = len;
}

inline bool 
XgponBurstProfile::GetFec ()
{
  return m_fec;
}
inline void 
XgponBurstProfile::SetFec (bool fecOrNot)
{
  m_fec = fecOrNot;
}





}; // namespace ns3

#endif // XGPON_BURST_PROFILE_H

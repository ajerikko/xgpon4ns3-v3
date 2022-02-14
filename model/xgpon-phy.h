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

#ifndef XGPON_PHY_H
#define XGPON_PHY_H

#include "ns3/object.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The parameters and common routines of XG-PON PHY layer. 
 * TODO:  Link rates should be changed from uint32_t to uint64_t and their unit should be changed to bit per second in the future to hold 40/100Gbps network
 *        Since attribute may not support uint64_t, we may change the unit to word per second or even larger, etc.
 */
class XgponPhy : public Object
{
  const static uint32_t XGPON1_DS_LINE_RATE = 1244160000;       //unit: Byte per second. 9953280000 bps (around 10Gbps)
  const static uint32_t XGPON1_US_LINE_RATE = 311040000;        //unit: Byte per second. 2488320000 bps (around 2.5Gbps)

  const static uint32_t XGPON1_DS_FRAME_SLOT_SIZE = 125000;     //unit: nano-second
  const static uint32_t XGPON1_DS_PSB_SIZE = 24;                //unit: byte

  const static uint32_t XGPON1_MINIMUM_GUARD_TIME = 2;          //unit: word (4 bytes per word)

  const static uint32_t XGPON1_FEC_BLOCK_SIZE = 248;            //unit: byte
  const static uint32_t XGPON1_DS_FEC_DATA_SIZE = 216;          //unit: byte
  const static uint32_t XGPON1_US_FEC_DATA_SIZE = 232;          //unit: byte

public:

  /**
   * \brief Constructor
   */
  XgponPhy ();
  virtual ~XgponPhy ();



  ///////////////////////////////////////////////Frame Size
  /**
   * \brief Get the length of a downstream frame. Unit: bytes. 
   *        Note that the physical layer overhead and FEC overhead are excluded. 
   * \return the length of XgponXgtcDsFrame in byte
   */
  uint32_t GetXgtcDsFrameSize ( );

  /**
   * \brief Get the length of upstream phy frame. Unit: word (4 bytes). used by DBA engine  
   * \return the length of upstream phy frame in word
   */
  uint32_t GetUsPhyFrameSizeInWord ( );



  ///////////////////////////////////////////////////XGPON-1 related member variable accessors
  ///////////////////////////////////////////////////Note that these variables should be set through attributes
  //Downstream Link Rate.  Unit: byte per second
  uint32_t GetDsLinkRate ( ) const;

  //Downstream frame time-slot length. Unit: nano-second
  uint32_t GetDsFrameSlotSize ( ) const;
  
  //Downstream PSB size. Unit: byte
  uint16_t GetDsPsbSize ( ) const;

  //Upstream Link Rate. Unit: byte per second
  uint32_t GetUsLinkRate ( ) const;

  //Guard time between consecutive upstream bursts. Unit: word (4 bytes)
  uint16_t GetUsMinimumGuardTime ( ) const;


  //FEC related information. Unit: byte
  uint16_t GetDsFecBlockDataSize ( ) const;
  uint16_t GetDsFecBlockSize ( ) const;
  uint16_t GetUsFecBlockDataSize ( ) const;
  uint16_t GetUsFecBlockSize ( ) const;


  /////////////////////////////////////////////Required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:
  //member variables to hold XGPON-1 parameters
  uint32_t m_dsLinkRate;          //the supported data rate in downstream direction. Unit: byte per second
  uint32_t m_dsFrameSlotSize;     //the duration of each downstream frame. Unit: nano-second.
  uint16_t m_dsPsbSize;           //the PSB size of each downstream frame. Unit: byte.

  uint32_t m_dsXgtcFrameSize;     //the length of the XGTC frame, plo and fec overhead excluded. Unit: bytes. 

  uint32_t m_usLinkRate;          //the supported data rate in upstream direction. Unit: byte per second
  uint32_t m_usMinimalGuardTime;  //the size of upstream PHY frame in word (4 byte).

  uint32_t m_usPhyFrameSize;      //the size of upstream PHY frame in word (4 byte).

  uint16_t m_dsFecBlockSize;    //FEC related variables
  uint16_t m_dsFecBlockDataSize; 
  uint16_t m_usFecBlockSize; 
  uint16_t m_usFecBlockDataSize;   

private:
  //calculate the size of XgponXgtcDsFrame in byte. The physical layer overhead and FEC overhead are excluded. 
  uint32_t CalculateXgtcDsFrameSize ( );

  //calculate the size of upstream phy frame in word (4 byte). used by dba engines.
  uint32_t CalculateUsPhyFrameSize ( );


  /////////////////////////////////////////////////Functions for FEC and line coding
  //Ptr<PonFrame> FecEncode (Ptr<PonFrame> frame);   
  //Ptr<PonFrame> FecDecode (Ptr<PonFrame> frame);
  //Ptr<PonFrame> LineEncode (Ptr<PonFrame> frame);
  //Ptr<PonFrame> LineDecode (Ptr<PonFrame> frame);
};




/////////////////////////////////////////////////INLINE functions

inline uint32_t 
XgponPhy::GetXgtcDsFrameSize ( ) 
{
  if(m_dsXgtcFrameSize==0) { m_dsXgtcFrameSize = CalculateXgtcDsFrameSize ( ); }

  return m_dsXgtcFrameSize;
}


inline uint32_t 
XgponPhy::GetUsPhyFrameSizeInWord ( )
{
  if(m_usPhyFrameSize==0) { m_usPhyFrameSize = CalculateUsPhyFrameSize ( ); }

  return m_usPhyFrameSize;
}






inline uint32_t 
XgponPhy::GetDsLinkRate ( ) const
{
  return m_dsLinkRate;
}

inline uint32_t
XgponPhy::GetDsFrameSlotSize ( ) const
{
  return m_dsFrameSlotSize;
}

inline uint16_t 
XgponPhy::GetDsPsbSize ( ) const
{
  return m_dsPsbSize;
}


inline uint32_t 
XgponPhy::GetUsLinkRate ( ) const
{
  return m_usLinkRate;
}

inline uint16_t 
XgponPhy::GetUsMinimumGuardTime ( ) const
{
  return m_usMinimalGuardTime;
}




inline uint16_t 
XgponPhy::GetDsFecBlockDataSize ( ) const
{
  return m_dsFecBlockDataSize;
}

inline uint16_t 
XgponPhy::GetDsFecBlockSize ( ) const
{
  return m_dsFecBlockSize;
}

inline uint16_t 
XgponPhy::GetUsFecBlockDataSize ( ) const
{
  return m_usFecBlockDataSize; 
}

inline uint16_t 
XgponPhy::GetUsFecBlockSize ( ) const
{
  return m_usFecBlockSize;
}


}; // namespace ns3

#endif // XGPON_PHY_H

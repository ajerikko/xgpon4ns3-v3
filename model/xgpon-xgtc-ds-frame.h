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

#ifndef XGPON_XGTC_DS_FRAME_H
#define XGPON_XGTC_DS_FRAME_H

#include <vector>

#include "xgpon-xgem-frame.h"
#include "xgpon-xgtc-ds-header.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The downstream XGTC frame of XG-PON.
 *
 * This class has fields corresponding to those in an XGTC downstream frame (the header and an array of packets/XGEM frames) 
 * as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponXgtcDsFrame
{
  //the downstream frame size in XG-PON1 is 160Kbytes. Thus, 4000 should be enough even when packet size is very small.
  const static uint32_t XGPON1_MAX_XGEM_FRAMES_PER_DS_FRAME = 4000; 

  //broadcast traffic is at most 10% of the downstream traffics
  const static uint32_t XGPON1_MAX_BROADCAST_XGEM_FRAMES_PER_DS_FRAME = 400; 

public:
  /**
   * \brief Constructor
   */
  XgponXgtcDsFrame ();
  virtual ~XgponXgtcDsFrame ();


  /////////////////////////////////////////////////////////Xgem Frames related functions
  /**
   * \brief add a frame to the list of unicast xgem frame
   * \param frame the xgem frame to be added
   */
  void AddUnicastXgemFrame (const Ptr<XgponXgemFrame>& frame);

  /**
   * \brief add a frame to the list of broadcast xgem frame
   * \param frame the broadcast xgem frame to be added
   */
  void AddBroadcastXgemFrame (const Ptr<XgponXgemFrame>& frame);


  /**
   * \return the list of unicast frames of this downstream frame
   */
  std::vector<Ptr<XgponXgemFrame> >& GetUnicastXgemFrames (void);

  /**
   * \return the list of broadcast frames of this downstream frame
   */
  std::vector<Ptr<XgponXgemFrame> >& GetBroadcastXgemFrames (void);


  /**
   * \return the number of unicast frames in the downstream frame
   */
  uint32_t GetNUnicastXgemFrames (void) const;


  /**
   * \return the number of broadcast frames in the downstream frame
   */
  uint32_t GetNBroadcastXgemFrames (void) const;





  /**
   * \return the bitmap that specifies which onu is served in this downstream frame
   */
  std::vector<uint8_t>& GetBitmap (void);



  ////////////////////////////////////////////member variables accessors
  XgponXgtcDsHeader& GetHeader ();





  //called by the receiver (ONU) to decide how to deserialize the packet burst.
  //the parameter is the overall size of the xgtc downstream frame
  void SetBurstSize (uint32_t size);  




  /////////////////////////general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);



private:    
  std::vector<Ptr<XgponXgemFrame> > m_burst;           //The list of xgem frames. vector + reservation are used to save CPU.

  std::vector<Ptr<XgponXgemFrame> > m_broadcastBurst;  //The list of xgem frames for broadcast traffics. vector + reservation are used to save CPU.

  XgponXgtcDsHeader m_header;

  //META-data: the burst size (header included) set by the receiver. It is used to determine the end of deserialization.
  uint32_t  meta_burstSize;  

  std::vector<uint8_t> m_bitmap;                      //The bitmap used to specify which onu is served in the unicast xgem frames;

};




////////////////////////////////////////////////////INLINE Functions
inline XgponXgtcDsHeader& 
XgponXgtcDsFrame::GetHeader ()
{
  return m_header;
}


inline void 
XgponXgtcDsFrame::SetBurstSize (uint32_t size)
{
  meta_burstSize = size;
}


inline void 
XgponXgtcDsFrame::AddUnicastXgemFrame (const Ptr<XgponXgemFrame>& frame)
{
  m_burst.push_back(frame);
}

inline void 
XgponXgtcDsFrame::AddBroadcastXgemFrame (const Ptr<XgponXgemFrame>& frame)
{
  m_broadcastBurst.push_back(frame);
}


inline std::vector<Ptr<XgponXgemFrame> >& 
XgponXgtcDsFrame::GetUnicastXgemFrames (void)
{
  return m_burst;
}

inline std::vector<Ptr<XgponXgemFrame> >& 
XgponXgtcDsFrame::GetBroadcastXgemFrames (void)
{
  return m_broadcastBurst;
}


inline uint32_t 
XgponXgtcDsFrame::GetNUnicastXgemFrames (void) const
{
  return m_burst.size();
}


inline uint32_t 
XgponXgtcDsFrame::GetNBroadcastXgemFrames (void) const
{
  return m_burst.size();
}


inline std::vector<uint8_t>& 
XgponXgtcDsFrame::GetBitmap (void)
{
  return m_bitmap;
}





}; // namespace ns3

#endif // XGPON_XGTC_DS_FRAME_H

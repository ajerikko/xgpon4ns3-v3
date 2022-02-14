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

#ifndef XGPON_CONNECTION_RECEIVER_H
#define XGPON_CONNECTION_RECEIVER_H

#include "ns3/packet.h"

#include "xgpon-connection.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The connection maintained at receiver-side. Its main function is to reassemble the segmentations of the same SDU.
 *
 */
class XgponConnectionReceiver : public XgponConnection
{
public:

  /**
   * \brief Constructor
   */
  XgponConnectionReceiver ();
  virtual ~XgponConnectionReceiver ();



  //////////////////////////////////////////////Member variable accessors

  /**
   * \brief put back the received fragments for further reassemble. 
   */  
  void SetPacket4Reassemble (const Ptr<Packet>& pkt);

  /**
   * \brief    get the received segments to carry out reassemble. 
   * \return   the segments received previously. 0: have not received any segment.
   *           Note that we cannot return one reference since m_pkt4Reassemble need be set to null
   *           since the received segment has been passed to the caller.
   */  
  const Ptr<Packet> GetPacket4Reassemble ( );



  //////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  Ptr<Packet> m_pkt4Reassemble;  //used to hold the packet to be reassembled further.

};





////////////////////////////////////////////////INLINE Functions
inline void 
XgponConnectionReceiver::SetPacket4Reassemble (const Ptr<Packet>& pkt)
{
  NS_ASSERT_MSG((pkt!=0), "The segment to be reassembled later is NULL!!!");
  m_pkt4Reassemble = pkt;
}
inline const Ptr<Packet>
XgponConnectionReceiver::GetPacket4Reassemble () 
{
  Ptr<Packet> pkt = m_pkt4Reassemble;
  m_pkt4Reassemble = 0;

  return pkt;
}






}; // namespace ns3

#endif // XGPON_CONNECTION_RECEIVER_H

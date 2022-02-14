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

#ifndef XGPON_XGEM_ENGINE_H
#define XGPON_XGEM_ENGINE_H

#include <math.h>

#include "ns3/object.h"
#include "ns3/packet.h"

#include "xgpon-xgem-header.h"
#include "xgpon-xgem-frame.h"
#include "xgpon-connection-sender.h"
#include "xgpon-key.h"



namespace ns3 {

class XgponNetDevice;

/**
 * \ingroup xgpon
 * \brief The class is just used to organize the codes for XGEM encapsulation and segmentation/reassemble. 
 *
 */
class XgponXgemRoutines
{
public:

  const static uint16_t XGPON_XGEM_FRAME_MAXLEN = 16388;       //header included (8 + 16380). Note that 16380 < 16383 (the maximal value of the 14bit PLI)
  const static uint16_t XGPON_IDLE_XGEM_PORT_ID = 0xFFFF;      //xgem port id used by one idle frame




  /**
   * \brief  generate one xgem frame for transmission.
   * \return the resulting xgem frame
   *
   * \param device the corresponding xgpon-net-device that is generating this frame. (used for tracing.)
   * \param conn the connection whose data will be transmitted.
   * \param maxLen the available space in XGTC frame or its fair-share of bandwidth to be granted.
   * \param key the key is used to encrypt the payload of xgem frame (unused in our model).
   * \param keyIndex the index of the current key for encryption
   * \param doSegmentation whether to carry out segmentation if SDU is larger than the available space for this connection.
   *        it should be true only when this connection is the last one for the current downstream frame or upstream Allocation.
   */
  static Ptr<XgponXgemFrame> GenerateXgemFrame (const Ptr<XgponNetDevice>& device, const Ptr<XgponConnectionSender>& conn, uint32_t maxLen, const Ptr<XgponKey>& key, uint8_t keyIndex, bool doSegmentation);    


  /**
   * \brief generate one idle xgem frame;
   * \return the idle xgem frame
   * \param the size of this idle xgem frame (both header and padding included)
   */
  static Ptr<XgponXgemFrame> CreateIdleXgemFrame (uint32_t frameSize);

  /**
   * \brief create a short idle xgem frame (four bytes that are all 0.)
   * \return the short idle xgem frame
   */
  static Ptr<XgponXgemFrame> CreateShortIdleXgemFrame ( );



private:
  /**
   * \brief add XGEM header and sdu to xggem frame.
   * \return the XGEM frame to be transmitted
   * \param sdu the SDU (may be a segment)
   * \param portId XGEM port that this packet belongs to
   * \param key the key used to encrypt the payload of XGEM frame
   * \param keyIndex the index of the current key for encryption
   * \param lastFrame whether its the last part of one SDU.
   */
  static Ptr<XgponXgemFrame> CreateXgemFrameWithData(const Ptr<Packet>& sdu, uint16_t portId, const Ptr<XgponKey>& key, uint8_t keyIndex, bool lastFrame);

};

}; // namespace ns3

#endif // XGPON_XGEM_ENGINE_H

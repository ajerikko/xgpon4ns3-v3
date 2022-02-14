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
#ifndef XGPON_OLT_FRAMING_ENGINE_H
#define XGPON_OLT_FRAMING_ENGINE_H

#include "ns3/packet.h"

#include "xgpon-olt-engine.h"
#include "xgpon-xgtc-ds-frame.h"
#include "xgpon-xgtc-us-burst.h"


namespace ns3 {

class XgponOltFramingEngine : public XgponOltEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOltFramingEngine ();
  virtual ~XgponOltFramingEngine ();

  /**
   * \brief produce the XgponXgtcDsFrame to be passed to the PHY adaptation layer
   * \param frame the XgponXgtcDsFrame to be filled
   */
  void ProduceXgtcDsFrame (XgponXgtcDsFrame& xgtcDsFrame);

  
  /**
   * \brief Parse the upstream XGTC burst. 
   * Since the burst may contain multiple lists of packets from different T-CONTs, 
   * framing engine will call xgem engine directly (for multiple times) to process these payloads.
   * \param burst the upstream XGTC burst
   */
  void ParseXgtcUpstreamBurst (XgponXgtcUsBurst& burst);

	

  /////////////////////////////////////////////////////Required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

};

}// namespace ns3

#endif // XGPON_OLT_FRAMING_ENGINE_H

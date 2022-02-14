/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
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
 * Author: Pedro Alvarez <pinheirp@tcd.ie>
 */

#ifndef XGPON_ONU_XGEM_ENGINE_H
#define XGPON_ONU_XGEM_ENGINE_H

#include <vector>

#include "xgpon-onu-engine.h"
#include "xgpon-xgem-frame.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class is just used to parse and generate xgem lists at ONU side. 
 *        When producing the payload of upstream burst, it follows the bwmap from OLT.
 *
 */
class XgponOnuXgemEngine : public XgponOnuEngine
{
public:

  XgponOnuXgemEngine ();
  virtual ~XgponOnuXgemEngine ();

  //////////////////////////////////////////////////////////Main functions
  /**
   * \brief receive a list of XGEM Frames from lower layer (the peer). 
   */
  void ProcessXgemFramesFromLowerLayer(std::vector<Ptr<XgponXgemFrame> >& xgemFrames);


  /**
   * \brief generate a list of XGEM Frames to be transmitted in upstream direction (payload of XgponXgtcUsAllocation).
   * \param xgemFrames the list that the generated xgem frames will be put into
   * \param payloadLength the total length of these generated frames (unit: byte)
   * \param allocId the T-CONT that these frames belong to. 
   */
  void GenerateFramesToTransmit(std::vector<Ptr<XgponXgemFrame> >& xgemFrames, uint32_t payloadLength, uint16_t allocId);




  //////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:

  /* more variables may be needed */

};

}// namespace ns3

#endif // XGPON_ONU_XGEM_ENGINE_H

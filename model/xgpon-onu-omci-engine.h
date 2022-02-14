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

#ifndef XGPON_ONU_OMCI_ENGINE_H
#define XGPON_ONU_OMCI_ENGINE_H

#include "ns3/packet.h"

#include "xgpon-onu-engine.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief OMCI Engine at ONU-side. Currently, we just implement the interfaces.
 *
 */
class XgponOnuOmciEngine : public XgponOnuEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuOmciEngine ();
  virtual ~XgponOnuOmciEngine ();



  ////////////////////////////////////////////////////////////////Operations
  /**
   * \brief receive and process one OMCI packet.
   * \param omciPacket OMCI packet from OLT
   */
  void ReceiveOmciPacket (const Ptr<Packet>& omciPacket);  


  /**
   * \brief generate one OMCI packet and put it into the corresponding queue.
   * \param type the type of the OMCI packet to be generated.
   * \param into the content to be put into the OMCI packet.
   */
  void GenerateOmciPacket4Transmit (uint16_t type, void * info);





  ////////////////////////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;




private:

  /* more variables may be needed */

};





}; // namespace ns3

#endif // XGPON_ONU_OMCI_ENGINE_H

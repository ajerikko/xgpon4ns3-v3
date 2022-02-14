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

#ifndef XGPON_ONU_NET_DEVICE_H
#define XGPON_ONU_NET_DEVICE_H

#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/address.h"

#include "xgpon-net-device.h"
#include "xgpon-onu-conn-manager.h"
#include "xgpon-onu-ploam-engine.h"
#include "xgpon-onu-omci-engine.h"

#include "xgpon-onu-xgem-engine.h"
#include "xgpon-onu-dba-engine.h"
#include "xgpon-onu-framing-engine.h"
#include "xgpon-onu-phy-adapter.h"

#include "xgpon-tcont-onu.h"
#include "xgpon-xgtc-us-burst.h"
#include "xgpon-xgtc-ds-header.h"
#include "xgpon-link-info.h"



namespace ns3{

/**
 * \ingroup xgpon
 * \brief The XG-PON net device at ONU-side. It is the main class for ONU that interacts with upper layers and the optical distribution channel (through XgponPhy).
 *
 */

class XgponOnuNetDevice : public XgponNetDevice
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuNetDevice ( );
  virtual ~XgponOnuNetDevice ( );


  /**
   * \brief receive one downstream frame from xgpon channel. Inherited from PonNetDevice
   */
  virtual void ReceivePonFrameFromChannel (const Ptr<PonFrame>& frame);      






  /**
   * \brief produce and transmit one upstream burst based on BWmap received previously. 
   *        this function will be scheduled when ONU processes BWmap through its DBA engine.
   * \param map the BWmap in which this burst is scheduled
   * \param first the index of the first T-CONT (belonging to this burst) in the BWmap
   */
  void ProduceAndTransmitUsBurst (const Ptr<XgponXgtcBwmap>& map, uint16_t first);






  ////////////////////////////////////////////////////Member variable accessor
  uint16_t GetOnuId ( ) const; 
  void SetOnuId (uint16_t id);


  const Ptr<XgponOnuConnManager>& GetConnManager ( ) const; 
  void SetConnManager (const Ptr<XgponOnuConnManager>& onuConnManager);

  const Ptr<XgponOnuDbaEngine>& GetDbaEngine ( ) const; 
  void SetDbaEngine (const Ptr<XgponOnuDbaEngine>& onuDbaEngine);

  const Ptr<XgponOnuPloamEngine>& GetPloamEngine ( ) const; 
  void SetPloamEngine (const Ptr<XgponOnuPloamEngine>& onuPloamEngine);

  const Ptr<XgponOnuFramingEngine>& GetFramingEngine ( ) const; 
  void SetFramingEngine (const Ptr<XgponOnuFramingEngine>& onuFramingEngine);



  void SetPhyAdapter (const Ptr<XgponOnuPhyAdapter>& phy);
  const Ptr<XgponOnuPhyAdapter>& GetPhyAdapter ( ) const;

  void SetXgemEngine (const Ptr<XgponOnuXgemEngine>& engine);
  const Ptr<XgponOnuXgemEngine>& GetXgemEngine ( ) const;

  void SetOmciEngine (const Ptr<XgponOnuOmciEngine>& engine);
  const Ptr<XgponOnuOmciEngine>& GetOmciEngine ( ) const;




  ////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:

  //process one packet from upper layers. Note that we cannot use the addresses in parameters since they are MAC-layer addresses.
  //Otherwise, we cannot assign multiple XGEM ports to one network interface.
  //The main function is to put the packet into the corresponding queue. return false if the queue is full.
  virtual bool DoSend (const Ptr<Packet>& packet, const Address& dest, uint16_t protocolNumber);    
  virtual bool DoSendFrom (const Ptr<Packet>& packet, const Address& source, const Address& dest, uint16_t protocolNumber);


private:
  Ptr<XgponOnuConnManager> m_onuConnManager;
  Ptr<XgponOnuDbaEngine> m_onuDbaEngine;

  Ptr<XgponOnuPloamEngine> m_onuPloamEngine;
  Ptr<XgponOnuFramingEngine> m_onuFramingEngine;


  Ptr<XgponOnuPhyAdapter> m_onuPhyAdapter;

  Ptr<XgponOnuXgemEngine> m_onuXgemEngine;
  Ptr<XgponOnuOmciEngine> m_onuOmciEngine;



  uint16_t m_onuId;             //The ID of this ONU
  uint16_t m_tcontType;         //static tcont type to be used, when the TCONT type is not calculate from upper layers (e.g. DSCP)


  TracedCallback<const Ptr<const Packet>&, Time > m_phyRxEndTrace;
  TracedCallback<const Ptr<const Packet>&, Time > m_phyTxEndTrace;


};







//////////////////////////////////////////////INLINE functions

inline uint16_t 
XgponOnuNetDevice::GetOnuId ( ) const
{
  return m_onuId;
}
inline void 
XgponOnuNetDevice::SetOnuId (uint16_t id)
{
  m_onuId = id;
}





inline const Ptr<XgponOnuConnManager>& 
XgponOnuNetDevice::GetConnManager ( ) const
{
  NS_ASSERT_MSG((m_onuConnManager!=0), "ConnManager has not been set yet.");
  return m_onuConnManager;
}
inline void 
XgponOnuNetDevice::SetConnManager (const Ptr<XgponOnuConnManager>& onuConnManager)
{
  m_onuConnManager = onuConnManager;
}

inline const Ptr<XgponOnuDbaEngine>& 
XgponOnuNetDevice::GetDbaEngine ( ) const
{
  NS_ASSERT_MSG((m_onuDbaEngine!=0), "Dba Engine has not been set yet.");
  return m_onuDbaEngine;
}
inline void 
XgponOnuNetDevice::SetDbaEngine (const Ptr<XgponOnuDbaEngine>& onuDbaEngine)
{
  m_onuDbaEngine = onuDbaEngine;
}

inline const Ptr<XgponOnuPloamEngine>& 
XgponOnuNetDevice::GetPloamEngine ( ) const
{
  NS_ASSERT_MSG((m_onuPloamEngine!=0), "Ploam Engine has not been set yet.");
  return m_onuPloamEngine;
}
inline void 
XgponOnuNetDevice::SetPloamEngine (const Ptr<XgponOnuPloamEngine>& onuPloamEngine)
{
  m_onuPloamEngine = onuPloamEngine;
}

inline const Ptr<XgponOnuFramingEngine>& 
XgponOnuNetDevice::GetFramingEngine ( ) const
{
  NS_ASSERT_MSG((m_onuFramingEngine!=0), "Framing Engine has not been set yet.");
  return m_onuFramingEngine;
}
inline void 
XgponOnuNetDevice::SetFramingEngine (const Ptr<XgponOnuFramingEngine>& onuFramingEngine)
{
  m_onuFramingEngine = onuFramingEngine;
}




inline void 
XgponOnuNetDevice::SetPhyAdapter (const Ptr<XgponOnuPhyAdapter>& phyAdapter)
{
  m_onuPhyAdapter = phyAdapter;
}
inline const Ptr<XgponOnuPhyAdapter>& 
XgponOnuNetDevice::GetPhyAdapter ( ) const
{
  NS_ASSERT_MSG((m_onuPhyAdapter!=0), "Phy Adapter has not been set yet.");
  return m_onuPhyAdapter;
}

inline void 
XgponOnuNetDevice::SetXgemEngine (const Ptr<XgponOnuXgemEngine>& engine)
{
  m_onuXgemEngine = engine;
}
inline const Ptr<XgponOnuXgemEngine>& 
XgponOnuNetDevice::GetXgemEngine ( ) const
{
  NS_ASSERT_MSG((m_onuXgemEngine!=0), "Xgem engine has not been set yet.");
  return m_onuXgemEngine;
}

inline void 
XgponOnuNetDevice::SetOmciEngine (const Ptr<XgponOnuOmciEngine>& engine)
{
  m_onuOmciEngine = engine;
}
inline const Ptr<XgponOnuOmciEngine>& 
XgponOnuNetDevice::GetOmciEngine ( ) const
{
  NS_ASSERT_MSG((m_onuOmciEngine!=0), "Omci engine has not been set yet.");
  return m_onuOmciEngine;
}




} //namespace ns3
#endif /* XGPON_ONU_NET_DEVICE_H */

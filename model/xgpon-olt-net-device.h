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

#ifndef XGPON_OLT_NET_DEVICE_H
#define XGPON_OLT_NET_DEVICE_H

#include "ns3/address.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "xgpon-net-device.h"
#include "xgpon-olt-conn-manager.h"
#include "xgpon-olt-ploam-engine.h"
#include "xgpon-olt-omci-engine.h"

#include "xgpon-olt-xgem-engine.h"
#include "xgpon-olt-ds-scheduler.h"
#include "xgpon-olt-dba-engine.h"
#include "xgpon-olt-framing-engine.h"
#include "xgpon-olt-phy-adapter.h"



namespace ns3{

/**
 * \ingroup xgpon
 * \brief The XG-PON net device at OLT-side. It is the main class for OLT that interacts with upper layers and the optical distribution channel.
 *
 */

class XgponOltNetDevice: public XgponNetDevice
{
public:

  /**
   * \brief Constructor
   */
  XgponOltNetDevice ( );
  virtual ~XgponOltNetDevice ( );
  


  /**
   * \brief receive one upstream burst from xgpon channel. Inherited from PonNetDevice
   */
  virtual void ReceivePonFrameFromChannel (const Ptr<PonFrame>& frame);



  ///////////////////////////////////////////////////////member variable accessors
  const Ptr<XgponOltConnManager>& GetConnManager ( ) const; 
  void SetConnManager (const Ptr<XgponOltConnManager>& oltConnManager);

  const Ptr<XgponOltDsScheduler>& GetDsScheduler ( ) const; 
  void SetDsScheduler (const Ptr<XgponOltDsScheduler>& oltDsScheduler);

  const Ptr<XgponOltDbaEngine>& GetDbaEngine ( ) const; 
  void SetDbaEngine (const Ptr<XgponOltDbaEngine>& oltDbaEngine);


  const Ptr<XgponOltPloamEngine>& GetPloamEngine ( ) const; 
  void SetPloamEngine (const Ptr<XgponOltPloamEngine>& oltPloamEngine);


  const Ptr<XgponOltFramingEngine>& GetFramingEngine ( ) const; 
  void SetFramingEngine (const Ptr<XgponOltFramingEngine>& oltFramingEngine);


  void SetPhyAdapter (const Ptr<XgponOltPhyAdapter>& phy);
  const Ptr<XgponOltPhyAdapter>& GetPhyAdapter ( ) const;

  void SetXgemEngine (const Ptr<XgponOltXgemEngine>& engine);
  const Ptr<XgponOltXgemEngine>& GetXgemEngine ( ) const;

  void SetOmciEngine (const Ptr<XgponOltOmciEngine>& engine);
  const Ptr<XgponOltOmciEngine>& GetOmciEngine ( ) const;




  //////////////////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



protected:
  //virtual void DoStart (void);
  virtual void DoInitialize (void);

private:

  //process one packet from upper layers. Note that we cannot use the addresses in parameters since they are MAC-layer addresses.
  //Otherwise, we cannot assign multiple XGEM ports to one network interface.
  //The main function is to put the packet into the corresponding queue. return false if the queue is full.
  virtual bool DoSend (const Ptr<Packet>& packet, const Address& dest, uint16_t protocolNumber);  
  virtual bool DoSendFrom (const Ptr<Packet>& packet, const Address& source, const Address& dest, uint16_t protocolNumber);

  /**
   * \brief generate one downstream frame per 125 micro-second and send to ONUs. started in DoStart ();
   */
  void SendDownstreamFrameToChannelPeriodically ( );
  

private:

  Ptr<XgponOltConnManager> m_oltConnManager;
  Ptr<XgponOltDsScheduler> m_oltDsScheduler;
  Ptr<XgponOltDbaEngine> m_oltDbaEngine;

  Ptr<XgponOltFramingEngine> m_oltFramingEngine;

  Ptr<XgponOltPloamEngine> m_oltPloamEngine;

  Ptr<XgponOltPhyAdapter> m_oltPhyAdapter;

  Ptr<XgponOltXgemEngine> m_oltXgemEngine;
  Ptr<XgponOltOmciEngine> m_oltOmciEngine;



  TracedCallback<Ptr<const XgponDsFrame>, Time > m_phyTxEndTrace;
  TracedCallback<Ptr<const Packet>, Time > m_phyRxEndTrace;
};








///////////////////////////////////////////////////INLINE FUNCTIONS

inline const Ptr<XgponOltConnManager>& 
XgponOltNetDevice::GetConnManager ( ) const
{
  NS_ASSERT_MSG((m_oltConnManager!=0), "ConnManager has not been set yet.");
  return m_oltConnManager;
}
inline void 
XgponOltNetDevice::SetConnManager (const Ptr<XgponOltConnManager>& oltConnManager)
{
  m_oltConnManager = oltConnManager;
}

inline const Ptr<XgponOltDsScheduler>&
XgponOltNetDevice::GetDsScheduler ( ) const
{
  NS_ASSERT_MSG((m_oltDsScheduler!=0), "Downstream Scheduler has not been set yet.");
  return m_oltDsScheduler;
}
inline void 
XgponOltNetDevice::SetDsScheduler (const Ptr<XgponOltDsScheduler>& oltDsScheduler)
{
  m_oltDsScheduler = oltDsScheduler;
}

inline const Ptr<XgponOltDbaEngine>& 
XgponOltNetDevice::GetDbaEngine ( ) const
{
  NS_ASSERT_MSG((m_oltDbaEngine!=0), "Dba Engine has not been set yet.");
  return m_oltDbaEngine;
}
inline void 
XgponOltNetDevice::SetDbaEngine (const Ptr<XgponOltDbaEngine>& oltDbaEngine)
{
  m_oltDbaEngine = oltDbaEngine;
}

inline const Ptr<XgponOltPloamEngine>& 
XgponOltNetDevice::GetPloamEngine ( ) const
{
  NS_ASSERT_MSG((m_oltPloamEngine!=0), "Ploam Engine has not been set yet.");
  return m_oltPloamEngine;
}
inline void 
XgponOltNetDevice::SetPloamEngine (const Ptr<XgponOltPloamEngine>& oltPloamEngine)
{
  m_oltPloamEngine = oltPloamEngine;
}

inline const Ptr<XgponOltFramingEngine>& 
XgponOltNetDevice::GetFramingEngine ( ) const
{
  NS_ASSERT_MSG((m_oltFramingEngine!=0), "Framing Engine has not been set yet.");
  return m_oltFramingEngine;
}
inline void 
XgponOltNetDevice::SetFramingEngine (const Ptr<XgponOltFramingEngine>& oltFramingEngine)
{
  m_oltFramingEngine = oltFramingEngine;
}




inline void 
XgponOltNetDevice::SetPhyAdapter (const Ptr<XgponOltPhyAdapter>& phyAdapter)
{
  m_oltPhyAdapter = phyAdapter;
}
inline const Ptr<XgponOltPhyAdapter>& 
XgponOltNetDevice::GetPhyAdapter ( ) const
{
  NS_ASSERT_MSG((m_oltPhyAdapter!=0), "Phy Adapter has not been set yet.");
  return m_oltPhyAdapter;
}

inline void 
XgponOltNetDevice::SetXgemEngine (const Ptr<XgponOltXgemEngine>& engine)
{
  m_oltXgemEngine = engine;
}
inline const Ptr<XgponOltXgemEngine>& 
XgponOltNetDevice::GetXgemEngine ( ) const
{
  NS_ASSERT_MSG((m_oltXgemEngine!=0), "Xgem engine has not been set yet.");
  return m_oltXgemEngine;
}

inline void 
XgponOltNetDevice::SetOmciEngine (const Ptr<XgponOltOmciEngine>& engine)
{
  m_oltOmciEngine = engine;
}
inline const Ptr<XgponOltOmciEngine>& 
XgponOltNetDevice::GetOmciEngine ( ) const
{
  NS_ASSERT_MSG((m_oltOmciEngine!=0), "Omci engine has not been set yet.");
  return m_oltOmciEngine;
}



}; //namespace ns3
#endif /* XGPON_OLT_NET_DEVICE_H */

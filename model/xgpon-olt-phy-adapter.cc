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
#include "ns3/log.h"

#include "xgpon-olt-phy-adapter.h"
#include "xgpon-olt-net-device.h"

#include "xgpon-psbu.h"
#include "xgpon-psbd.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltPhyAdapter");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltPhyAdapter);

TypeId 
XgponOltPhyAdapter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltPhyAdapter")
    .SetParent<XgponOltEngine> ()
    .AddConstructor<XgponOltPhyAdapter> ()
  ;
  return tid;
}
TypeId 
XgponOltPhyAdapter::GetInstanceTypeId () const
{
  return GetTypeId ();
}



XgponOltPhyAdapter::XgponOltPhyAdapter () : XgponOltEngine(), m_sfc(0), m_ponid(0)
{
}
XgponOltPhyAdapter::~XgponOltPhyAdapter ()
{
}




void 
XgponOltPhyAdapter::ProcessXgponUsBurstFromChannel (const Ptr<XgponUsBurst>& burst, const Ptr<XgponBurstProfile>& profile)
{
  NS_LOG_FUNCTION(this);

  //profile is determined based on the receiving time.
  //it might be used for deserialization, FEC decode, etc. when physical layer details are simulated. 

  return;
}



void
XgponOltPhyAdapter::ProcessXgtcDsFrameFromUpperLayer (const Ptr<XgponDsFrame>& frame)
{
  NS_LOG_FUNCTION(this);

  XgponPsbd& psbd = frame->GetPsbd();

  psbd.SetPsync(0);                  //currently, we don't care about the value of PSYNC
  psbd.SetSfc(m_sfc++);              //frame counter must be added by one
  psbd.SetPonId (m_ponid);
  psbd.CalculateSfcHec (0);
  psbd.CalculatePonIdHec (m_ponid);

  //FEC and Line Coding are omitted here.

  return;  
}


}//namespace ns3

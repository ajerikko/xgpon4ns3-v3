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
 * Author: Jerome Arokkiam <jerom2005raj@gmail.com>
 */

#include "ns3/log.h"

#include "ns3/ipv4-address.h"

#include "xgpon-onu-conn-manager-speed.h"
#include "xgpon-onu-net-device.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuConnManagerSpeed");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuConnManagerSpeed);


TypeId 
XgponOnuConnManagerSpeed::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuConnManagerSpeed")
    .SetParent<XgponOnuConnManager> ()
    .AddConstructor<XgponOnuConnManagerSpeed> ()
  ;
  return tid;
}
TypeId 
XgponOnuConnManagerSpeed::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOnuConnManagerSpeed::XgponOnuConnManagerSpeed () 
  :XgponOnuConnManager(),
  m_tconts(16), m_usConns(64),
  m_dsConns(64), m_broadcastConns(0)
{
}
XgponOnuConnManagerSpeed::~XgponOnuConnManagerSpeed ()
{
}







void 
XgponOnuConnManagerSpeed::AddOneUsTcont (const Ptr<XgponTcontOnu>& tcont)
{
  NS_LOG_FUNCTION(this);

  uint16_t onuId = m_device->GetOnuId();
  uint16_t allocId = tcont->GetAllocId();
  uint16_t index = ((allocId - onuId) / 1024) - 1; //jerome, X2
  //jerome, X2
  //std::cout << "index: " << index << std::endl;
  
  //jerome, X2, to make sure only 16 allocIDs are possible in an ONU ID
  //allocId = 1024*index + onuId
  NS_ASSERT_MSG((((allocId % 1024)==onuId) && (index<16)), "Unreasonable Alloc-ID for this ONU!!!");

  m_tconts[index] = tcont;
}

//jerome, X2, this is exactly the reverse of the AddOnueUsTcont function
const Ptr<XgponTcontOnu>& 
XgponOnuConnManagerSpeed::GetTcontById (uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  uint16_t onuId = m_device->GetOnuId();

  if((allocId % 1024)!=onuId) return m_nullTcont;
  
  uint16_t index = ((allocId - onuId) / 1024) - 1; //jerome, X2
  NS_ASSERT_MSG((index<16), "Unreasonable Alloc-ID for this ONU!!!");

  return m_tconts[index];
}



void 
XgponOnuConnManagerSpeed::AddOneUsConn (const Ptr<XgponConnectionSender>& conn, uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponTcontOnu>& tcont = GetTcontById (allocId);
  if(tcont != 0)
  {
    tcont->AddOneConnection (conn);    

    uint16_t xgemPort = conn->GetXgemPort();
    uint16_t onuId = m_device->GetOnuId();

    uint16_t index = (xgemPort - onuId) / 1024;
    //jerome, X2
		//std::cout << "allocId: " << allocId << ", onuId: " << onuId << ", index: " << allocId/1024 - 1 << std::endl;
		
		NS_ASSERT_MSG((((xgemPort % 1024)==onuId) && (index<64)), "Unreasonable XgemPort for this ONU!!!");
    m_usConns[allocId/1024 - 1] = conn;   //jerome, X2 

    if(conn->GetOnuId() == conn->GetXgemPort()) m_usOmciConn = conn;    
  }
}


const Ptr<XgponConnectionSender>& 
XgponOnuConnManagerSpeed::FindUsConnByAddress (const Address& addr)
{
  NS_LOG_FUNCTION(this);

  //TODO: byte-order needs to be investigated further
  Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
  uint32_t temp = ipv4Addr.Get();
  //uint16_t onuId = (temp >> (32 - m_onuNetmaskLen)) % 1024;
  //uint16_t newOnuId = onuId / 256; //jerome, X1
  uint16_t onuId = ((temp >> 16) % 1024)-512; //jerome, X1, this is to cater 14.x.x.x ip from eNB insted of default 12.x.x.x in onu client
  if(onuId != m_device->GetOnuId()) return m_nullConnSender;

  uint16_t index = temp & 0x0000003F;  
  //std::cout << "connection index: " << index << std::endl;
  return m_usConns[index];
}

//jerome, X2
const Ptr<XgponConnectionSender>& 
XgponOnuConnManagerSpeed::FindUsConnByTcontType (const uint16_t& type)
{
  NS_LOG_FUNCTION(this);
	
	//std::cout << "tcontType: " << type << std::endl;
	//TODO: Tcont 1 is not used in simulations. Thus it is ignored.
	if (type == 1)
		return m_usConns[0];
	else if (type == 2)
		return m_usConns[1];
	else if (type == 3)
		return m_usConns[2];
	else if (type == 4)
		return m_usConns[3];
	else
		return m_nullConnSender;
}






void 
XgponOnuConnManagerSpeed::AddOneDsConn (const Ptr<XgponConnectionReceiver>& conn)
{
  NS_LOG_FUNCTION(this);

  if(conn->IsBroadcast()) m_broadcastConns.push_back(conn);
  else 
  {
    uint16_t onuId = m_device->GetOnuId();
    uint16_t xgemPort = conn->GetXgemPort();
    uint16_t index = (xgemPort - onuId) / 1024;
    NS_ASSERT_MSG((((xgemPort % 1024)==onuId) && (index<64)), "Unreasonable XgemPort for this ONU!!!");

    m_dsConns[index] = conn;
  }
}
const Ptr<XgponConnectionReceiver>& 
XgponOnuConnManagerSpeed::FindDsConnByXgemPort (uint16_t port)
{
  NS_LOG_FUNCTION(this);

  uint16_t tmp = port % 1024;
  uint16_t onuId = m_device->GetOnuId();

  if((tmp!=onuId) && (tmp!=1023)) return m_nullConnReceiver;  //XgemPort for other Onus

  if(tmp==onuId)  
  {
    uint16_t index = (port - onuId) / 1024;
    NS_ASSERT_MSG((index<64), "Unreasonable XgemPort for this ONU!!!");
    return m_dsConns[index];
  } else
  {
    int num = m_broadcastConns.size();
    for(int i=0; i<num; i++)
    {
      if(m_broadcastConns[i]->GetXgemPort()==port) return m_broadcastConns[i];
    }    
    return m_nullConnReceiver;
  }
}




}; // namespace ns3


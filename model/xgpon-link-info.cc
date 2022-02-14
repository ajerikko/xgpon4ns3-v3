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

#include "xgpon-link-info.h"



NS_LOG_COMPONENT_DEFINE ("XgponLinkInfo");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (XgponLinkInfo);

TypeId 
XgponLinkInfo::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponLinkInfo")
    .SetParent<Object> ()
    .AddConstructor<XgponLinkInfo> ()
  ;
  return tid;
}
TypeId 
XgponLinkInfo::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponLinkInfo::XgponLinkInfo ():
  m_onuId(0),
  m_activeDsKeys(MAX_NUMBER_OF_KEYS),
  m_curDsKeyIndex(0),
  m_activeUsKeys(MAX_NUMBER_OF_KEYS),
  m_curUsKeyIndex(0),
  m_profiles(MAX_NUMBER_OF_PROFILES),
  m_curProfileIndex(0),
  m_eqDelay(0),
  m_ploamMsgQueue(),
  m_dyingGasp(false),
  m_ploamExistAtOnu(false)
{
}
XgponLinkInfo::~XgponLinkInfo ()
{
}







void 
XgponLinkInfo::AddPloam2TxQueue (const Ptr<XgponXgtcPloam>& msg)
{
  m_ploamMsgQueue.push(msg);
}
const Ptr<XgponXgtcPloam> 
XgponLinkInfo::GetPloam2Transmit(void)
{
  Ptr<XgponXgtcPloam> msg = m_ploamMsgQueue.front( );

  m_ploamMsgQueue.pop( );

  return msg;
}


bool 
XgponLinkInfo::HasPloam2Transmit4ONU(void) const
{
  int num = m_ploamMsgQueue.size();

  if(num<=0) return false;
  else return true;
}




void XgponLinkInfo::DeepCopy(const Ptr<XgponLinkInfo>& linkInfo)
{
  m_onuId = linkInfo->m_onuId;

  m_curDsKeyIndex = linkInfo->m_curDsKeyIndex;
  int num = linkInfo->m_activeDsKeys.size();
  for(int i=0; i<num; i++)
  {
    if(linkInfo->m_activeDsKeys[i] !=0)
    {
      Ptr<XgponKey> key = CreateObject<XgponKey> ();
      key->DeepCopy(linkInfo->m_activeDsKeys[i]);
      m_activeDsKeys[i] = key;    
    }
  }


  m_curUsKeyIndex = linkInfo->m_curUsKeyIndex;
  num = linkInfo->m_activeUsKeys.size();
  for(int i=0; i<num; i++)
  {
    if(linkInfo->m_activeUsKeys[i] !=0)
    {
      Ptr<XgponKey> key = CreateObject<XgponKey> ();
      key->DeepCopy(linkInfo->m_activeUsKeys[i]);
      m_activeUsKeys[i] = key;    
    }
  }


  m_curProfileIndex = linkInfo->m_curProfileIndex;
  num = linkInfo->m_profiles.size();
  for(int i=0; i<num; i++)
  {
    if(linkInfo->m_profiles[i] !=0)
    {
      Ptr<XgponBurstProfile> profile = CreateObject<XgponBurstProfile> ();
      profile->DeepCopy(linkInfo->m_profiles[i]);
      m_profiles[i] = profile;    
    }
  }

  m_eqDelay = linkInfo->m_eqDelay;

  m_dyingGasp = linkInfo->m_dyingGasp;         

  m_ploamExistAtOnu = linkInfo->m_ploamExistAtOnu;   
}







}; // namespace ns3


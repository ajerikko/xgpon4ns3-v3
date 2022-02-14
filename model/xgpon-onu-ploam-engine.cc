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

#include "xgpon-onu-ploam-engine.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuPloamEngine");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOnuPloamEngine);


TypeId 
XgponOnuPloamEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuPloamEngine")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuPloamEngine> ()
  ;
  return tid;
}
TypeId 
XgponOnuPloamEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOnuPloamEngine::XgponOnuPloamEngine (): m_linkInfo(0)
{
}
XgponOnuPloamEngine::~XgponOnuPloamEngine ()
{
}







void 
XgponOnuPloamEngine::ReceivePloamMessage (const Ptr<XgponXgtcPloam>& msg)
{
  NS_LOG_FUNCTION(this);
  return;
}

void
XgponOnuPloamEngine::GeneratePloamMessage ( )
{
  NS_LOG_FUNCTION(this);

  Ptr<XgponXgtcPloam> ploam = Create<XgponXgtcPloam>();
  m_linkInfo->AddPloam2TxQueue (ploam);

  return;
}






}; // namespace ns3


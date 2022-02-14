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

#include "ns3/log.h"

#include "xgpon-olt-ds-scheduler.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltDsScheduler");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltDsScheduler);

TypeId 
XgponOltDsScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDsScheduler")
    .SetParent<XgponOltEngine> ()
  ;
  return tid;
}
TypeId
XgponOltDsScheduler::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOltDsScheduler::XgponOltDsScheduler ():m_nullConn(0)
{
  m_startFrame = true;
}
XgponOltDsScheduler::~XgponOltDsScheduler ()
{
}

void 
XgponOltDsScheduler::Prepare2ProduceDsFrame ( )
{
  m_startFrame = true;
}






}//namespace ns3

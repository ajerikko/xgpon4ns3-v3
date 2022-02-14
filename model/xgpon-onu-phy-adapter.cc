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

#include "xgpon-onu-phy-adapter.h"
#include "xgpon-onu-net-device.h"
/*
#include "xgpon-psbu.h"
#include "xgpon-psbd.h"
*/

NS_LOG_COMPONENT_DEFINE ("XgponOnuPhyAdapter");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOnuPhyAdapter);

TypeId 
XgponOnuPhyAdapter::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuPhyAdapter")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuPhyAdapter> ()
  ;
  return tid;
}
TypeId 
XgponOnuPhyAdapter::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOnuPhyAdapter::XgponOnuPhyAdapter () : XgponOnuEngine(), m_sfc(0), m_ponid(0)
{
}
XgponOnuPhyAdapter::~XgponOnuPhyAdapter ()
{
}



void
XgponOnuPhyAdapter::ProcessXgponDsFrameFromChannel (const Ptr<XgponDsFrame>& frame)
{
  NS_LOG_FUNCTION(this);

  NS_ASSERT_MSG((frame!=0), "The downstream to be processed is zero!!!");
  //FEC and Line decode are omitted here.
  //We also assume that data is received successfully (w/o error). May change in the future to simulate corruption

  XgponPsbd& psbd = frame->GetPsbd ();
  m_sfc = psbd.GetSfc();

  return;
}


void 
XgponOnuPhyAdapter::ProcessXgtcBurstFromUpperLayer (const Ptr<XgponUsBurst>& burst, const Ptr<XgponBurstProfile>& profile)
{
  //TODO: Currently, we don't care the content of preamble and delimiter.
  //      In the future, we may fill the values suggested by the standard.
  //      Several byte arrays may be added to XgponOnuPhyAdapter to hold these values.


  //NS_LOG_FUNCTION(this);
  //XgponPsbu& psbu = burst->GetPsbu();
  //uint8_t  tmpData[100];
  //psbu.SetPreamble(tmpData, profile->GetPreambleLen ());
  //psbu.SetDelimiter(tmpData, profile->GetDelimiterLen ());



  //the potential FEC and Line Coding are omitted here.

  return;
}



}//namespace ns3

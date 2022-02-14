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

#include "ns3/simulator.h"
#include "ns3/log.h"

#include "xgpon-olt-xgem-engine.h"
#include "xgpon-xgem-routines.h"

#include "xgpon-olt-net-device.h"




NS_LOG_COMPONENT_DEFINE ("XgponOltXgemEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltXgemEngine);

TypeId 
XgponOltXgemEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltXgemEngine")
    .SetParent<XgponOltEngine> ()
    .AddConstructor<XgponOltXgemEngine> ()
  ;
  return tid;
}
TypeId 
XgponOltXgemEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponOltXgemEngine::XgponOltXgemEngine ()
{
}
XgponOltXgemEngine::~XgponOltXgemEngine ()
{
}



void
XgponOltXgemEngine::GenerateFramesToTransmit(std::vector<Ptr<XgponXgemFrame> >& xgemFrames, std::vector<Ptr<XgponXgemFrame> >& broadcastXgemFrames, std::vector<uint8_t>& bitmap4Onus, uint32_t payloadLength)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponOltDsScheduler>& scheduler = m_device->GetDsScheduler();
  const Ptr<XgponOltPloamEngine>& ploamEngine = m_device->GetPloamEngine ( );
  
  scheduler->Prepare2ProduceDsFrame ( );

  uint32_t currentPayloadSize = 0;
  while(currentPayloadSize < payloadLength)
  {
    uint32_t availableSize = payloadLength - currentPayloadSize;
    if(availableSize==4)  //create a short idle xgem frame
    {
      xgemFrames.push_back(XgponXgemRoutines::CreateShortIdleXgemFrame ( ));
      return;
    }
    else if(availableSize<16)
    {
      xgemFrames.push_back(XgponXgemRoutines::CreateIdleXgemFrame (availableSize));
      return;
    }
    else //SDUs (if exist) will be encapsulated.
    {
      uint32_t amountToServe; 
      const Ptr<XgponConnectionSender>& conn = scheduler->SelectConnToServe (&amountToServe);

      if(conn==0)  //OLT has no data send. Fill with idle XGEM frames
      {
        while(availableSize>0)
        {
          if(availableSize > XgponXgemRoutines::XGPON_XGEM_FRAME_MAXLEN) 
          {
            xgemFrames.push_back(XgponXgemRoutines::CreateIdleXgemFrame (XgponXgemRoutines::XGPON_XGEM_FRAME_MAXLEN));
            availableSize = availableSize - XgponXgemRoutines::XGPON_XGEM_FRAME_MAXLEN;
          } 
          else if(availableSize==4)
          {
            xgemFrames.push_back(XgponXgemRoutines::CreateShortIdleXgemFrame ( ));
            return;
          }
          else
          {
            xgemFrames.push_back(XgponXgemRoutines::CreateIdleXgemFrame (availableSize));
            return;
          }
        }
        return;
      }
      else //OLT has data to send. Generate XGEM frames with upper layer data
      {
        bool doSegmentation = false;
        //if it is the last connection to be served, doSegmentation is enabled
        if(amountToServe > (payloadLength - currentPayloadSize)) 
        {
          amountToServe = (payloadLength - currentPayloadSize);
          doSegmentation = true;
        }

        Ptr<XgponXgemFrame> frame;
        do
        {
          if(conn->IsBroadcast())
          {
            frame = XgponXgemRoutines::GenerateXgemFrame (m_device, conn, amountToServe, 0, 0, doSegmentation);
          }
          else
          {
            const Ptr<XgponLinkInfo>& linkInfo = ploamEngine->GetLinkInfo(conn->GetOnuId());
            frame = XgponXgemRoutines::GenerateXgemFrame (m_device, conn, amountToServe, 
                                       linkInfo->GetCurrentUsKey(), linkInfo->GetCurrentUsKeyIndex(), doSegmentation);
          }

          if(frame!=0)
          {
            if(conn->IsBroadcast()) broadcastXgemFrames.push_back(frame);
            else 
            {
              bitmap4Onus[conn->GetOnuId()] = 1;
              xgemFrames.push_back(frame);
            }
           

            currentPayloadSize += frame->GetSerializedSize();
            amountToServe -= frame->GetSerializedSize();

            ///////////////////////////////update the statistics
            (m_device->GetStatistics()).m_passToXgponBytes += (frame->GetXgemHeader()).GetPli();
          }
        } while(amountToServe>=16 && frame!=0);
      }
    }
  }  //end of while 

  return;
}



void 
XgponOltXgemEngine::ProcessXgemFramesFromLowerLayer (std::vector<Ptr<XgponXgemFrame> >& frames, uint16_t onuId, uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  //Get the engines necessary for processing these Xgem frames
  const Ptr<XgponOltConnManager>& connManager = m_device->GetConnManager ( ); 

  //const Ptr<XgponLinkInfo>& linkInfo = (m_device->GetPloamEngine ( ))->GetLinkInfo(onuId);
  //used to get key for decryption

  const Ptr<XgponTcontOlt>& tcontOlt = connManager->GetTcontById(allocId);
  NS_ASSERT_MSG((tcontOlt!=0), "Cannot find the corresponding T-CONT at OLT-side!!!");

  //jerome, C1
  const uint16_t tcontOltType = (uint16_t)tcontOlt->GetTcontType();
  NS_ASSERT_MSG((tcontOltType!=0), "Invalid TCONT Type when receiving data at OLT-side!!!");

  std::vector<Ptr<XgponXgemFrame> >::iterator it, end;
  it = frames.begin();
  end = frames.end();
  for(;it!=end; it++)
  {  
    XgponXgemFrame::XgponXgemFrameType type = (*it)->GetType();
    if(type ==  XgponXgemFrame::XGPON_XGEM_FRAME_WITH_DATA) //For idle XGEM frame, do nothing
    {
      XgponXgemHeader& xgemHeader = (*it)->GetXgemHeader();
      const Ptr<Packet>& payload = (*it)->GetData();
      NS_ASSERT_MSG((payload->GetSize() > 0), "Data length should not be zero!!!");

      //const Ptr<XgponKey>& key = linkInfo->GetUsKeyByIndex (xgemHeader.GetKeyIndex());      
      //carry out decryption if needed.
 
      Ptr<Packet> sdu = tcontOlt->GetPacket4Reassemble();
      if(sdu!=0) { sdu->AddAtEnd(payload); }
      else { sdu = payload; }

      if(xgemHeader.GetLastFragmentFlag()==0) //save back for further reassemble
      {
        tcontOlt->SetPacket4Reassemble (sdu);
      }
      else  //send to upper layer
      {
        uint16_t portId = xgemHeader.GetXgemPortId ();
        if(portId == onuId) { m_device->GetOmciEngine()->ReceiveOmciPacket(sdu); } //send to OMCI
        else 
          { 
                //std::cout << "AllocID: " << allocId << ", T-CONT Type: " << tcontOltType << ", payload size: " << sdu->GetSize() << std::endl; //jerome, Apr 9
                m_device->SendSduToUpperLayer (sdu, tcontOltType, onuId); 
           } //send to upper layers
      } //end for fragmentation state
    } //end for frame with data   
  } //end for the loop

  return;
}





}//namespace ns3

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

#include "xgpon-onu-xgem-engine.h"
#include "xgpon-onu-net-device.h"

#include "xgpon-onu-us-scheduler.h"
#include "xgpon-xgem-routines.h"



NS_LOG_COMPONENT_DEFINE ("XgponOnuXgemEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOnuXgemEngine);

TypeId 
XgponOnuXgemEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOnuXgemEngine")
    .SetParent<XgponOnuEngine> ()
    .AddConstructor<XgponOnuXgemEngine> ()
  ;
  return tid;
}
TypeId 
XgponOnuXgemEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


XgponOnuXgemEngine::XgponOnuXgemEngine ()
{
}
XgponOnuXgemEngine::~XgponOnuXgemEngine ()
{
}



void
XgponOnuXgemEngine::GenerateFramesToTransmit(std::vector<Ptr<XgponXgemFrame> >& xgemFrames, uint32_t payloadLength, uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  Ptr<XgponConnectionSender> conn;

  const Ptr<XgponOnuConnManager>& connManager = m_device->GetConnManager ( ); 
  const Ptr<XgponTcontOnu>& tcontOnu = connManager->GetTcontById (allocId);
    //jerome, C1
    const uint16_t tcontOnuType = (uint16_t)tcontOnu->GetTcontType(); 
    NS_ASSERT_MSG((tcontOnuType!=0), "Invalid TCONT Type when sending data from ONU!!!");
  const Ptr<XgponOnuUsScheduler>& scheduler = tcontOnu->GetOnuUsScheduler();
  const Ptr<XgponLinkInfo>& linkInfo = (m_device->GetPloamEngine ( ))->GetLinkInfo();

  uint32_t currentPayloadSize, availableSize;
  currentPayloadSize = 0;
  while(currentPayloadSize < payloadLength)
  {
    availableSize = payloadLength - currentPayloadSize;
    if(availableSize==4)  //create a short idle xgem frame
    {
      xgemFrames.push_back(XgponXgemRoutines::CreateShortIdleXgemFrame ( ));
      return;
    }
    else if(availableSize<16) //create one idle xgem frame
    {
      xgemFrames.push_back(XgponXgemRoutines::CreateIdleXgemFrame(availableSize));
      return;
    }
    else //SDUs (if exist) will be encapsulated.
    {
      uint32_t amountToServe;
      conn = scheduler->SelectConnToServe (&amountToServe);
      if(conn==0)  //this T-CONT has no data send. fill with idle XGEM frames
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
      else
      {
        bool doSegmentation = false;
        if(amountToServe > (payloadLength - currentPayloadSize)) 
        {
          //the last connection to be served, segmentation will be carried out if it is necessary
          amountToServe = (payloadLength - currentPayloadSize);
          doSegmentation = true;
        }


        Ptr<XgponXgemFrame> frame;
        do
        {
          frame = XgponXgemRoutines::GenerateXgemFrame (m_device, conn, amountToServe, 
                                       linkInfo->GetCurrentUsKey(), linkInfo->GetCurrentUsKeyIndex(), doSegmentation);
          if(frame!=0)
          {
            xgemFrames.push_back(frame);
            currentPayloadSize += frame->GetSerializedSize();
            amountToServe -= frame->GetSerializedSize();

            ///////////////////////////////update statistics
            (m_device->GetStatistics()).m_passToXgponBytes += (frame->GetXgemHeader()).GetPli();
          }
        } while(amountToServe>=16 && frame!=0);
      }
    }
  }  //end of while 

  return;
}



void 
XgponOnuXgemEngine::ProcessXgemFramesFromLowerLayer (std::vector<Ptr<XgponXgemFrame> >& frames)
{
  NS_LOG_FUNCTION(this);


  const Ptr<XgponOnuConnManager>& connManager = m_device->GetConnManager ( ); 

  //jerome, C1, TODO: need to bring allocId into this function to set the correct tcontOnuType
  //const Ptr<XgponTcontOnu>& tcontOnu = connManager->GetTcontById (allocId); 
  //NS_ASSERT_MSG((tcontOnu!=0), "Cannot find the corresponding T-CONT at OLT-side!!!");
  const uint16_t tcontOnuType = 0;//(uint16_t)tcontOnu->GetTcontType();
  //NS_ASSERT_MSG((tcontOnuType!=0), "Invalid TCONT Type when receiving data at OLT-side!!!");
//const Ptr<XgponLinkInfo>& linkInfo = (m_device->GetPloamEngine ( ))->GetLinkInfo();
  //used to find the key for decryption


  std::vector<Ptr<XgponXgemFrame> >::iterator it, end;
  it = frames.begin();
  end = frames.end();
  for(;it!=end; it++)
  {
    XgponXgemFrame::XgponXgemFrameType type = (*it)->GetType();
    if(type ==  XgponXgemFrame::XGPON_XGEM_FRAME_WITH_DATA) //For idle XGEM frame, do nothing
    {
      XgponXgemHeader& xgemHeader = (*it)->GetXgemHeader();
      uint16_t portId = xgemHeader.GetXgemPortId ();
      const Ptr<XgponConnectionReceiver>& conn = connManager->FindDsConnByXgemPort(portId);

      if(conn!=0)  //whether this XGEM frame is for this ONU
      {
        const Ptr<Packet>& payload = (*it)->GetData();
        NS_ASSERT_MSG((payload->GetSize() > 0), "Data length should not be zero!!!");

        //Ptr<XgponKey> key = linkInfo->GetDsKeyByIndex (xgemHeader.GetKeyIndex());
        //carry out decryption if needed.
 
        Ptr<Packet> sdu = conn->GetPacket4Reassemble();
        if(sdu!=0) { sdu->AddAtEnd(payload); }
        else { sdu = payload; }

        if(xgemHeader.GetLastFragmentFlag()==0) //save back for further reassemble
        {
          conn->SetPacket4Reassemble (sdu);
        }
        else  //send to upper layer
        {          
          if(portId == m_device->GetOnuId( )) { m_device->GetOmciEngine()->ReceiveOmciPacket(sdu); } //send to OMCI
          else { m_device->SendSduToUpperLayer (sdu, tcontOnuType, 1024); } //send to upper layers          
        } //end for fragmentation state
      } //end for frames whose destination is this ONU   
    } //end for non-idle-frames
  } //end for the loop
  return;
}



}//namespace ns3

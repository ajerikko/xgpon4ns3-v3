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

#include "xgpon-xgem-routines.h"
#include "xgpon-net-device.h"


namespace ns3 {

Ptr<XgponXgemFrame>
XgponXgemRoutines::CreateIdleXgemFrame (uint32_t frameSize)
{
  NS_ASSERT_MSG(((frameSize%4) == 0 && (frameSize <= XGPON_XGEM_FRAME_MAXLEN) ),"Idle XGEM frame size is too long or the size is not a multiple of 4!!!");

  Ptr<XgponXgemFrame> frame = Create<XgponXgemFrame> ();
  frame->SetType (XgponXgemFrame::XGPON_XGEM_FRAME_LONG_IDLE);

  XgponXgemHeader& idleHeader = frame->GetXgemHeader ();
  idleHeader.SetXgemPortId(XGPON_IDLE_XGEM_PORT_ID);
  idleHeader.SetPli(frameSize-XgponXgemHeader::XGPON_XGEM_HEADER_LENGTH);
  idleHeader.SetKeyIndex(0);
  idleHeader.SetOptions(0);
  idleHeader.SetLastFragmentFlag(1);	
  idleHeader.CalculateHec ();

  return frame;
}

Ptr<XgponXgemFrame>
XgponXgemRoutines::CreateShortIdleXgemFrame ( )
{
  Ptr<XgponXgemFrame> frame = Create<XgponXgemFrame> ();
  frame->SetType (XgponXgemFrame::XGPON_XGEM_FRAME_SHORT_IDLE);

  return frame;
}






Ptr<XgponXgemFrame> 
XgponXgemRoutines::GenerateXgemFrame (const Ptr<XgponNetDevice>& device, const Ptr<XgponConnectionSender>& conn, uint32_t maxLen, const Ptr<XgponKey>& key, uint8_t keyIndex, bool doSegmentation)
{
  NS_ASSERT_MSG((maxLen % 4 == 0), "maxLen has a strange value!!!");

  bool segmenting = conn->IsSegmentationRunning ( );  //must be called before the call "conn->GetOnePacketForTransmit" that will affect the mode.


  Ptr<Packet> sdu = conn->GetOnePacketForTransmit();
  if(sdu == 0) return 0;  //all packets had been transmitted.


  //////////for sdu that will be segmented, only one event is traced for the first segment.
  if(segmenting ==false)  
  {    
    //trace the virtual per-device queue event: dequeue
    device->TraceVirtualQueueDequeueEvent (sdu);

    //for sniffering that one packet is transmitting from this netdevice.
    device->TraceForSniffers (sdu);
  }


  uint32_t sduSize = sdu->GetSize();
  uint32_t frameSize = XgponXgemFrame::GetPaddedPayloadSize(sduSize) + XgponXgemHeader::XGPON_XGEM_HEADER_LENGTH;
  NS_ASSERT_MSG((frameSize <= XGPON_XGEM_FRAME_MAXLEN), "The size of the sdu to be generated is too long!!!");

  if(frameSize <= maxLen)
  {
    return CreateXgemFrameWithData(sdu, conn->GetXgemPort(), key, keyIndex, true);
  }
  else if(doSegmentation)
  {
    uint32_t firstSegmentSize = maxLen - XgponXgemHeader::XGPON_XGEM_HEADER_LENGTH;
    Ptr<Packet> frag0 = sdu->CreateFragment (0, firstSegmentSize); 
    Ptr<Packet> frag1 = sdu->CreateFragment (firstSegmentSize, (sduSize - firstSegmentSize)); 
    conn->PutRemainingSegmentIntoQueue(frag1);  //push back to tx-queue.

    return CreateXgemFrameWithData(frag0, conn->GetXgemPort(), key, keyIndex, false);
  } else return 0;
}





Ptr<XgponXgemFrame>
XgponXgemRoutines::CreateXgemFrameWithData(const Ptr<Packet>& sdu, uint16_t portId, const Ptr<XgponKey>& key, uint8_t keyIndex, bool lastFrame)
{
  Ptr<XgponXgemFrame> frame = Create<XgponXgemFrame> ();  

  frame->SetType (XgponXgemFrame::XGPON_XGEM_FRAME_WITH_DATA);
  frame->SetData (sdu);

  //Setting Xgem Header
  XgponXgemHeader& xgemHeader = frame->GetXgemHeader();
  xgemHeader.SetPli(sdu->GetSize());
  xgemHeader.SetKeyIndex(keyIndex);
  xgemHeader.SetXgemPortId(portId);
  xgemHeader.SetOptions(0);
  if(lastFrame)	xgemHeader.SetLastFragmentFlag(1);
  else xgemHeader.SetLastFragmentFlag(0);
  xgemHeader.CalculateHec ();

  //ENCRYPTION is omitted for simulation speed.
	
  return frame;	
}


}//namspace ns3

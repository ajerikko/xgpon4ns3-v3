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

#ifndef XGPON_XGEM_FRAME_H
#define XGPON_XGEM_FRAME_H

#include <stack>

#include "ns3/simple-ref-count.h"
#include "ns3/packet.h"


#include "xgpon-xgem-header.h"

namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The Xgem Frame transmitted over XG-PON.
 *
 * This class contains both XgponXgemHeader and the corresponding SDU.
 * It also supports the general operations for (de)serialization.
 */
class XgponXgemFrame : public SimpleRefCount<XgponXgemFrame>
{
  //static uint64_t CREATED_XGEM_FRAME_NUM4DEBUG;
  //static uint64_t DELETED_XGEM_FRAME_NUM4DEBUG;

  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  //Enumeration of the modes supported in the class.
  enum XgponXgemFrameType
  {
    XGPON_XGEM_FRAME_SHORT_IDLE,     //short idle frame (shorter than header, there is no header and data)
    XGPON_XGEM_FRAME_LONG_IDLE,      //long idle frame (one header exist, but sdu is empty)
    XGPON_XGEM_FRAME_WITH_DATA,      //normal frame (with both header and sdu)
  };


  /**
   * \brief Constructor
   */
  XgponXgemFrame ();
  virtual ~XgponXgemFrame ();


  ///////////////////////////////////////////////////Member variable analysis
  XgponXgemFrame::XgponXgemFrameType GetType ();
  const Ptr<Packet>& GetData ();

  void SetType(XgponXgemFrame::XgponXgemFrameType t);
  void SetData(const Ptr<Packet>& pkt);


  XgponXgemHeader& GetXgemHeader ();  //there is no corresponding set-operator for the header.
                                      //the caller will change the header of the xgem frame directly.



  /**
   * \brief get payload length after padding
   */
  static uint32_t GetPaddedPayloadSize (uint32_t payloadSize);  




  /////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();



  //general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

private:
  XgponXgemFrameType m_type;
  XgponXgemHeader m_header;
  Ptr<Packet> m_data;

  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponUsBurst::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }
};


inline XgponXgemFrame::XgponXgemFrameType 
XgponXgemFrame::GetType ()
{
  return m_type;
}

inline const Ptr<Packet>& 
XgponXgemFrame::GetData ()
{
  return m_data;
}

inline void 
XgponXgemFrame::SetType(XgponXgemFrame::XgponXgemFrameType t)
{
  m_type = t;
}

inline void 
XgponXgemFrame::SetData(const Ptr<Packet>& pkt)
{
  m_data = pkt;
}

inline XgponXgemHeader& 
XgponXgemFrame::GetXgemHeader ()
{
  return m_header;
}




}; // namespace ns3

#endif // XGPON_XGEM_FRAME_H

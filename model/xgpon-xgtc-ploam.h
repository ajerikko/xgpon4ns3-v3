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

#ifndef XGPON_XGTC_PLOAM_H
#define XGPON_XGTC_PLOAM_H

#include <cstdlib>
#include <stack>

#include "ns3/simple-ref-count.h"
#include "ns3/buffer.h"


namespace ns3 {


/**
 * \ingroup xgpon
 * \brief The PLOAM Message used by XG-PON.
 *
 * This class has fields corresponding to those in an XGTC PLOAM message as well as
 * methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */


class XgponXgtcPloam : public SimpleRefCount<XgponXgtcPloam>
{
  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  const static uint16_t XGPON_XGTC_PLOAM_LENGTH=48;           //unit: byte

  /**
   * \brief Constructor
   */
  XgponXgtcPloam ();
  virtual ~XgponXgtcPloam ();

  XgponXgtcPloam& operator=(const XgponXgtcPloam &rhs);


  /////////////////////////////////////member variables accessors (inline functions)
  void SetOnuId (uint16_t oid);
  uint16_t GetOnuId () const;

  void SetType (uint8_t mtype);
  uint8_t GetType () const;

  void SetSeqNo (uint8_t seq);
  uint8_t GetSeqNo () const;

  void SetContent (uint8_t *src);
  void GetContent (uint8_t *dst) const;


  //inline functions since the two functions are empty.
  void CalculateMic ();
  bool VerifyMic () const;
  void GetMic (uint8_t *dst);  //inline function too



  /////////////////////////////////////////META-data; time related; inline function
  void SetCreateTime (uint64_t time);  
  uint64_t GetCreateTime () const; 

  void SetReceiveTime (uint64_t time); 
  uint64_t GetReceiveTime () const;





  ///////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
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
  uint16_t  m_onuId;          //onu-id. Len: 16bits. Note that 6 MSB bits are reserved and the default value is 0.
  uint8_t   m_type;           //the type of the PLOAM message. Len: 8 bits
  uint8_t   m_seqNo;          //sequence number of this PLOAM message. Len: 8 bits
  uint8_t   m_content[36];    //message content. Len: 36 bytes. The value depends on the kind of PLOAM message
  uint8_t   m_mic[8];         //Message integrity check. Len: 8 bytes



  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) 
  //void* operator new[](size_t size) throw(const char*)
  {
    void *p = malloc(size);
    if (!p) throw "XgponXgtcPloam::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }


};









////////////////////////////////////////////////////////INLINE Functions
inline void 
XgponXgtcPloam::SetOnuId (uint16_t oid)
{
  m_onuId = oid & 0x03ff;  //10bits
}
inline uint16_t XgponXgtcPloam::GetOnuId () const
{
  return m_onuId;
}

inline void 
XgponXgtcPloam::SetType (uint8_t mtype)
{
  m_type = mtype;
}
inline uint8_t XgponXgtcPloam::GetType () const
{
  return m_type;
}

inline void 
XgponXgtcPloam::SetSeqNo (uint8_t seq)
{
  m_seqNo = seq;
}
inline uint8_t XgponXgtcPloam::GetSeqNo () const
{
  return m_seqNo;
}

inline void 
XgponXgtcPloam::SetContent (uint8_t *src)
{
  for(int i=0; i<36; i++) { m_content[i] = *(src + i); }
}
inline void 
XgponXgtcPloam::GetContent (uint8_t *dst) const
{
  for(int i=0; i<36; i++) { *(dst + i) = m_content[i]; }
}


inline void 
XgponXgtcPloam::CalculateMic ()
{
  //leave alone for saving CPU
  return;
}
inline bool XgponXgtcPloam::VerifyMic ( ) const
{
  return true;
}

inline void 
XgponXgtcPloam::GetMic (uint8_t *dst)
{
  for(int i=0; i<8; i++) { *(dst + i) = m_mic[i]; }
}




}; // namespace ns3

#endif // XGPON_XGTC_PLOAM_H

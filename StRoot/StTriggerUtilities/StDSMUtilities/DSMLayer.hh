//
// Pibero Djawotho <pibero@comp.tamu.edu>
// Texas A&M University Cyclotron Institute
// 1 Jan 2009
//

#ifndef DSM_LAYER_HH
#define DSM_LAYER_HH

#if defined(__linux__) || defined(__APPLE__)
#if defined(__linux__) 
/* linux has its own (fast) swaps */
#include <byteswap.h>
#else
#if defined(__APPLE__)
//#warning "byteswap.h is an unportable GNU extension!  Don't use!"
static inline unsigned short bswap_16(unsigned short x) {
  return ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)) ;
}
static inline unsigned int bswap_32(unsigned int x) {
  return (bswap_16(x&0xffff)<<16) | (bswap_16(x>>16));
}
static inline unsigned long long bswap_64(unsigned long long x) {
  return (((unsigned long long)bswap_32(x&0xffffffffull))<<32) |
(bswap_32(x>>32));
}
#endif
#define swap16(x) bswap_16(x)
#define swap32(x) bswap_32(x)
#endif
#else	/* non-linux && non-APPLE stuff */

extern inline unsigned short swap16(unsigned short x)
{
	return ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)) ;
}

extern inline unsigned int swap32(unsigned int x) 
{
     return ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |               \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24)) ;
}

#endif /* BYTESWAP */
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

#include "DSM.hh"

inline long long swapLL(long long x)
{
  return ((x & 0xffff000000000000ll) >> 48 |
	  (x & 0x0000ffff00000000ll) >> 16 |
	  (x & 0x00000000ffff0000ll) << 16 |
	  (x & 0x000000000000ffffll) << 48);
}

inline void copy_and_swap16(void* dest, const void* src)
{
  long long* x = (long long*)src;
  long long* y = (long long*)dest;

  //*y++ = bswap_64(*x++);
  //*y++ = bswap_64(*x++);

  *y = bswap_64(*x); ++x; ++y;
  *y = bswap_64(*x); ++x; ++y;
}

inline void copy_and_swap8(void* dest, const void* src)
{
  long long* x = (long long*)src;
  long long* y = (long long*)dest;

  *y++ = swapLL(*x++);
  *y++ = swapLL(*x++);
}

template<class T> struct DSMLayer : public vector<DSM> {
  DSMLayer(int n) : vector<DSM>(n) {}
  virtual ~DSMLayer() {}
  virtual void setRegister(int i, int value);
  virtual int  getRegister(int i) const;
  virtual bool read(const T& event) = 0;
  virtual void write(DSMLayer& layer) = 0;
  virtual void run() = 0;
  virtual void save(int nchannels, short* buffer);
  virtual void dump() { for_each(begin(), end(), mem_fun_ref(&DSM::dump)); }
};

template<class T> inline void DSMLayer<T>::setRegister(int i, int value)
{
  for (vector<DSM>::iterator dsm = begin(); dsm != end(); ++dsm)
    dsm->registers[i] = value;
}

template<class T> inline int DSMLayer<T>::getRegister(int i) const
{
  return front().registers[i];
}

template<class T> inline void DSMLayer<T>::save(int nchannels, short* buffer)
{
  for (size_t dsm = 0; dsm < size(); ++dsm) {
    short* channels = (*this)[dsm].channels;
    copy(&channels[0], &channels[nchannels], &buffer[dsm*nchannels]);
  }
}

#endif	// DSM_LAYER_HH

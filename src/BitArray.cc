#include "BitArray.h"

BitArray::BitArray()
  : array(0)
{

}

BitArray::BitArray(size_t size)
  : array((1<<size) - 1)
{

}

void BitArray::set(size_t index)
{
  uint64_t mask = 1 << index;
  array = array | mask;
}

void BitArray::reset(size_t index)
{
  uint64_t mask = ~(1 << index);
  array = array & mask;
}

uint64_t BitArray::decimal()
{
  return array;
}

int64_t BitArray::find_msone() {
  int64_t index = -1;
  if (array > 0)
  {
    if (array < 0x8000000000000000)
    {
      uint64_t mask = 1;
      while (mask <= array)
      {
          index++;
          mask = mask << 1;
      }
    }
    else
      index = 63;
  }
  return index;
}

void BitArray::clear()
{
  array = 0;
}

void BitArray::load(uint64_t data)
{
  array = data;
}

std::ostream& operator<<(std::ostream& os, const BitArray& a)
{
  return os << a.array;
}
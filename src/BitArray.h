#ifndef BITARRAY_H
#define BITARRAY_H

#include <cstdlib>
#include <cstdint>
#include <limits>
#include <cstddef>
#include <iostream>

class BitArray {
  protected:
    /** @brief The bit container (max 64 bits) */
    uint64_t array;

  public:
    /** @brief The default constructor */
    BitArray();
    /** @brief Sets as many bits as the parameter indicates */
    BitArray(size_t);
    /** @brief Sets the bit indicating in the position */
    virtual void set(size_t);
    /** @brief Resets the bit inidicating in the position */
    virtual void reset(size_t);
    /** @brief Clears the array */
    virtual void clear();
    /** @brief Loads an unsigned 64-bit integer */
    virtual void load(uint64_t);
    /** @brief Returns the decimal value of the array.
     *  @return An 64-bit unsigned integer
     */
    virtual uint64_t decimal();
    /** @brief Finds the most significant one on the array.  
     *  @return The index of the most significant one, or -1
     */
    virtual int64_t find_msone();
    /** @brief Overloads the operator << */
    friend std::ostream& operator<<(std::ostream&, const BitArray&);
};

#endif
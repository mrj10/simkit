#include "identity_elements.hh"

#include <stdint.h>

template<> uint8_t getANDIdentityElement<uint8_t>() { return UINT8_MAX; }
template<> uint16_t getANDIdentityElement<uint16_t>() { return UINT16_MAX; }
template<> uint32_t getANDIdentityElement<uint32_t>() { return UINT32_MAX; }
template<> uint64_t getANDIdentityElement<uint64_t>() { return UINT64_MAX; }
template<> int8_t getANDIdentityElement<int8_t>() { return INT8_MAX; }
template<> int16_t getANDIdentityElement<int16_t>() { return INT16_MAX; }
template<> int32_t getANDIdentityElement<int32_t>() { return INT32_MAX; }
template<> int64_t getANDIdentityElement<int64_t>() { return INT64_MAX; }
template<> bool getANDIdentityElement<bool>() { return true; }

template<> uint8_t getORIdentityElement<uint8_t>() { return 0; }
template<> uint16_t getORIdentityElement<uint16_t>() { return 0; }
template<> uint32_t getORIdentityElement<uint32_t>() { return 0; }
template<> uint64_t getORIdentityElement<uint64_t>() { return 0; }
template<> int8_t getORIdentityElement<int8_t>() { return 0; }
template<> int16_t getORIdentityElement<int16_t>() { return 0; }
template<> int32_t getORIdentityElement<int32_t>() { return 0; }
template<> int64_t getORIdentityElement<int64_t>() { return 0; }
template<> bool getORIdentityElement<bool>() { return false; }

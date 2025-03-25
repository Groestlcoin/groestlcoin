// Copyright (c) 2014-2024 The Groestlcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <consensus/amount.h>
#include <uint256.h>
#include <serialize.h>
#include <crypto/common.h>
#include <span.h>

namespace XCoin {

class ConstBuf {
public:
	const unsigned char *P;
	size_t Size;

	ConstBuf(const std::span<const unsigned char> p) {
		if (p.begin() == p.end()) {
			P = 0;
			Size = 0;
		} else {
			P = (unsigned char*)(p.data());
            Size = p.size();
		}
	}

	template <typename T>
	ConstBuf(const T pb, const T pe) {
		if (pb == pe) {
			P = 0;
			Size = 0;
		} else {
			P = (unsigned char*)(&pb[0]);
			Size = (pe-pb) * sizeof(pb[0]);
		}
	}

	ConstBuf(const std::vector<unsigned char>& vch) {
		if (vch.empty()) {
			P = 0;
			Size = 0;
		} else {
			P = &vch[0];
			Size = vch.size();
		}
	}
};


uint256 HashGroestl(const ConstBuf& cbuf);

uint256 HashFromTx(const ConstBuf& cbuf);
uint256 HashForSignature(const ConstBuf& cbuf);
inline uint256 HashPow(const ConstBuf& cbuf) { return HashGroestl(cbuf); }
inline uint256 HashMessage(const ConstBuf& cbuf) { return HashGroestl(cbuf); }
inline uint256 HashForAddress(const ConstBuf& cbuf) { return HashGroestl(cbuf); }


class GroestlHasher {
private:
	void *ctx;
public:
	void Finalize(std::span<unsigned char> output);
	GroestlHasher& Write(const unsigned char *data, size_t len);
	GroestlHasher& Write(std::span<const unsigned char> input);
	GroestlHasher();
	GroestlHasher(GroestlHasher&& x);
	~GroestlHasher();
	GroestlHasher& operator=(GroestlHasher&& x);
};

/** A writer stream (for serialization) that computes a 256-bit hash. */
class GroestlHashWriter
{
private:
    GroestlHasher ctx;

public:
    void write(std::span<const std::byte> src)
    {
        ctx.Write(UCharCast(src.data()), src.size());
    }

    /** Compute the hash of all data written to this object.
     *
     * Invalidates this object.
     */
    uint256 GetHash() {
        uint256 result;
	ctx.Finalize(result);
	return result;
    }

     /**
      * Returns the first 64 bits from the resulting hash.
      */
     inline uint64_t GetCheapHash() {
         uint256 result = GetHash();
         return ReadLE64(result.begin());
     }

     template <typename T>
     GroestlHashWriter& operator<<(const T& obj)
     {
          ::Serialize(*this, obj);
	  return *this;
     }
};


} // XCoin::

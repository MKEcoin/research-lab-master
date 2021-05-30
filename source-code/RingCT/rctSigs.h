// Copyright (c) 2016, Monero Research Labs
//
// Author: Shen Noether <shen.noether@gmx.com>
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#define DBG

#ifndef RCTSIGS_H
#define	RCTSIGS_H

#include <cstddef>
#include <mutex>
#include <vector>
#include <tuple>

#include "generic-ops.h"

#include "crypto-ops.h"
#include "random.h"
#include "keccak.h"
#include "crypto.h"


#include "rctTypes.h"
#include "rctOps.h"




using namespace std;
using namespace crypto;

namespace rct {

    //Schnorr Non-linkable
    //Gen Gives a signature (L1, s1, s2) proving that the sender knows "x" such that xG = one of P1 or P2
    //Ver Verifies that signer knows an "x" such that xG = one of P1 or P2
    //These are called in the below ASNL sig generation
    void GenSchnorrNonLinkable(key & L1, key & s1, key & s2, const key & x, const key & P1, const key & P2, int index);
    bool VerSchnorrNonLinkable(const key & P1, const key & P2, const key & L1, const key & s1, const key & s2);

    //Aggregate Schnorr Non-linkable Ring Signature (ASNL)
    // c.f. http://eprint.iacr.org/2015/1098 section 5.
    // These are used in range proofs (alternatively Borromean could be used)
    // Gen gives a signature which proves the signer knows, for each i,
    //   an x[i] such that x[i]G = one of P1[i] or P2[i]
    // Ver Verifies the signer knows a key for one of P1[i], P2[i] at each i
    asnlSig GenASNL(key64 x, key64 P1, key64 P2, bits indices);
    bool VerASNL(key64 P1, key64 P2, asnlSig &as);

    //Multilayered Spontaneous Anonymous Group Signatures (MLSAG signatures)
    //These are aka MG signatutes in earlier drafts of the ring ct paper
    // c.f. http://eprint.iacr.org/2015/1098 section 2.
    // keyImageV just does I[i] = xx[i] * HashToPoint(xx[i] * G) for each i
    // Gen creates a signature which proves that for some column in the keymatrix "pk"
    //   the signer knows a secret key for each row in that column
    // Ver verifies that the MG sig was created correctly
    keyV keyImageV(const keyV &xx);
    mgSig MLSAG_Gen(const keyM & pk, const keyV & xx, const int index);
    bool MLSAG_Ver(keyM &pk, mgSig &sig);
    //mgSig MLSAG_Gen_Old(const keyM & pk, const keyV & xx, const int index);

    //proveRange and verRange
    //proveRange gives C, and mask such that \sumCi = C
    //   c.f. http://eprint.iacr.org/2015/1098 section 5.1
    //   and Ci is a commitment to either 0 or 2^i, i=0,...,63
    //   thus this proves that "amount" is in [0, 2^64]
    //   mask is a such that C = aG + bH, and b = amount
    //verRange verifies that \sum Ci = C and that each Ci is a commitment to 0 or 2^i
    rangeSig proveRange(key & C, key & mask, const xmr_amount & amount);
    bool verRange(key & C, rangeSig & as);

    //Ring-ct MG sigs
    //Prove:
    //   c.f. http://eprint.iacr.org/2015/1098 section 4. definition 10.
    //   This does the MG sig on the "dest" part of the given key matrix, and
    //   the last row is the sum of input commitments from that column - sum output commitments
    //   this shows that sum inputs = sum outputs
    //Ver:
    //   verifies the above sig is created corretly
    mgSig proveRctMG(const ctkeyM & pubs, const ctkeyV & inSk, const keyV &outMasks, const ctkeyV & outPk, int index);
    bool verRctMG(mgSig mg, ctkeyM & pubs, ctkeyV & outPk);

    //These functions get keys from blockchain
    //replace these when connecting blockchain
    //getKeyFromBlockchain grabs a key from the blockchain at "reference_index" to mix with
    //populateFromBlockchain creates a keymatrix with "mixin" columns and one of the columns is inPk
    //   the return value are the key matrix, and the index where inPk was put (random).
    void getKeyFromBlockchain(ctkey & a, size_t reference_index);
    tuple<ctkeyM, xmr_amount> populateFromBlockchain(ctkeyV inPk, int mixin);

    //RingCT protocol
    //genRct:
    //   creates an rctSig with all data necessary to verify the rangeProofs and that the signer owns one of the
    //   columns that are claimed as inputs, and that the sum of inputs  = sum of outputs.
    //   Also contains masked "amount" and "mask" so the receiver can see how much they received
    //verRct:
    //   verifies that all signatures (rangeProogs, MG sig, sum inputs = outputs) are correct
    //decodeRct: (c.f. http://eprint.iacr.org/2015/1098 section 5.1.1)
    //   uses the attached ecdh info to find the amounts represented by each output commitment
    //   must know the destination private key to find the correct amount, else will return a random number
    rctSig genRct(ctkeyV & inSk, ctkeyV  & inPk, const keyV & destinations, const vector<xmr_amount> amounts, const int mixin);
    bool verRct(rctSig & rv);
    xmr_amount decodeRct(rctSig & rv, key & sk, int i);



}
#endif	/* RCTSIGS_H */


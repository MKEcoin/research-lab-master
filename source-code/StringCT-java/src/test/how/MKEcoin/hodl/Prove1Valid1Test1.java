package test.how.MKEcoin.hodl;

import how.MKEcoin.hodl.crypto.Curve25519Point;
import how.MKEcoin.hodl.crypto.Scalar;

import static how.MKEcoin.hodl.crypto.CryptoUtil.COMb;
import static how.MKEcoin.hodl.ringSignature.StringCT.*;

public class Prove1Valid1Test1 {

  public static void main(String[] args) {

    Scalar[][] b = new Scalar[][] {
      new Scalar[]{Scalar.ONE, Scalar.ZERO},
      new Scalar[]{Scalar.ZERO, Scalar.ONE}
    };

    Scalar r = Scalar.ONE;

    Proof1 P = PROVE1(b, r);

    Curve25519Point B = COMb(b, r);

    System.out.println("VALID1 returns " + VALID1(B, P));

  }


}

#!/usr/bin/python

import sys
import hmac, binascii

def pbkdf2_bin(hash_fxn, password, salt, iterations, keylen=16):
    _pack_int = struct.Struct('>I').pack
    hashfunc = sha1
    mac = hmac.new(password, None, hashfunc)

    def _pseudorandom(x, mac=mac):
        h = mac.copy()
        h.update(x)
        return map(ord, h.digest())

    buf = []
    for block in xrange(1, -(-keylen // mac.digest_size) + 1):
        rv = u = _pseudorandom(salt + _pack_int(block))
        for i in xrange(iterations - 1):
            u = _pseudorandom(''.join(map(chr, u)))
            rv = itertools.starmap(operator.xor, itertools.izip(rv, u))
        buf.extend(rv)
    return ''.join(map(chr, buf))[:keylen]


try:
    from hashlib import pbkdf2_hmac
except ImportError:
    # python version not available (Python <2.7.8, macOS < 10.11)
    # use @mitsuhiko's pbkdf2 method
    pbkdf2_hmac = pbkdf2_bin
    from hashlib import sha1


if len(sys.argv) >= 2:
    key = pbkdf2_hmac('sha1', sys.argv[1], b'saltysalt', 1003)[:16]
    print binascii.hexlify(key)

#!/usr/bin/env python3
import os, ctypes

lib = ctypes.CDLL('./tweetnacl.so')

def mac(key, msg):
    tag = ctypes.create_string_buffer(16)
    lib.crypto_onetimeauth_poly1305_tweet(tag, key, len(msg), msg) # they fucking swapped msg and key
    return bytes(tag)

def chk(key, tag, msg):
    return not lib.crypto_onetimeauth_poly1305_tweet_verify(tag, key, len(msg), msg)


key = os.urandom(32) # actually msg
print('key:', key.hex())

for _ in range(32):
    msg = os.urandom(32) # actuall key
    print(msg.hex(), mac(key, msg).hex())

msg = os.urandom(32)
print(msg.hex(), '?'*32)

tag = bytes.fromhex(input())

if not chk(key, tag, msg):
    print('no')
    exit(1)

print(open('flag.txt').read().strip())


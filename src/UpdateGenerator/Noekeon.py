import struct

def Debug(Text, Data):
    print(Text, "{:08x} {:08x} {:08x} {:08x}".format(*Data))

def __ToUnsigned(Number):
    return Number + 2**32

def __Theta(k, a):
    tmp  = a[0] ^ a[2]
    tmp ^= ((tmp << 8) & 0xFFFFFFFF | (tmp >> 24)) ^ ((tmp << 24) & 0xFFFFFFFF | (tmp >> 8))
    a[1]^= tmp
    a[3]^= tmp

    a[0] ^= k[0]
    a[1] ^= k[1]
    a[2] ^= k[2]
    a[3] ^= k[3]

    tmp  = a[1]^a[3]
    tmp ^= ((tmp << 8) & 0xFFFFFFFF | (tmp >> 24)) ^ ((tmp << 24) & 0xFFFFFFFF | (tmp >> 8))
    a[0] ^= tmp
    a[2] ^= tmp
    return a

def __Pi1(a):
    a[1] = ((a[1] << 1) & 0xFFFFFFFF | (a[1] >> (31)))
    a[2] = ((a[2] << 5) & 0xFFFFFFFF | (a[2] >> (27)))
    a[3] = ((a[3] << 2) & 0xFFFFFFFF | (a[3] >> (30)))
    return a

def __Pi2(a):
    a[1] = (((a[1] << 31) & 0xFFFFFFFF) | (a[1] >> 1))
    a[2] = (((a[2] << 27) & 0xFFFFFFFF) | ((a[2]) >> 5))
    a[3] = (((a[3] << 30) & 0xFFFFFFFF) | ((a[3]) >> 2))
    return a

def __Gamma(a):
    #/* first non-linear step in gamma */
    a[1] ^= __ToUnsigned(~a[3] & ~a[2])
    a[0] ^=   a[2] & a[1]

    #/* linear step in gamma */
    tmp   = a[3]
    a[3]  = a[0]
    a[0]  = tmp
    a[2] ^= a[0] ^ a[1] ^ a[3]
    
    #/* last non-linear step in gamma */
    a[1] ^= __ToUnsigned(~a[3] & ~a[2])
    a[0] ^=   a[2] & a[1]
    #/* Gamma */
    return a

def __Round(k, a, RC1, RC2):
    a[0] ^= RC1
    a = __Theta(k,a)
    a[0] ^= RC2
    a = __Pi1(a)
    a = __Gamma(a)
    a = __Pi2(a)
    return a

def __RCShiftRegFwd(RC):
    if (RC & 0x80):
        return ((RC << 1) ^ 0x1B) & 0xFF
    else:
        return (RC << 1) & 0xFF

def __RCShiftRegBwd(RC):
    if (RC & 0x01):
        return (RC >> 1) ^ 0x8D
    else:
        return RC >> 1

def __CommonLoop(k, a, RC1, RC2):
    for _ in range(16):
        a = __Round(k, a, RC1, RC2)
        RC1 = __RCShiftRegFwd(RC1)
        RC2 = __RCShiftRegBwd(RC2)
    a[0]^=RC1
    a = __Theta(k,a)
    a[0]^=RC2
    return a

# Noekeon encryption
# Both Data & Key should be bytes
def Encrypt(Key, Data):
    State = list(struct.unpack(">IIII", Data))
    Result = __CommonLoop(Key, State, 0x80, 0)
    return struct.pack(">IIII", *Result)

def KeySetup(Key):
    State = list(struct.unpack(">IIII", Key))
    Result = __CommonLoop((0, 0, 0, 0), State, 0x80, 0)
    return Result
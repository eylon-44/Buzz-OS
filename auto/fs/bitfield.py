# Bit-field Interface

import struct
import math

from config import *

class BitField:
    
    _bits = Config.get()["disk"]["bits"]  # number of bits in a single element in the bit-field

    def __init__(self, count: int) -> None:
        ''' Set the number of bits in the bit-field and create the field. '''
        self.count  = count
        self._field = [0] * math.ceil(self.count/BitField._bits)
        self.seek   = 0

    def __str__(self) -> str:
        return bin(int.from_bytes(self.binary(), byteorder='little'))

    @property
    def binary(self) -> bytes:
        ''' Get the binary representation of the bit-field '''
        return struct.pack(f"{Config.get()['disk']['endian']}{math.ceil(self.count/BitField._bits)}I", *self._field)


    def set(self, index: int, value: int | bool) -> None:
        ''' Set a bit at a certain index. '''
        if index >= self.count or index < 0:
            raise ValueError("Bit-field Index is out of range")

        i = int(index / BitField._bits)     # index
        s = index % BitField._bits          # shift

        if value:
            self._field[i] |= (1 << s)      # set bit
        else:
            self._field[i] &= ~(1 << s)     # unset bit
        

    def get(self) -> int:
        ''' Get an index of a free bit in the bit-field and mark it as used. '''
        # Step the bit-field until finding a free bit
        while self.check(self.seek):
            self.seek = (self.seek + 1) % self.count

        self.set(self.seek, 1)

        return self.seek
    

    def check(self, index: int) -> None:
        ''' Get the value of a bit at a certain index. '''
        if index >= self.count or index < 0:
            raise ValueError("Bit-field Index is out of range")
        
        i = int(index / BitField._bits)     # index
        s = index % BitField._bits          # shift

        return (self._field[i] >> s) & 1
    
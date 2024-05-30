# Disk Image Driver

class Disk:

    def __init__(self, path: str, size: int) -> None:
        ''' Create a disk image with a given size. '''
        self._img = path
        print(f"Creating a new disk image at {self._img}.")
        with open(self._img, "wb") as img:
            img.truncate(size)


    def write(self, data: bytes | bytearray, offset: int) -> None:
        ''' Write binary data to the disk image at a given offset. '''
        print(f"Writing {len(data)} bytes to disk at offset {offset}.")
        
        # Write to disk
        with open(self._img, 'r+b') as img:
            img.seek(offset)
            img.write(data)


    def read(self, offset: int, length: int) -> bytes:
        ''' Read binary data from the disk image at a given offset and length. '''
        print(f"Reading {len(data)} bytes from disk at offset {offset}.")
        
        # Read disk
        with open(self._img, 'rb') as img:
            img.seek(offset)
            data = img.read(length)

        return data
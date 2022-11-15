#!/usr/bin/python3

from pathlib import Path
import shutil
import sys

def crc32mpeg2(buf, crc=0xffffffff):
    for val in buf:
        crc ^= val << 24
        for _ in range(8):
            crc = crc << 1 if (crc & 0x80000000) == 0 else (crc << 1) ^ 0x104c11db7
    return crc


if len(sys.argv) < 4:
    print ("Insufficient arguments!")
    print("Usage: gen-server.py FileName FileVersion ServerPath")
    quit()

InputFile = Path(sys.argv[1])
if not InputFile.is_file():
    print ("Error!! File Not Exists!\n")
    quit()

VersionList = sys.argv[2].split(".")
if len(VersionList) < 3:
    print ("Error!! Version format incorrect!\n")
    print ("Use SemVer format: Major.Minor.Path!\n")
    quit()

ServerDir = Path(sys.argv[3])
if not ServerDir.is_dir():
    print ("Error!! Server path is not a directory!\n")
    quit()

BinaryData = bytearray()
#=============================================================
# Write version info
BinaryData.append(int(VersionList[2]))
BinaryData.append(int(VersionList[1]))
BinaryData.append(int(VersionList[0]))
BinaryData.append(0)
#=============================================================
# Write file name
BinaryData.append(len(InputFile.name))
BinaryData.extend(InputFile.name.encode('latin-1'))
BinaryData.append(0)
# Write file size
BinaryData.extend(InputFile.stat().st_size.to_bytes(4, 'little'))
#=============================================================
# Calculate CRC32
CRC = 0xffffffff
with open(InputFile, "rb") as file:
    while True:
        buffer = file.read(4)
        if not buffer:
            break
        WordData = bytearray()
        WordData.extend(buffer)
        WordData.reverse()
        CRC = crc32mpeg2(WordData, CRC)

print("Generated CRC: " + hex(CRC).upper())
BinaryData.extend(CRC.to_bytes(4, 'little'))
#=============================================================
# Copy binary file to server directory
shutil.copy(InputFile, ServerDir)
# Create update file
UpdateFile = Path(ServerDir / "update")
UpdateFile.write_bytes(BinaryData)

print("ALL Done!\n")
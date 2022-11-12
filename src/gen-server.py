#!/usr/bin/python3

from pathlib import Path
import shutil
import zlib
import sys


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
CRC = zlib.crc32(InputFile.read_bytes(), 0)
BinaryData.extend(CRC.to_bytes(4, 'little'))
#=============================================================
# Copy binary file to server directory
shutil.copy(InputFile, ServerDir)
# Create update file
UpdateFile = Path(ServerDir / "update")
UpdateFile.write_bytes(BinaryData)

print("ALL Done!\n")
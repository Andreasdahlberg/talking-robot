import os
import subprocess
import struct

# ffmpeg -i out.wav -ar 8000 -c:a pcm_u8 low.wav
# ffmpeg -i out.wav -ar 8000 -c:a pcm_u8 -flags +bitexact low.wav // No list section

WAV_FMT = '4si4s3sIHHIIHH4sI'

wav_file = open('out.wav', 'rb')

with open('low.wav', 'rb') as wav_file:
    data = wav_file.read()


    header = struct.unpack(WAV_FMT, data[:struct.calcsize(WAV_FMT)])
    print(header)

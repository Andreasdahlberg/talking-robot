import os
import subprocess
import struct

# ffmpeg -i out.wav -ar 8000 -c:a pcm_u8 low.wav
# ffmpeg -i out.wav -ar 8000 -c:a pcm_u8 -flags +bitexact low.wav // No list section

WAV_HEADER_FMT = '4si4s3sIHHIIHH4sI'

wav_file = open('out.wav', 'rb')


STRUCT_FMT = """
struct sound_{name} {{
    struct sound base;
    uint8_t data[{length}];
}};
"""

STRUCT_DATA_FMT = """
static struct sound_{name} {name} = {{
    {{{length}}},
    {{{data}}}
}};
"""

def trim(wav):
    QUIET_SAMPLE = 128

    leading_trim = 0
    trailing_trim = 0

    for i in range(wav.data_size):
        if wav.data[i] != QUIET_SAMPLE:
            print('Trim {} leading samples'.format(i))
            leading_trim = i
            if leading_trim > 0:
                leading_trim -= 1
            break

    for i in range(wav.data_size):
        if wav.data[wav.data_size - 1 - i] != QUIET_SAMPLE:
            print('Trim {} trailing samples'.format(i))
            trailing_trim = i
            if trailing_trim > 0:
                trailing_trim -= 1
            break

    total_trim = leading_trim + trailing_trim
    print('Trim {} samples({}%)'.format(total_trim, int(total_trim / wav.data_size * 100)))

    wav.data = wav.data[leading_trim:-trailing_trim]


class Wav(object):
    _WAV_HEADER_FMT = '4si4s3sIHHIIHH4sI'
    _WAV_HEADER_SIZE = struct.calcsize(_WAV_HEADER_FMT)

    def __init__(self):
        self._file_size = 0
        self._number_of_channels = 0
        self._sample_rate = 0
        self._bits_per_sample = 0
        self._data_size = 0
        self._data = None

    @property
    def file_size(self):
        return self._file_size

    @property
    def number_of_channels(self):
        return self._number_of_channels

    @property
    def sample_rate(self):
        return self._sample_rate

    @property
    def bits_per_sample(self):
        return self._bits_per_sample

    @property
    def data_size(self):
        return self._data_size

    @property
    def data(self):
        return self._data

    @data.setter
    def data(self, data):
        self._data = data
        self._data_size = len(data)

    def _fill(self, raw_data):
        header = struct.unpack(self._WAV_HEADER_FMT, raw_data[:self._WAV_HEADER_SIZE])

        self._file_size = header[1]
        self._number_of_channels = header[6]
        self._sample_rate = header[7]
        self._bits_per_sample = header[10]
        self._data_size = header[12]
        self._data = raw_data[self._WAV_HEADER_SIZE:self._WAV_HEADER_SIZE + self._data_size]

        assert(self._data_size == len(self._data))

    def load(self, file_name):
        with open(file_name, 'rb') as wav_file:
            raw_data = wav_file.read()
            self._fill(raw_data)


wav = Wav()
wav.load('low.wav')
trim(wav)

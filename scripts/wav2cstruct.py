#!/usr/bin/env python3
# -*- coding: utf-8 -*

"""Script to generate c-structs from Wav-files."""

import struct

class Wav():
    """Class for storing and manipulating WAV-files."""
    _WAV_HEADER_DATA_FMT = '4sI'
    _WAV_HEADER_DATA_SIZE = struct.calcsize(_WAV_HEADER_DATA_FMT)
    _WAV_HEADER_FMT = '4si4s3sIHHIIHH' + _WAV_HEADER_DATA_FMT
    _WAV_HEADER_SIZE = struct.calcsize(_WAV_HEADER_FMT)
    _WAV_QUIET_SAMPLE = 128

    _WAV_STRUCT_FMT = """
    struct sound_{name} {{
        struct sound base;
        uint8_t data[{length}];
    }};
    """

    _WAV_STRUCT_DATA_FMT = """
    static const struct sound_{name} {name} PROGMEM = {{
        {{{length}}},
        {{{data}}}
    }};
    """

    def __init__(self, name):
        self._name = name
        self._number_of_channels = 0
        self._sample_rate = 0
        self._bits_per_sample = 0
        self._data_size = 0
        self._data = None

    @property
    def name(self):
        return self._name

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

        self._number_of_channels = header[6]
        self._sample_rate = header[7]
        self._bits_per_sample = header[10]

        if header[11] == b'data':
            self._data_size = header[12]
            self._data = raw_data[self._WAV_HEADER_SIZE:self._WAV_HEADER_SIZE + self._data_size]
        elif header[11] == b'LIST':
            list_size = header[12]

            data_header_start = self._WAV_HEADER_SIZE + list_size
            data_header_end = data_header_start + self._WAV_HEADER_DATA_SIZE

            data = struct.unpack(self._WAV_HEADER_DATA_FMT,
                                 raw_data[data_header_start:data_header_end])

            self._data_size = data[1]
            self._data = raw_data[data_header_end:data_header_end + self._data_size]
        else:
            assert False


        assert self._data_size == len(self._data)

    def load(self, file_name):
        """Load object with data from file."""
        with open(file_name, 'rb') as wav_file:
            raw_data = wav_file.read()
            self._fill(raw_data)

    def trim(self):
        """Remove any leading and trailing quiet samples."""
        leading_trim = 0
        trailing_trim = 0

        for i in range(self.data_size):
            if self.data[i] != self._WAV_QUIET_SAMPLE:
                leading_trim = i
                if leading_trim > 0:
                    leading_trim -= 1
                break

        for i in range(self.data_size):
            if self.data[self.data_size - 1 - i] != self._WAV_QUIET_SAMPLE:
                trailing_trim = i
                if trailing_trim > 0:
                    trailing_trim -= 1
                break

        self.data = self.data[leading_trim:-trailing_trim]
        return leading_trim + trailing_trim

    def to_c_struct(self):
        """Return WAV-data formated as a C-struct."""
        sample_str = ','.join(['0x{:02X}'.format(sample) for sample in self.data])
        return (self._WAV_STRUCT_FMT.format(name=self.name, length=self.data_size),
                self._WAV_STRUCT_DATA_FMT.format(name=self.name,
                                                 length=self.data_size,
                                                 data=sample_str))


def main():
    wav = Wav('dog')
    wav.load('dog.wav')
    wav.trim()
    wav.to_c_struct()
    return 0


if __name__ == '__main__':
    exit(main())

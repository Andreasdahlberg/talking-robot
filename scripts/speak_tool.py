#!/usr/bin/env python3
# -*- coding: utf-8 -*

"""Tool for generating wav-files from text."""

import subprocess
import tempfile
from espeakng import ESpeakNG

class SpeakTool(object):

    def __init__(self):
        self._tmp_in_file = tempfile.NamedTemporaryFile()
        self._tmp_out_file = tempfile.NamedTemporaryFile()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self._tmp_in_file.close()
        self._tmp_out_file.close()

    def _convert(self, bit_rate):
        command = [
            'ffmpeg',
            '-i', self._tmp_in_file.name,
            '-ar', str(bit_rate),
            '-c:a', 'pcm_u8',
            '-flags', '+bitexact',
            '-y',
            '-f', 'wav',
            self._tmp_out_file.name
        ]

        command_str = ' '.join(command)
        subprocess.call(command_str, shell=True)

    def text_to_speech(self, text, voice='en-us', bit_rate=8000):
        speak = ESpeakNG(volume=200)
        speak.voice = voice

        wav_data = speak.synth_wav(text)
        self._tmp_in_file.seek(0)
        self._tmp_in_file.write(wav_data)

        self._convert(bit_rate)


    def save(self, output_file):
        self._tmp_out_file.seek(0)
        output_file.write(self._tmp_out_file.read())



def main():
    with SpeakTool() as speak_tool:
        speak_tool.text_to_speech('hello')
        speak_tool.save('hello.wav')

        speak_tool.text_to_speech('I like ice-cream')
        speak_tool.save('ice_cream.wav')
    return 0


if __name__ == '__main__':
    exit(main())

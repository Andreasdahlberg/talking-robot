#!/usr/bin/env python3
# -*- coding: utf-8 -*

import json
import tempfile
import speak_tool
import wav2cstruct


INCLUDE_FMT = '#include "{name_lower}.inc"'
ID_DEFINE_FMT = '#define AUDIOLIBRARY_{name_upper}_ID   {id}'
CASE_FMT = """      case AUDIOLIBRARY_{name_upper}_ID:
            return CastToSoundPtr(&{name_lower});
"""


def main():


    names = []
    ids = []
    index = 0

    with open('words.json') as json_file:
        json_entries = json.loads(json_file.read())

    with speak_tool.SpeakTool() as st:
        for json_entry in json_entries:
            print(json_entry)
            st.text_to_speech(text=json_entry['words'], voice='mb-sw1')

            with tempfile.NamedTemporaryFile() as tmp:
                st.save_f(tmp)
                wav = wav2cstruct.Wav(json_entry['name'])
                wav.load(tmp.name)

            wav.trim()

            c_struct = wav.to_c_struct()
            with open(json_entry['name'] + '.inc', 'w') as output_file:
                output_file.write(c_struct[0])
                output_file.write(c_struct[1])

            names.append(json_entry['name'])
            ids.append(index)
            index += 1

    includes = '\n'.join([INCLUDE_FMT.format(name_lower=name.lower()) for name in names])
    defines = '\n'.join([ID_DEFINE_FMT.format(name_upper=name.upper(), id=index) for index, name in enumerate(names)])
    cases = '\n'.join([CASE_FMT.format(name_upper=name.upper(), name_lower=name.lower()) for name in names])


    with open('audio_library_template.h') as template_h:
        template_fmt = template_h.read()

    with open('audio_library.h', 'w') as h_file:
        h_file.write(template_fmt.format(id_defines=defines))

    with open('audio_library_template.c') as template_c:
        template_fmt = template_c.read()

    with open('audio_library.c', 'w') as c_file:
        c_file.write(template_fmt.format(includes=includes, cases=cases))

    return 0

if __name__ == '__main__':
    exit(main())

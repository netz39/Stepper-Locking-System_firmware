#!/usr/bin/python3
from subprocess import Popen, PIPE
import re
import glob
import os
import sys

def removeNoneAcii(str):
    return ''.join([i if ord(i) < 128 else ' ' for i in str])

def analyzeAndCheckStaticLib(filepath):
    # start nm and let it retrieve the names of all symbols
    process = Popen(["nm", filepath], stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    if exit_code != 0:
        print("error calling nm command")
        exit(1)

    output = removeNoneAcii(str(output))
    entries = output.split(sep="\\n")

    # extract letter (type) and function name from entries
    # e.g. '00000001 T Error_Handler'   first match is T, second one is Error_Handler
    # all undefined functions without number are not matched and don't matter
    # entry names starting with $ also dont matter
    extracted = []
    for e in entries:
        res = re.findall(r"\d+?\s(\w)\s([\w\d_]+)\b", e)
        if len(res) == 1:
            type = res[0][0]
            name = res[0][1]
            extracted.append({'type': type, 'name': name})
        # else:
        #    print(e)

    # extract all duplicate symbols and their type
    duplicates = {}
    for i in extracted:
        for j in extracted:
            if i is j:
                continue
            if i['name'] == j['name']:
                if not i['name'] in duplicates:
                    duplicates[i['name']] = set()
                duplicates[i['name']].add(str(i['type']).lower())
                duplicates[i['name']].add(str(j['type']).lower())


    foundDuplicatesWithDifferentTypes = False
    for e in duplicates:
        if len(duplicates[e]) > 1:
            if not foundDuplicatesWithDifferentTypes:
                print("Analyzing: " + str(os.path.basename(filepath)))
            print("Symbol " + str(e) + "is defined multiple times with types: " + str(duplicates[e]))
            foundDuplicatesWithDifferentTypes = True

    if foundDuplicatesWithDifferentTypes:
        print("-------------------\n"
              "The Linker will not necessarily select the appropriate function from a static library "
              "without you defining --whole-archive.\nAs CMake doesn't have a convenient way of doing this. Please "
              "change your library type from STATIC to OBJECT so you don't bugs due to wrongly linked applications. "
              "See core issue #3.\n"
              "-----------------")
        exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise "not enough parameters"

    path = sys.argv[1]
    text_files = glob.glob(path + "/**/*.a", recursive=True)
    for file in text_files:
        analyzeAndCheckStaticLib(file)
    print("Checked " + str(len(text_files)) + " static libraries, all ok.")

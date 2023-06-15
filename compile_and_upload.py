import zipfile as zf
import glob
import re
import subprocess as sp
import sys

def print_message(message):
    print("----------------------------------------------------------------------------------")
    print(message)
    print("----------------------------------------------------------------------------------")


def create_zip_library():
    print_message("Creating zip library...")
    with zf.ZipFile("internal-client-arduino.zip", "w") as f:
        for file in glob.glob("include/*.h") + glob.glob("include/**/*.hpp", recursive = True) \
                    + glob.glob("source/**/*.cpp", recursive = True) + glob.glob("source/**/*.c", recursive = True) \
                    + glob.glob("source/**/*.h", recursive = True):
            
            # Arduino library can't have any subdirectories -> writing to root dir without path
            f.write(file, "internal-client-arduino/" + re.sub('.*/', '', file))

def reinstall_zip_library():
    print_message("Reinstalling zip library as Arduino library...")
    sp.run(["arduino-cli", "lib", "uninstall", "internal-client-arduino"])
    sp.run(["arduino-cli", "lib", "install", "--zip-path", "./internal-client-arduino.zip"])

def compile_sketch():
    print_message("Compiling opta_implementation.ino sketch with internal-client-arduino library...")
    result = sp.run(["arduino-cli", "compile", "-b", "arduino:mbed_opta:opta", "opta_implementation/opta_implementation.ino"])

    if(result.returncode != 0):
        sys.exit(1)

def upload_to_opta(port):
    print_message("Uploading compiled sketch to Arduino Opta on port "+ port + "...")
    result = sp.run(["arduino-cli", "upload", "-p", port, "-b", "arduino:mbed_opta:opta", "opta_implementation/opta_implementation.ino"])

    if(result.returncode != 0):
        sys.exit(1)



if(len(sys.argv) != 2):
    print("ERROR: Expecting exactly one argument describing port of connected Arduino Opta", file = sys.stderr)
    sys.exit(1)

create_zip_library()
reinstall_zip_library()
compile_sketch()
upload_to_opta(sys.argv[1])


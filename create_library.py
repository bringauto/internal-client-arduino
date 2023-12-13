import zipfile as zf
import glob
import re
import subprocess

PATTERN = '.*/'  # renaming for better readability

def init_update_submodules():
    # Initialize and Update the Submodules
    subprocess.run("git submodule update --init --recursive", shell=True, check=True)


def find_files(patterns):
    # This function finds files based on the list of patterns
    files = []
    for pattern in patterns:
        files.extend(glob.glob(pattern, recursive=True))

    return files


def create_zip_library():
    with zf.ZipFile("internal-client-arduino.zip", "w") as f:
        # For loop for all necessary files from Fleet Protocol repository
        fleet_patterns = ["libs/fleet-protocol/lib/common_headers/**/*.h",
                          "libs/fleet-protocol/lib/internal_client/**/*.h"]
        for file in find_files(fleet_patterns):
            filename = re.sub(PATTERN, '', file)
            f.write(file, "internal-client-arduino/" + filename)

        arduino_patterns = ["include/**/*.hpp",
                            "source/**/*.cpp",
                            "source/**/*.c",
                            "source/**/*.h"]
        # Arduino library can't have any subdirectories -> writing to root dir without path
        for file in find_files(arduino_patterns):
            f.write(file, "internal-client-arduino/" + re.sub(PATTERN, '', file))

    print("Zip library created")

if __name__ == '__main__':
    init_update_submodules()  # initialize and update submodules
    create_zip_library()
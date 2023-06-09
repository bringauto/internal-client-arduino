import zipfile as zf
import glob
import re

def remove_path(filename : str):
    return re.sub('.*/', '', filename)

with zf.ZipFile("internal-client-arduino.zip", "w") as f:
    for file in glob.glob("include/*.h") + glob.glob("include/**/*.hpp", recursive = True) \
                + glob.glob("source/**/*.cpp", recursive = True) + glob.glob("source/**/*.c", recursive = True) \
                + glob.glob("source/**/*.h", recursive = True):
        f.write(file, "internal-client-arduino/" + remove_path(file))

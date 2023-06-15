WIP

# Requirements

 - curl

# Usage

## Generating Protobuf files (optional)

 > Generating InternalProtocol protobuf files is not necessary if InternalProtocol declaration in InternalProtocol.proto file is not changed.

From root directory go to source/protobuf and run this command:

```bash
python ../../libs/nanopb/generator/nanopb_generator.py InternalProtocol.proto
```

Now in current directory should be generated 2 new files: `InternalProtocol.pb.c` and `InternalProtocol.pb.h`. Include the header file to use InternalProtocol implementation. These generated files are moved in compiled directory. Some headers from nanopb submodule with universal purpose declarations are in source/nanopb directory.

## Installing arduino-cli

In repo root directory run:

```bash
python install_arduino_cli.py
```

This script installs arduino-cli executable into /usr/bin and initializes ~/.arduino15 directory. Now check if arduino-cli is executable. If not update your $PATH to include /usr/bin or change the script to install arduino-cli to another location.

## Compile and upload sketch and library to Arduino Opta

Now with installed arduino-cli you need to find out port on which Arduino Opta is connected. Run this:

```bash
arduino-cli board list
```

The output should look like this:

```
Port         Protocol Type              Board Name FQBN                   Core             
/dev/ttyACM0 serial   Serial Port (USB) Opta       arduino:mbed_opta:opta arduino:mbed_opta
```

The port (in example is /dev/ttyACM0) we will use as mandatory argument to next python script. Run this:

```bash
python compile_and_upload.py <your_port>
```

This script will zip all neccessary internal-client library files in this repo and install as library usable by arduino-cli (and removes previous internal-client-arduino library if neccessary). Then the script compiles opta_implementation/opta_implementation.ino arduino sketch and internal-client library and uploads it to Arduino Opta connected on entered port.

 > If you have problems with uploading and you're on Linux, try [this](https://support.arduino.cc/hc/en-us/articles/9005041052444-Fix-udev-rules-on-Linux). Or put the Opta board in bootloader mode (double-press reset button) and try again.

 > For uploading to different arduino board than Opta you need to change both install_arduino_upload.py and compile_and_upload arduino-cli commands. Look [here](https://arduino.github.io/arduino-cli/0.21/getting-started/).

 > The compile_and_upload script defautly compiles opta_implementation/opta_implementation.ino. If you want to compile different sketches you need to change the script.

# TODO

 - Think about timeouts in TCP communication implementation
 - Debug internal_client implementation on Arduino Opta
 - Finish README
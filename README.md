This is Internal client library implementation for Arduino boards. At this moment, this library is tested only on Arduino Opta board. To use the library with other Arduino boards, more testing is required.

# Requirements

## Tools
 - python, curl

## Ethernet library initialization

This library uses TCP communication implemented in [Ethernet library](https://reference.arduino.cc/reference/en/libraries/ethernet/). Before use of internal-client library the Ethernet library needs to be initialized with calling `Ethernet.begin()`. The minimal initialization could be:

```cpp
byte mac[] = { 0x5E, 0xA2, 0xBE, 0xCF, 0xFE, 0x8D };
Ethernet.begin(mac);
```

# Usage

## Generate Protobuf files (optional)

 > Generating InternalProtocol protobuf files is not necessary if you've changed InternalProtocol declaration in InternalProtocol.proto file or options in InternalProtocol.options.

From root directory go to source/bringauto/internal_client/arduino/protobuf and run this command:

```bash
python ../../../../../libs/nanopb/generator/nanopb_generator.py InternalProtocol.proto
```

Now in current directory should be generated 2 new files: `InternalProtocol.pb.c` and `InternalProtocol.pb.h`. Include the header file to use InternalProtocol implementation. These generated files are moved in compiled directory. Some headers from nanopb submodule with universal purpose declarations are in source/nanopb directory.

 > This generator uses InternalProtocol.options, where max sizes of some structures are defined to avoid using callbacks in nanopb. More about callbacks in nanopb [here](https://jpa.kapsi.fi/nanopb/docs/concepts.html). 

## Create zip library usable by arduino-cli

In repo root directory run:

```bash
python create_library.py
```

This script will zip all necessary files into internal-client-arduino.zip, which can be used as zip library by arduino-cli (command line utility for Arduino boards).

## Install arduino-cli

To install internal-client-arduino library as Arduino library, you need arduino-cli. You can install it using official install script with this command:

```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=. sh
```

> This command installs arduino-cli to the current directory. If you want to install it to different directory, change the `BINDIR` variable. You can also change it to path, which is in your `$PATH` variable to make arduino-cli globally available.

> The arduino-cli utility is not dependent on installation directory. The installation script initializes ~/.arduino15 directory, which arduino-cli only uses. This means that after installation of arduino-cli, you can freely copy and move arduino-cli to another directories and maintain desired behaviour of arduino-cli.

> For more information or installation option, check official [Arduino guide](https://arduino.github.io/arduino-cli/0.33/installation/).

## Configure arduino-cli

Now you need to configure arduino-cli to allow installation of zip library. Run these commands:

```bash
./arduino-cli config init
./arduino-cli config set library.enable_unsafe_install true
```

## Install internal-client-arduino as Arduino library

Now you can install internal-client-arduino library with next command. You need to pass the path to library zip created by create_library.py script.

```bash
./arduino-cli lib install --zip-path <path-to-library-zip>
```

> To reinstall Arduino library, you can simply run previous install command. The arduino-cli automatically removes installed library with same name (the name of zip file is considered) before installation.

# Limitations

Variable length message variables in .proto files (like strings or bytes) needed to be changed to avoid using callbacks. This was bug in nanopb, when using variable length variables inside oneofs in messages. It was maybe fixed in [nanopb 0.4](https://jpa.kapsi.fi/nanopb/docs/whats_new.html), but the documentation is quite poor. For now the size of variable length variables was changed to fixed length variables of max size described in source/protobuf/InternalProtocol.options. To change max size of these variables you need to change values in this file and regenerate protobuf files.

# Notes

 - This repository follows directory tree Bringauto rules, but include's path are not. That is because Arduino libraries can't have any subdirectories. The create_library.py script take all source files and insert them into zip with only one root directory with all files. Therefore, the include's path are excluded to proper functionality as Arduino library.
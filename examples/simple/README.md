This is simple example of usage of internal-client-arduino library in Arduino sketch (specific Arduino source file recognised with `.ino` file extension, more info [here](https://docs.arduino.cc/learn/programming/sketches)). The code in sketch initializes connection, prints the connection result to serial output, sends 5 statuses, receives commands and then closes the connection. The main goal of this example is to show steps to successfully get the library up and running and make it usable in Arduino sketches.

# Requirements

 - internal-client-arduino library, build by [create_library.py](../../../create_library.py)
 - arduino IDE 2 or arduino-cli

# Usage - arduino IDE 2
Load the library into Arduino IDE 2. Then open the sketch and upload it to your Arduino board.
**If your board is not in the output and you are on Linux, you might need to fix udev rules. This is described in this [link](https://support.arduino.cc/hc/en-us/articles/9005041052444-Fix-udev-rules-on-Linux).**


# Usage - arduino-cli
## Enable use of arduino-cli in current directory (optional)

For more convenient use of arduino-cli, you might want to use it in current directory. To achieve this, simply move or copy the arduino-cli file to current directory.

## Connect Arduino board to your computer

Connect Arduino board to your computer. Then try, if arduino-cli sees the board, run this:

```bash
./arduino-cli board list
```

The output will show you all connected boards to your computer. It should look similarly to this:

```
Port         Protocol Type              Board Name FQBN                   Core             
/dev/ttyACM0 serial   Serial Port (USB) Opta       arduino:mbed_opta:opta arduino:mbed_opta
```

> If your board is not in the output and you are on Linux, you might need to fix udev rules. This is described in this [link](https://support.arduino.cc/hc/en-us/articles/9005041052444-Fix-udev-rules-on-Linux).

Find your board and note the Port, FQBN and Core values. These values will be used in next steps.

## Install your board core

Firstly you need to download core for your Arduino board. Pass the Core value to the next command:

```bash
./arduino-cli core install <Core>
```

At this moment you are able to compile and upload sketches to your board.

## Compile and upload sketch to Arduino board

To compile your sketch for specific Arduino board, run this:

```bash
./arduino-cli compile --fqbn <FQBN> <sketch-path>
```

To upload the sketch to your board, run next command. As arguments use Port, FQBN and path of your sketch:

```bash
./arduino-cli upload -p <Port> --fqbn <FQBN> <sketch-path>
```

This command will upload your sketch with all installed libraries to your Arduino board connected on entered port.

 > If you have problems with uploading and you're on Linux, try [this](https://support.arduino.cc/hc/en-us/articles/9005041052444-Fix-udev-rules-on-Linux). Or put the board in bootloader mode (find how to get into bootloader mode for your board) and try again.

 > To compile and upload with one command, you can use `compile` command with `-u` option. Example of this command:

 ```bash
./arduino-cli compile -p <Port> -u --fqbn <FQBN> <sketch-path>
 ```

> For more information, check official [arduino-cli documentation](https://arduino.github.io/arduino-cli/0.33/).

## Using arduino-cli serial monitor

To use serial monitor (get output from Serial.print functions), run this command:

```bash
./arduino-cli monitor -p <Port>
```

import getpass
import sys
import subprocess as sp

def print_message(message):
    print("----------------------------------------------------------------------------------")
    print(message)
    print("----------------------------------------------------------------------------------")

def configure_arduino_cli():
    print_message("Configuring arduino-cli...")
    sp.run(["arduino-cli", "config", "init"])
    sp.run(["arduino-cli", "config", "set", "library.enable_unsafe_install", "true"])


def install_arduino_cli():
    print_message("Downloading arduino-cli...")
    result = sp.run(["curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=. sh"],
                    shell = True, capture_output = True, text = True)

    if(result.returncode == 0):
        print("arduino-cli downloaded")
    else:
        print("arduino-cli download failed")
        sys.exit(1)
    
    print_message("Installing arduino-cli to /usr/bin...")

    password = getpass.getpass("Please enter your password: ")
    
    proc = sp.Popen(["sudo", "-S",  "mv",  "arduino-cli", "/usr/bin"], stdin = sp.PIPE, stderr = sp.PIPE, stdout = sp.PIPE)

    proc.communicate(password.encode())
    
    sp.run(["arduino-cli", "core", "update-index"])
    sp.run(["arduino-cli", "core", "install", "arduino:mbed_opta"])

    configure_arduino_cli()



install_arduino_cli()

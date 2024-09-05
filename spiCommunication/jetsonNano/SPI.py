# python 2.7

import spidev
from enum import Enum

class MOD_SEL_ENUM_E(Enum):
    MOD_SEL_3_AXIS_BRIDGE = 1   # Module selection for 3-axis bridge
    MOD_SEL_TRACTION = 2        # Module selection for traction
    MOD_SEL_DIRECTION = 3       # Module selection for direction
    MOD_SEL_POWER = 4           # Module selection for power


class CMD_3_AXIS_BRIDGE_ENUM_E(Enum):
    SET_COORDINATE_X = 1        # Set coordinate X
    SET_COORDINATE_Y = 2        # Set coordinate Y
    HARVEST = 3                 # Harvest
    CALIBRATE = 4               # Calibrate
    READ_COORDINATES_X = 10     # Read coordinates X
    READ_COORDINATES_Y = 11     # Read coordinates Y
    READ_INTERNAL_STATE = 12    # Read internal state

class CMD_TRACTION_ENUM_E(Enum):
    MOVE_FORWARD = 1            # Move forward
    MOVE_BACKWARDS = 2          # Move backwards
    STOP = 3                    # Stop
    READ_TRACTION_STATE = 4     # Read internal state

class CMD_DIRECTION_ENUM_E(Enum):
    SET_DIRECTION_ANGLE = 1     # Set direction angle
    CALIBRATE_DIRECTION = 2     # Calibrate
    READ_DIRECTION_STATE = 3    # Read internal state

class CMD_POWER_ENUM_E(Enum):
    READ_POWER_STATUS = 1       # Read power status

class ERROR_CODE_ENUM_E(Enum):
    ACK = 1                     # Command ACK
    INVALID_MODULE = -1         # Invalid Module
    INVALID_COMMAND = -2        # Invalid Command
    PAYLOAD_OUT_OF_RANGE = -3   # Payload Out Of Supported Range


def send_message(module_selection, command, payload):
    # Prepare message in MSB-first order (high byte first)
    high_byte = (payload >> 8) & 0xFF  # Extract the high byte of the payload
    low_byte = payload & 0xFF           # Extract the low byte of the payload
    message = [module_selection, command, high_byte, low_byte]

    # Send message (4 bytes) and receive response (4 bytes)
    response = spi.xfer2(message)  # Send 4 bytes to the slave

    # Master must clock additional 4 bytes to read the slave's response
    reply = spi.xfer2([0, 0, 0, 0])  # Clock 4 bytes to receive the reply from the slave

    return reply

# Initialize SPI communication
spi = spidev.SpiDev()       # Create an instance of the SpiDev class
spi.open(0, 0)              # Open SPI bus 0, device 0 (CS0 on Jetson)
spi.max_speed_hz = 50000    # Set SPI clock speed to 50kHz (adjust as necessary)
spi.mode = 0b00             # Set SPI mode 0 (CPOL=0, CPHA=0)

# Example usage of send_message function
module_selection = MOD_SEL_3_AXIS_BRIDGE  # Example module selection for 3-axis-bridge
command = SET_COORDINATE_X          # Example command (e.g., read X coordinate)
payload = 15000       # Example payload to send (ignored in read commands)

# Send the message to the slave and receive the response
reply = send_message(module_selection, command, payload)

# Print the response received from the slave
print("Reply: %s" % reply)  # Use the Python 2.7 print statement syntax

# Close the SPI connection when done
spi.close()

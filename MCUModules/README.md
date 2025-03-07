# CAN Communications Specifications

CAN protocol communicates in 8-bit data frames by default.
We use multi-byte communication to send 4 bytes by turns.
This prococol is shared by every node in the system and is used for both low level commands and high level commands.
The MCU connect to a MCP2515 which has a TJA1050 intergrated and we communicate with it via SPI


Byte order for Payload MSB
Clock speed: 16 MHz SPI clock
Speed Set: 500KBPS - to be noise resistant over 3m distances


Command and Read Message Format:
command - byte [1]  
payload - byte [2,3]  


Example for setting coordinate y to 15000:  
15,000 = 0x3A98 so split into 2 bytes. High Byte: [0x3A] Low Byte: [0x98]  
High byte fist.

Sender sends bytes [0x98][0x3A][2]
uint8_t canMsg[MESSAGE_LENGTH];
canMsg[0] = cmd;
// Convert value to big-endian format (MSB first)
canMsg[1] = (payload >> 8) & 0xFF;  // High byte
canMsg[2] = payload & 0xFF;         // Low byte

Receiver receives [0x98][0x3A][2] 
uint8_t cmd = canMsg[0];
uint16_t receivedPayload = (canMsg[1] << 8) | canMsg[2];

Check functions my_can_receive() and my_can_send()

Then Mega proceeds with the same logic to reply to the command filling the payload with "1" for ACK

We are using the lib <iso-tp.h> to handle this mechanic

## Module Selection
| Module Selection Index | Module Name                        | Description                                            |
|------------------------|------------------------------------|--------------------------------------------------------|
| 0                      | MOD_SEL_JETSON                     | Module selection for Jetson Nano                       |
| 1                      | MOD_SEL_MCU_MAIN                   | Main MCU module, traction right and turbines           |
| 2                      | MOD_SEL_HW_DIR_RIGHT               | Individual HW module direction right side              |
| 3                      | MOD_SEL_HW_DIR_LEFT                | Individual HW module direction left side               |
| 4                      | MOD_SEL_HW_X_TRACTION_LEFT         | Individual HW module X axis and traction left side     |
| 5                      | MOD_SEL_HW_Y                       | Individual HW module Y axis                            |
| 6                      | MOD_SEL_HW_Z                       | Individual HW module Z axis                            |
| 7                      | MOD_SEL_HW_CUT                     | Individual HW module harvesting                        |

## Command List
| Cmd Idx | Description                               | Range          | Reply        |
|---------|-------------------------------------------|----------------|--------------|
| 1       | Set coordinate X                          | [0-65535]      | [0,1]        |
| 2       | Set coordinate Y                          | [0-65535]      | [0,1]        |
| 3       | Harvest                                   |                | [0,1]        |
| 4       | Calibrate XY                              |                | [0,1]        |
| 5       | Read coordinates X                        |                | [0-65535]    |
| 6       | Read coordinates Y                        |                | [0-65535]    |
| 7       | Read internal state of 3-axis-bridge axis |                | [0-5]        |
| 20      | Move forward                              |                | [0,1]        |
| 21      | Move backwards                            |                | [0,1]        |
| 22      | Stop                                      |                | [0,1]        |
| 23      | Read internal state traction module       |                | [0-3]        |
| 30      | Set direction angle                       | [0,200]        | [0,1]        |
| 31      | Calibrate Direction                       |                | [0,1]        |
| 32      | Read internal state direction module      |                | [0-4]        |
| 40      | Read power status                         |                | [0-100]      |

## High Level Internal States Table
| Module        | List                                                                      | Code  |
|---------------|---------------------------------------------------------------------------|-------|
| 3-axis-bridge | INIT(not calibrated), CALIBRATING, HARVESTING, READY, POSITIONING, FAULTY | [0-5] |
| Traction      | READY, MOVINGFORWARD, MOVINGBACKWARDS, FAULTY                             | [0-3] |
| Direction     | INIT(not calibrated), CALIBRATING, READY, POSITIONING, FAULTY             | [0-4] |

For every command MCU replies with 1 for ACK and "ERROR"
## Error list
| Error							| Code	|
|-------------------------------|-------|
| Command ACK					| 1		|		
| Invalid Module				| -1	|
| Invalid Command				| -2	|
| Payload Out Of Suported Range	| -3	|


The following do not Require SPI Communication. The are internally to the Jetson Nano
## Plant Altitude Meter Module
| Inputs                              | Outputs                                             |
|-------------------------------------|-----------------------------------------------------|
| read measure                        | Altitude value                                      |
| read internal state                 | READY, FAULTY                                       |

## Cut Module Camera
| Inputs                              | Outputs                                             |
|-------------------------------------|-----------------------------------------------------|
| get plant's position and classification | List of aromatics Positions in pixel coordinates xy |

## Multiple Cameras Tracking Module
| Inputs                              | Outputs                                             |
|-------------------------------------|-----------------------------------------------------|
| get plant's position                | List of aromatics Positions in pixel coordinates xy |

## Line Navidation Module
| Inputs                              | Outputs                                             |
|-------------------------------------|-----------------------------------------------------|
| move until plant                    | Command reply finished                              |

## Plants map history
| Inputs                              | Outputs                                             |
|-------------------------------------|-----------------------------------------------------|
| add plant, remove plant, read plants | List of aromatics Positions in cm coordinates xy    |



# SPI Communications Specifications

The Arduino Mega 2560 communicates over SPI in 8-bit frames by default. Each SPI transmission transfers 1 byte (8 bits) of data at a time.
We use multi-byte communication to send 4 bytes by turns.  
Jetson sends 4 bytes and then Mega replies with 4 bytes back.  

Byte order for Payload MSB
SPI clock speed: 8 MHz SPI clock (8 Mb\s, 1 MB/s)  


Command and Read Message Format:
module selection - byte [0]  
command - byte [1]  
payload - byte [2,3]  


Example for setting coordinate y to 15000:  
15,000 = 0x3A98 so split into 2 bytes. High Byte: [0x3A] Low Byte: [0x98]  
High byte fist.

Jetson -> Mega sends bytes [1][2][0x3A][0x98]  
Mega receives [0x98][0x3A][2][1]  
module 	= message[0] -> 1  
command = message[1] -> 2  
payload = ((receivedMessage[2] << 8) | receivedMessage[3]) -> 0x3A98

Then Mega proceeds with the same logic to reply to the command filling the payload with "1" for ACK

## Module Selection
| Module 		    | Code	|
|---------------|-------|
| 3-axis-bridge | 1		  |
| Traction		  | 2		  |
| Direction		  | 3		  |
| Power			    | 4		  |


## 3-axis-bridge Module
| Command Type  | Command                             | Code	  | Payload                                             						          | Code			    |
|---------------|-------------------------------------|---------|---------------------------------------------------------------------------|---------------|
| Command       | set coordinate x                    | 1		    | value                                         							              | [0, 65535]	  |
| Command       | set coordinate y                    | 2		    | value                                         							              | [0, 65535]	  |
| Command       | harvest                             | 3		    | 0                                         								                | 0				      |
| Command       | calibrate                           | 4		    | 0                                         								                | 0				      |
| Read			    | read coordinates x                  | 10		  | current coordinates x in motor pulses               						          | [0, 65535] 	  |
| Read			    | read coordinates y                  | 11		  | current coordinates y in motor pulses               						          | [0, 65535] 	  |
| Read			    | read internal state                 | 12		  | INIT(not calibrated), CALIBRATING, HARVESTING, READY, POSITIONING, FAULTY | [1,2,3,4,5,6] |

## Traction Module
| Command Type  | Command                             | Code	  | Payload                                             						          | Code			    |
|---------------|-------------------------------------|---------|---------------------------------------------------------------------------|---------------|
| Command       | move forward                        | 1		    | 0     	                                								                  | 0				      |
| Command       | move backwards                      | 2		    | 0     	                                								                  | 0				      |
| Command       | stop                                | 3		    | 0     	                                								                  | 0				      |
| Read			    | read internal state                 | 4		    | READY, MOVINGFORWARD, MOVINGBACKWARDS, FAULTY       						          | [1,2,3,4]		  |


## Direction Module
| Command Type  | Command                             | Code	  | Payload                                             						          | Code			    |
|---------------|-------------------------------------|---------|---------------------------------------------------------------------------|---------------|
| Command       | set direction angle                 | 1		    | value                              										                    | [-45,45]		  |
| Command       | calibrate                           | 2		    | 0                              											                      | 0				      |
| Read			    | read internal state                 | 3		    | INIT(not calibrated), CALIBRATING, READY, POSITIONING, FAULTY 			      | [1,2,3,4,5]	  |

## Power Module
| Command Type  | Command                             | Code	  | Payload                                             						          | Code			    |
|---------------|-------------------------------------|---------|---------------------------------------------------------------------------|---------------|
| Read			    | read power status                   | 1		    | Battery status percentage 												                        | [0,100]		    |

For every command MCU replies with 1 for ACK and "ERROR"
## Error list
| Error							            | Code	|
|-------------------------------|-------|
| Command ACK					          | 1		  |		
| Invalid Module				        | -1	  |
| Invalid Command				        | -2	  |
| Payload Out Of Suported Range	| -3	  |


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



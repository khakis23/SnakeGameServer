# Online 2 Player Snake 
## Server
### Author: Tyler Black

The server program for 2 player snake game ([client repo found here](https://github.com/khakis23/SnakeGameClient)).


## Architecture
![Server Architecture Diagram](readmeAssets/ServerArch.png)

## Game Codes

| Code        | Direction     | Payload        | Description                                                         |
|-------------|---------------|----------------|---------------------------------------------------------------------|
| **SEAT**        | Server → Client | —              | —                                                                   |
| **START**       | Server → Client | `player`       | Both users connected, start application (before set, first message) |
| **COLLISION**   | Server → Client | `player`       | Snake collided                                                      |
| **APPLE**       | Server → Client | `x,y`          | Sends new apple coordinates                                         |
| **GROW**        | Server → Client | `player`       | Tells client that a `player` should grow                            |
| **SCORE**       | Server → Client | `s1,s2`        | Sends updated scores for both players                               |
| **SET**         | Server → Client | —              | Both players ready, start game                                      |
| **DISCONNECT**  | Server → Client | `player`       | Notifies that a `player` disconnected                               |
| **MOVE**        | Both             | `x,y`          | Movement update (player → server, server → client)                  |
| **READY**       | Client → Server | —              | Client indicates it is ready to start game                          |
| **RESET**       | Client → Server | —              | Client requests a full game reset                                   |
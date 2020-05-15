# Tilta Nucleus N Protocol

First night tests and writeup. Please check all info with tester (multimeter) before connecting, I could be wrong

It uses standart UART 115200 to communicate to other Tilta devices. Both handunit and motor send exact same commands

## Pins on handunit

```
|-----------------|
| TX   +   +  GND |
|          +  ??? |
| RX?  +   +  GND |
|-----------------|
```

## Pins on focus motor

```
|-----------------------|
| ---|----|----|------\ |
| ---|----|----|------/ |
| GND  TX   RX   4.2v   |
|-----------------------|
```

## Commands

All commands are hex 15 chars long, starting with ":"

#### Sent every sec. Assume ping or heartbeat

```
:96060002000161
:C90680000000B1
```
#### Sent when focus changed by handunit. 

010601 - focus cmd

75 - ? 

270D - focus value 

4F - ? (checksum maybe)

```
:010601 75 270D 4F
```




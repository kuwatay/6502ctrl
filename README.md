# 6502ctrl
AVR-based supervisor for a w65c02 microcomputer based on z80ctrl https://github.com/jblang/z80ctrl

![](hardware/rev2/6502ctrl-rev2-image.jpg)

6502ctrl has the following features:
* Support W65C02 and 64KB(128KB) ram
* Console and other I/O are handled by DMA (ultra fast WAI instruction)
* Monitor with file management, memory inspector, and built-in 6502 disassembler
* Single-step debugging, breakpoints, and watchpoints to trace 6502 bus activity in real-time
* Arduino Shield style hardware (6502ctrl-mega)
* Flexible MIT-licensed firmware inhelited from z80ctrl

# Documentation

## Application
https://github.com/kuwatay/6502ctrl/wiki/Application

## Hardware note
https://github.com/kuwatay/6502ctrl/wiki/Hardware-of-6502ctrl-mega

## Supervisor sample session
https://github.com/kuwatay/6502ctrl/wiki/Sample-session-of--Supervisor

## DMA handling
https://github.com/kuwatay/6502ctrl/wiki/DMA-I-O-request

## MIT license
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

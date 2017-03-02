# Snake Program for MSP430

This is a simple school project for the MSP430 launchpad that makes use LCD both onboard buttons as well as 4 additional off-board buttons wired between their respective pins and ground for use the MSP430's onboard pull-up resistors. Due to the constraints of the initial assignment, it uses a simple busy wait for a delay rather than using interrupts.
**Setup Information**:
To set up, wire up pins P2.0 and P9.1 to two buttons; these will be used to increase and decrease the speed the snake moves. P9.2 and P9.4 are set to increase and decrease the length of the snake. 

// DS1722.h
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>


// configuration register is accessed with 00h for reads and 80h for writes
// data is written to config reg MSb first for SPI communication

void ds1722_init(void);
// DS1722.c
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>


// want to config SPI slave before themaster sends a clock
// to avoid undesired data transmission

// Master at full-duplex starts to communicate whent heSPI 
// is enabled and TXFIFO is not empty, or with the next write to TXFIFO

// DS1722 continuously CONVERTS in the background (after configuring it)
// but neveractually drivesthe spi bus by itself
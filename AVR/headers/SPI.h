#ifndef __SPI_H__
#define __SPI_H__


#define DDR_SPI DDRB
#define DD_SS	DDRB4
#define DD_MOSI DDRB5
#define DD_MISO DDRB6
#define DD_SCK  DDRB7

unsigned char receivedData;

void SPI_MasterInit(){
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(unsigned char cData){
	/* Start transmission */
	SPDR = cData;
	
	//Set SS Low
	PORTB &= 0xEF;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)));
	
	//Set SS High
	PORTB |= 0x10;
}

void SPI_SlaveInit(void){
	/* Set MISO output, all others input */
	DDR_SPI = (1 << DD_MISO);
	/* Enable SPI */
	SPCR = (1 << SPE);
}

unsigned char SPI_SlaveReceive(void){
	/* Wait for reception complete */
	while(!(SPSR & (1 << SPIF)));
	/* Return Data Register */
	return SPDR;
}

void SPI_ISR(){
	receivedData = SPDR;

}

ISR(SPI_STC_vect){
	SPI_ISR();
}

#endif


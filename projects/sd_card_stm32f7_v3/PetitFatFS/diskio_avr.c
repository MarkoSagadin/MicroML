/*-------------------------------------------------------------------------*/
/* PFF - Low level disk control module for AVR							   */
/* Modified from (C)ChaN, 2014											   */
/*-------------------------------------------------------------------------*/

#include "pff.h"
#include "diskio.h"

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions MODIFY FOR YOUR DEVICE           */
/*-------------------------------------------------------------------------*/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "utility.h"

#define DESELECT()	(gpio_set(GPIO_PORT_CS, GPIOCS))      /* MMC CS = H */
#define SELECT()	(gpio_clear(GPIO_PORT_CS, GPIOCS))      /* MMC CS = L */

#define GPIO_PORT_CS             GPIOB
#define RCC_GPIO_PORT_CS         RCC_GPIOB
#define GPIOCS                   GPIO4
#define SPI_SD                   SPI1

#define GPIO_PORT_SPI_SD         GPIOA
#define GPIOSPI_SD_SCK           GPIO5
#define GPIOSPI_SD_MISO          GPIO6
#define GPIOSPI_SD_MOSI          GPIO7
#define RCC_SPI                  RCC_SPI1
#define RCC_GPIO_PORT_SPI        RCC_GPIOA

static void init_spi(void)
{
    rcc_periph_clock_enable(RCC_GPIO_PORT_CS);
    rcc_periph_clock_enable(RCC_GPIO_PORT_SPI);
	rcc_periph_clock_enable(RCC_SPI);

	/* Configure I/O for Card Chip select */
	DESELECT();     // Set CS high before setting as output to avoid spike
    gpio_mode_setup(GPIO_PORT_CS, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIOCS);
    gpio_set_output_options(GPIO_PORT_CS, 
                            GPIO_OTYPE_PP, 
                            GPIO_OSPEED_50MHZ, 
                            GPIOCS);


    // Setup pins as alternate SPI pins
    gpio_mode_setup(GPIO_PORT_SPI_SD, GPIO_MODE_AF, GPIO_PUPD_NONE, 
                                                    GPIOSPI_SD_SCK | 
                                                    GPIOSPI_SD_MISO |
                                                    GPIOSPI_SD_MOSI);

	gpio_set_af(GPIO_PORT_SPI_SD, GPIO_AF5, GPIOSPI_SD_SCK | 
                                            GPIOSPI_SD_MISO |
                                            GPIOSPI_SD_MOSI);


    /* Clear all SPI and associated RCC registers */
	spi_reset(SPI_SD);

	/* SPI configuration */
    spi_init_master(SPI_SD, 
                    SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_MSBFIRST);



    //spi_set_full_duplex_mode(SPI_SD);
    spi_set_master_mode(SPI_SD);
    //spi_set_unidirectional_mode(SPI_SD);

	/*
	 * Set NSS management to software.
	 *
	 * Note:
	 * Setting nss high is very important, even if we are controlling the GPIO
	 * ourselves this bit needs to be at least set to 1, otherwise the spi
	 * peripheral will not send any data out.
	 */
	spi_enable_software_slave_management(SPI_SD);
	spi_set_nss_high(SPI_SD);

    // We're using 8 bit per packet
    spi_set_data_size(SPI_SD, SPI_CR2_DS_8BIT);
    spi_fifo_reception_threshold_8bit(SPI_SD);

	spi_enable(SPI_SD);

	/* drain SPI */
	/* Wait for transfer finished. */
	//while (!(SPI_SR(SPI_SD) & SPI_SR_TXE));
	//uint16_t dummyread = SPI_DR(SPI_SD);
     
}

static BYTE spi(BYTE d)
{
    // Usually we would use here spi_xfer to transmit and receive.
    // Problem is that SPI_DR data register that we use to load and read
    // data from is by defulat in STM32F7 accessed with 32 bit pointer
    // we we load one byte into it, it will automatically expand it
    // to 16 bits. What we did was to copy everthing form inside of
    // spi_xfer function (and any called functions and replaced SPI_DR
    // acces with SPIDR8.
    //return spi_xfer(SPI_SD, d);
    

    // SPI write with uint8_t access
    SPI_DR8(SPI_SD) = (uint8_t) d;

	/* Wait for transfer finished. */
	while (!(SPI_SR(SPI_SD) & SPI_SR_RXNE));

	/* Read the data, 8 bits */
	//return SPI_DR(spi);
    return SPI_DR8(SPI_SD);
}

static void xmit_spi(BYTE d)
{
	spi(d);
}

static BYTE rcv_spi(void)
{
	return spi(0xff);
}

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0 (0x40 + 0)    /* GO_IDLE_STATE */
#define CMD1 (0x40 + 1)    /* SEND_OP_COND (MMC) */
#define ACMD41 (0xC0 + 41) /* SEND_OP_COND (SDC) */
#define CMD8 (0x40 + 8)    /* SEND_IF_COND */
#define CMD16 (0x40 + 16)  /* SET_BLOCKLEN */
#define CMD17 (0x40 + 17)  /* READ_SINGLE_BLOCK */
#define CMD24 (0x40 + 24)  /* WRITE_BLOCK */
#define CMD55 (0x40 + 55)  /* APP_CMD */
#define CMD58 (0x40 + 58)  /* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC 0x01   /* MMC ver 3 */
#define CT_SD1 0x02   /* SD ver 1 */
#define CT_SD2 0x04   /* SD ver 2 */
#define CT_BLOCK 0x08 /* Block addressing */

BYTE CardType = 0;

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
static BYTE send_cmd(BYTE  cmd, /* 1st byte (Start + Index) */
                     DWORD arg  /* Argument (32 bits) */
                     )
{
	BYTE n, res;

	if (cmd & 0x80) { /* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1)
			return res;
	}

	/* Select the card */
	DESELECT();
	rcv_spi();
	SELECT();
	rcv_spi();

	/* Send a command packet */
	xmit_spi(cmd);               /* Start + Command index */
	xmit_spi((BYTE)(arg >> 24)); /* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16)); /* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));  /* Argument[15..8] */
	xmit_spi((BYTE)arg);         /* Argument[7..0] */
	n = 0x01;                    /* Dummy CRC + Stop */
	if (cmd == CMD0)
		n = 0x95; /* Valid CRC for CMD0(0) */
	if (cmd == CMD8)
		n = 0x87; /* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive a command response */
	n = 10; /* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcv_spi();
	} while ((res & 0x80) && --n);

	return res; /* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if _USE_WRITE
	//if (CardType && ((PINB & PORTB0) == 0))
	if (CardType && ((gpio_get(GPIO_PORT_CS, GPIOCS) >> 4) == 0))
		disk_writep(0, 0); /* Finalize write process if it is in progress */
#endif

	init_spi(); /* Initialize ports to control MMC */

	DESELECT();

	for (n = 0; n<10; n++)
    {
		rcv_spi(); /* 80 dummy clocks with CS=H */
    }

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {         /* GO_IDLE_STATE */
    printf("we get here 1\n");
		if (send_cmd(CMD8, 0x1AA) == 1) { /* SDv2 */
			for (n     = 0; n < 4; n++)
				ocr[n] = rcv_spi();                 /* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) { /* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--)
					//_delay_us(100);                   /* Wait for leaving idle state (ACMD41 with HCS bit) */
                    delay(1);
				if (tmr && send_cmd(CMD58, 0) == 0) { /* Check CCS bit in the OCR */
					for (n     = 0; n < 4; n++)
						ocr[n] = rcv_spi();
					ty         = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* SDv2 (HC or SC) */
				}
			}
		} else { /* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) {
				ty  = CT_SD1;
				cmd = ACMD41; /* SDv1 */
			} else {
				ty  = CT_MMC;
				cmd = CMD1; /* MMCv3 */
			}
			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--)
				//_delay_us(100);                    /* Wait for leaving idle state */
                delay(1);
			if (!tmr || send_cmd(CMD16, 512) != 0) /* Set R/W block length to 512 */
				ty = 0;
		}
	}

	CardType = ty;
	DESELECT();
	rcv_spi();

	return ty ? 0 : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read partial sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp(BYTE *buff,   /* Pointer to the read buffer (NULL:Forward to the stream) */
                   DWORD sector, /* Sector number (LBA) */
                   UINT  offset, /* Byte offset to read from (0..511) */
                   UINT  count   /* Number of bytes to read (ofs + cnt mus be <= 512) */
                   )
{
	DRESULT res;
	BYTE    rc;
	UINT    bc;

	if (!(CardType & CT_BLOCK))
		sector *= 512; /* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) { /* READ_SINGLE_BLOCK */

		// bc = 40000;	/* Time counter */
		do { /* Wait for response */
			rc = rcv_spi();
		} while (rc == 0xFF);

		if (rc == 0xFE) { /* A data packet arrived */

			bc = 512 + 2 - offset - count; /* Number of trailing bytes to skip */

			/* Skip leading bytes */
			while (offset--)
				rcv_spi();

			/* Receive a part of the sector */
			if (buff) { /* Store data to the memory */
				do {
					*buff++ = rcv_spi();
				} while (--count);
			} else { /* Forward data to the outgoing stream */
				do {
					// FORWARD(rcv_spi());
				} while (--count);
			}

			/* Skip trailing bytes and CRC */
			do
				rcv_spi();
			while (--bc);

			res = RES_OK;
		}
	}

	DESELECT();
	rcv_spi();

	return res;
}

/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_writep(const BYTE *buff, /* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
                    DWORD       sc    /* Number of bytes to send, Sector number (LBA) or zero */
                    )
{
	DRESULT     res;
	UINT        bc;
	static UINT wc; /* Sector write counter */

	res = RES_ERROR;

	if (buff) { /* Send data bytes */
		bc = sc;
		while (bc && wc) { /* Send data bytes to the card */
			xmit_spi(*buff++);
			wc--;
			bc--;
		}
		res = RES_OK;
	} else {
		if (sc) { /* Initiate sector write process */
			if (!(CardType & CT_BLOCK))
				sc *= 512;                  /* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) { /* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF);
				xmit_spi(0xFE); /* Data block header */
				wc  = 512;      /* Set byte counter */
				res = RES_OK;
			}
		} else { /* Finalize sector write process */
			bc = wc + 2;
			while (bc--) {
				xmit_spi(0); /* Fill left bytes and CRC with zeros */
			}
			do {
				res = rcv_spi();
			} while (res == 0xFF);
			if ((res & 0x1F) == 0x05) { /* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; rcv_spi() != 0xFF && bc; bc--) /* Wait for ready */
					//_delay_us(100);
                    delay(1);
				if (bc)
					res = RES_OK;
			}
			DESELECT();
			rcv_spi();
		}
	}

	return res;
}
#endif

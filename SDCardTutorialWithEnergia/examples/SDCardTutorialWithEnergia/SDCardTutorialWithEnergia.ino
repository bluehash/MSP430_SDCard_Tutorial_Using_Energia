#include <pfatfs.h>
#include <pffconf.h>

/*----------------------------------------------------------------------*/
/* Petit FatFs sample project for generic uC  (C)ChaN, 2010             */
/*----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
   Ported to Energia by calinp
   Copy the file LOG.txt from this file's location to the root of the 
   SD-Card.

   12/21/2013
   Modified by bluehash(43oh) to log temperature data from the MSP430G2553
   internal temperature sensor and log it to a file on the SD-Card
   References:
   http://forum.43oh.com/topic/3209-energia-library-petit-fatfs-sd-card-library/
 */

 
#include "SPI.h" 
#include "pfatfs.h"

#define cs_pin      8             // chip select pin 
#define read_buffer 128             // size (in bytes) of read buffer 
#define LOG_DELAY   5000          // 5000ms -> 5sec

unsigned short int bw, br;//, i;
char buffer[read_buffer];
int rc;
DIR dir;				/* Directory object */
FILINFO fno;			/* File information object */

uint32_t ui32_ReadTemp = 0;
uint8_t StringLength = 0;
char buf[30];
uint32_t counter = 0;
uint32_t AccStringLength = 0;

void setup()
{
   pinMode(PUSH2, INPUT_PULLUP);
   Serial.begin(9600);                // initialize the serial terminal
   analogReference(INTERNAL1V5);
   analogRead(TEMPSENSOR);           // first reading usually wrong
   FatFs.begin(cs_pin);              // initialize FatFS library calls
   Serial.print("\n\n\n MSP430 Temperature Logger \n\r"); 
   Serial.println("Press S2 button to start...");
   while(digitalRead(PUSH2)==1){}
   delay(100);
   while(digitalRead(PUSH2)==0){}
}
         
/* Stop with dying message */         
void die ( int pff_err	)
{
   Serial.println();
   Serial.print("Failed with rc=");
   Serial.print(pff_err,DEC);
   for (;;) ;
}

void printDec(uint32_t ui) 
{
   Serial.print(ui/10, DEC);
   Serial.print(".");
   Serial.print(ui%10, DEC);
}

/*-----------------------------------------------------------------------*/
/* Program Main                                                          */
/*-----------------------------------------------------------------------*/
void loop()
{
   #if 0
   Serial.println();
   Serial.println("Press button to start...");
   while(digitalRead(PUSH2)==1){}
   delay(100);
   while(digitalRead(PUSH2)==0){}
   #endif

   ui32_ReadTemp = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16;

   #if DEBUG
   Serial.println();
   Serial.println("Opening log file to write temperature(LOG.txt).");
   delay(100);
   #endif
   
   rc = FatFs.open("LOG.TXT");
   if (rc) die(rc);

   delay(100);
   bw=0;
   ui32_ReadTemp = ((uint32_t)analogRead(TEMPSENSOR)*27069 - 18169625) *10 >> 16;
   sprintf( buf, "%lu Current temperature is %lu.%lu\r\n", counter, ui32_ReadTemp/10, ui32_ReadTemp%10 );
   counter++;
   StringLength =  strlen(buf);
   Serial.println(buf);        

   #if DEBUG
   Serial.print(StringLength, DEC);
   Serial.println();
   Serial.print(AccStringLength, DEC);
   #endif

   rc = FatFs.lseek(  AccStringLength );
   if (rc) die(rc);
   AccStringLength =  AccStringLength + 512;
   rc = FatFs.write(buf, StringLength,&bw);
   if (rc) die(rc);
   rc = FatFs.write(0, 0, &bw);  //Finalize write
   if (rc) die(rc);
   rc = FatFs.close();  //Close file
        if (rc) die(rc);

       
   #if READ         
   delay(100);
   Serial.println();
   Serial.println("Read Temp data from the log file (LOG.txt).");
   delay(100);
   rc = FatFs.open("LOG.TXT");
   if (rc) die(rc);

   delay(100);
   for (;;) {
   rc = FatFs.read(buffer, sizeof(buffer), &br);	/* Read a chunk of file */
   if (rc || !br) break;			/* Error or end of file */
   for (uint16_t i = 0; i < br; i++)		/* Type the data */
   Serial.print(buffer[i]);
   delay(100);
   }
   if (rc) die(rc);
   #endif

   // Log delay
   delay(LOG_DELAY);
}


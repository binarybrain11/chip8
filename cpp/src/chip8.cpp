/* 
 * Chip8 implementation based off of the documentation found here:
 * https://chip-8.github.io/links/
 *
 * Author: Zachary Gorman (binarybrain11 on github)
 */

#include "chip8.h"
#include <iostream>
#include <fstream>

/* 
 * Class constructor for opening stdin
 */
chip8::chip8( ){
   this->ROM = &std::cin;
   std::cout << this->ROM;
}

/* 
 * Class constructor for opening a file
 */
chip8::chip8( std::string ROMname ){
   std::filebuf ROMbuf;
   if ( ROMbuf.open( ROMname, std::ios::in) ){
      this->ROM = new std::istream( &ROMbuf );
   } else {
      this->ROM = &std::cin;
   }
}

/* 
 * Class deconstructor
 */
chip8::~chip8(){
   if ( this->ROM != &std::cin ) {
      delete this->ROM;
   }
}

void chip8::echoROM() {
   std::string str;
   while ( !this->ROM->eof() ) {
      *(this->ROM) >> str;
      std::cout << str;
   }
}

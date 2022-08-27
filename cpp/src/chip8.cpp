/* 
 * Chip8 implementation based off of the documentation found here:
 * https://chip-8.github.io/links/
 *
 * Author: Zachary Gorman (binarybrain11 on github)
 */

#include "chip8.h"
#include <iostream>     // I/0 library
#include <fstream>      // File I/O
#include <string>       // String helpers
#include <cstdlib>      // Random 
#include <thread>       // Sleep function
#include <chrono>       // Time for accurate sleep

/* UNIX stuff */
#include <termios.h>    // Reading input raw, no echo
#include <unistd.h>     // STDIN_FILENO

chip8::chip8() {
   
}

/* 
 * Class constructor for opening a file
 */
chip8::chip8( std::string ROMname ){
   this->RAM = new unsigned char[4096];
   /* Initializing the font */
   this->RAM[ 0] = 0b01000000; // .#..
   this->RAM[ 1] = 0b10100000; // #.#.
   this->RAM[ 2] = 0b10100000; // #.#.
   this->RAM[ 3] = 0b10100000; // #.#.
   this->RAM[ 4] = 0b01000000; // .#..

   this->RAM[ 5] = 0b01000000; // .#..
   this->RAM[ 6] = 0b11000000; // ##..
   this->RAM[ 7] = 0b01000000; // .#..
   this->RAM[ 8] = 0b01000000; // .#..
   this->RAM[ 9] = 0b11100000; // ###.

   this->RAM[10] = 0b11100000; // ###.
   this->RAM[11] = 0b00100000; // ..#.
   this->RAM[12] = 0b11100000; // ###.
   this->RAM[13] = 0b10000000; // #...
   this->RAM[14] = 0b11100000; // ###.

   this->RAM[15] = 0b11100000; // ###.
   this->RAM[16] = 0b00100000; // ..#.
   this->RAM[17] = 0b11100000; // ###.
   this->RAM[18] = 0b00100000; // ..#.
   this->RAM[19] = 0b11100000; // ###.

   this->RAM[20] = 0b10100000; // #.#.
   this->RAM[21] = 0b10100000; // #.#.
   this->RAM[22] = 0b11100000; // ###.
   this->RAM[23] = 0b00100000; // ..#.
   this->RAM[24] = 0b00100000; // ..#.

   this->RAM[25] = 0b11100000; // ###.
   this->RAM[26] = 0b10000000; // #...
   this->RAM[27] = 0b11100000; // ###.
   this->RAM[28] = 0b00100000; // ..#.
   this->RAM[29] = 0b11100000; // ###.

   this->RAM[30] = 0b11100000; // ###.
   this->RAM[31] = 0b10000000; // #...
   this->RAM[32] = 0b11100000; // ###.
   this->RAM[33] = 0b10100000; // #.#.
   this->RAM[34] = 0b11100000; // ###.

   this->RAM[35] = 0b11100000; // ###.
   this->RAM[36] = 0b00100000; // ..#.
   this->RAM[37] = 0b00100000; // ..#.
   this->RAM[38] = 0b00100000; // ..#.
   this->RAM[39] = 0b00100000; // ..#.

   this->RAM[40] = 0b11100000; // ###.
   this->RAM[41] = 0b10100000; // #.#.
   this->RAM[42] = 0b11100000; // ###.
   this->RAM[43] = 0b10100000; // #.#.
   this->RAM[44] = 0b11100000; // ###.

   this->RAM[45] = 0b11100000; // ###.
   this->RAM[46] = 0b10100000; // #.#.
   this->RAM[47] = 0b11100000; // ###.
   this->RAM[48] = 0b00100000; // ..#.
   this->RAM[49] = 0b00100000; // ..#.

   this->RAM[50] = 0b11100000; // ###.
   this->RAM[51] = 0b10100000; // #.#.
   this->RAM[52] = 0b11100000; // ###.
   this->RAM[53] = 0b10100000; // #.#.
   this->RAM[54] = 0b10100000; // #.#.

   this->RAM[55] = 0b11000000; // ##..
   this->RAM[56] = 0b10100000; // #.#.
   this->RAM[57] = 0b11100000; // ###.
   this->RAM[58] = 0b10100000; // #.#.
   this->RAM[59] = 0b11000000; // ##..

   this->RAM[60] = 0b11100000; // ###.
   this->RAM[61] = 0b10000000; // #...
   this->RAM[62] = 0b10000000; // #...
   this->RAM[63] = 0b10000000; // #...
   this->RAM[64] = 0b11100000; // ###.

   this->RAM[65] = 0b11000000; // ##..
   this->RAM[66] = 0b10100000; // #.#.
   this->RAM[67] = 0b10100000; // #.#.
   this->RAM[68] = 0b10100000; // #.#.
   this->RAM[69] = 0b11000000; // ##..

   this->RAM[70] = 0b11100000; // ###.
   this->RAM[71] = 0b10000000; // #...
   this->RAM[72] = 0b11000000; // ##..
   this->RAM[73] = 0b10000000; // #...
   this->RAM[74] = 0b11100000; // ###.

   this->RAM[75] = 0b11100000; // ###.
   this->RAM[76] = 0b10000000; // #...
   this->RAM[77] = 0b11000000; // ##..
   this->RAM[78] = 0b10000000; // #...
   this->RAM[79] = 0b10000000; // #...

   /* Creates a clean console for the display */ 
   std::cout << "\e[1J";

   /* RNG seed */ 
   std::chrono::time_point time = std::chrono::steady_clock::now();
   std::srand( 
         std::chrono::duration_cast<std::chrono::milliseconds>( 
            time.time_since_epoch() 
         )
      .count() );

   /* Open ROM */
   this->openROM( ROMname );

   /* Start the key listening thread */
   this->enableInput();
   this->running = true;
   this->keyListenThread = std::thread(&chip8::listenKey, this);
}

/* 
 * Class deconstructor
 */
chip8::~chip8(){
   /* TODO revisit */
   delete this->ROM;
   this->running = false;
   this->keyListenThread.join();
}

void chip8::openROM( std::string ROMname ){
   std::filebuf ROMbuf;
   if ( ROMbuf.open( ROMname, std::ios::in) ){
      this->ROM = new std::istream( &ROMbuf );
   } else {
      exit(1);
   }
   /* Load the ROM into RAM */
   *(this->ROM) >> (this->RAM + 0x200);
}

/* 
 * Translates the opcode to a function 
 */
void chip8::opcode( unsigned int code ) {
   if ( code == 0x00E0 ) 
   {
      /* Clear display */
      for (int row = 0; row < 32; row++) {
         for (int col = 0; col < DISP_WIDTH/8; col++){
            this->RAM[ row * (DISP_WIDTH/8) + col ] = 0;
         }
      }
      this->PC += 2;
   } 
   else if ( code == 0x00EE ) 
   {
      /* Return from subroutine */
      this->PC = this->pop();
   } 
   else if ( 0x0000 < code && code < 0x0FFF ) 
   {
      /* Calls machine code routine at address 0x0NNN
       * not necessary for most ROMs
       */
      this->opcode( this->RAM[ this->PC ] );
      this->PC += 2;
   } 
   else if ( ( 0xF000 & code ) == 0x1000 )
   { 
      /* Jump to address 0x0NNN similar to goto */ 
      this->PC = code & 0x0FFF;
   } 
   else if ( ( 0xF000 & code ) == 0x2000 )
   {
      /* Calls subroutine at 0x0NNN, similar to *(0x0NNN)() */
      this->push( this->PC );
      this->PC = code & 0x0FFF;
   }
   else if ( ( 0xF000 & code ) == 0x3000 )
   {
      /* Skips next instruction if VX ( 0x0X00 ) equals 0x00NN */
      char X = (code & 0x0F00) >> 8;
      if ( this->V[X] == (unsigned char)( code & 0x00FF ) ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0x4000 )
   {
      /* Skips next instruction if VX ( 0x0X00 ) not equals 0x00NN */
      char X = (code & 0x0F00) >> 8;
      if ( this->V[X] != (unsigned char)( code & 0x00FF ) ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF00F & code ) == 0x5000 )
   {
      /* Skips next instruction if VX ( 0x0X00 ) equals VY ( 0x00Y0 ) */
      char X = (code & 0x0F00) >> 8;
      char Y = (code & 0x00F0) >> 4;
      if ( this->V[X] != this->V[Y] ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0x6000 )
   {
      /* Sets VX ( 0x0X00 ) to 0x00NN */
      char X = code & 0x0F00 >> 8;
      this->V[X] = code & 0x00FF;
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0x7000 )
   {
      /* Adds 0x00NN to VX ( 0x0X00 ), carry flag unchanged */
      char X = code & 0x0F00 >> 8;
      this->V[X] += code & 0x00FF;
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0x8000 )
   {
      /* Register operators */ 
      if ( ( 0x000F & code ) == 0x0000 ) 
      {
         /* Sets VX ( 0x0X00 ) to VY ( 0x00Y0 ) */ 
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         this->V[X] = this->V[Y];
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0001 ) 
      {
         /* Sets VX ( 0x0X00 ) to VX OR VY ( 0x00Y0 ) */ 
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         this->V[X] = this->V[X] | this->V[Y];
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0002 ) 
      {
         /* Sets VX ( 0x0X00 ) to VX AND VY ( 0x00Y0 ) */ 
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         this->V[X] = this->V[X] & this->V[Y];
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0003 ) 
      {
         /* Sets VX ( 0x0X00 ) to VX XOR VY ( 0x00Y0 ) */ 
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         this->V[X] = this->V[X] ^ this->V[Y];
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0004 ) 
      {
         /* Sets VX ( 0x0X00 ) to VX plus VY ( 0x00Y0 ). 
          * VF set to 1 on carry, 0 otherwise. 
          */
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         int tmp = (unsigned int)this->V[X] + (unsigned int)this->V[Y];
         this->V[X] = tmp & 0xFF;
         if ( tmp & 0xFF00 ) {
            this->V[0xF] = 1;
         } else {
            this->V[0xF] = 0;
         }
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0005 ) 
      {
         /* Sets VX ( 0x0X00 ) to VX minus VY ( 0x00Y0 ). 
          * VF set to 0 on borrow, 1 otherwise. 
          */
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         int tmp = (unsigned int)this->V[X] - (unsigned int)this->V[Y];
         this->V[X] = tmp & 0xFF;
         if ( tmp < 0 ) {
            this->V[0xF] = 0;
         } else {
            this->V[0xF] = 1;
         }
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0006 ) 
      {
         /* Stores the least significant bit of VX in VF 
          * then shift VX to the right by 1. 
          */
         char X = (code & 0x0F00) >> 8;
         this->V[0xF] = this->V[X] & 0x1;
         this->V[X] = this->V[X] >> 1;
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0007 ) 
      {
         /* Sets VX ( 0x0X00 ) to VY ( 0x00Y0 ) minus VX. 
          * VF set to 0 on borrow, 1 otherwise. 
          */
         char X = (code & 0x0F00) >> 8;
         char Y = (code & 0x00F0) >> 4;
         int tmp = (unsigned int)this->V[Y] - (unsigned int)this->V[X];
         this->V[X] = tmp & 0xFF;
         if ( tmp < 0 ) {
            this->V[0xF] = 0;
         } else {
            this->V[0xF] = 1;
         }
         this->PC += 2;
      }
      else if ( ( 0x000F & code ) == 0x0008 ) 
      {
         /* Stores the most significant bit of VX in VF 
          * then shift VX to the left by 1. 
          */
         char X = (code & 0x0F00) >> 8;
         this->V[0xF] = this->V[X] >> 7;
         this->V[X] = this->V[X] << 1;
         this->PC += 2;
      }
   }
   else if ( ( 0xF00F & code ) == 0x9000 )
   {
      /* Skips the next instruction if VX ( 0x0X00 ) does not equal VY ( 0x00Y0 ) */
      char X = (code & 0x0F00) >> 8;
      char Y = (code & 0x00F0) >> 4;
      if ( this->V[X] != this->V[Y] ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0xA000 )
   {
      /* Sets I to address 0x0NNN */
      this->I = code & 0x0FFF;
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0xB000 )
   {
      /* Jumps to address 0x0NNN + V0 */
      this->PC = (unsigned int)this->V[0] + code & 0x0FFF;
   }
   else if ( ( 0xF000 & code ) == 0xC000 )
   {
      /* Sets VX ( 0x0X00 ) to ( 0x00NN ) AND a random number 
       */
      char X = (code & 0x0F00) >> 8;
      unsigned char rand = std::rand();
      this->V[X] = code & 0xFF & rand;
      this->PC += 2;
   }
   else if ( ( 0xF000 & code ) == 0xD000 )
   {
      /* Draw a sprite at coordinate (VX, VY) [ code: 0x0XY0 ] of 
       * width 8 pixels by 0x000N pixels. Each row is read as bit-coded 
       * starting from memory I. VF is set to 1 if pixels are flipped from
       * set to unset, 0 otherwise. 
       */
      char X = (code & 0x0F00) >> 8;
      char Y = (code & 0x00F0) >> 4;
      char height = (code & 0x000F);
      unsigned char VX = this->V[X];
      unsigned char VY = this->V[Y];
      /* Sprite could straddle byte boundaries */
      unsigned char col = VX / 8;
      unsigned char colBit = VX - ( col * 8 );
      unsigned char lowMask = 0xFF >> colBit;
      unsigned char highMask = 0xFF << (8 - colBit);
      this->V[0xF] = 0;
      for (int row=VY; row < ( VY + height ) && row < DISP_HEIGHT; row++) {
         /* Checks to see if a set pixel is flipped */ 
         if ( 
              ( ( col < DISP_WIDTH ) && 
                ( this->RAM[ row * (DISP_WIDTH/8) + col ] & lowMask & this->RAM[I + row] )
                  
              ) ||
              ( ( col + 1 < DISP_WIDTH ) &&
                ( this->RAM[ row * (DISP_WIDTH/8) + col + 1 ] & highMask & this->RAM[I + row] )
              ) 
            ) 
         {
            this->V[0xF] = 1;
         }
         /* XORs the necessary bits using a mask */
         if ( col < DISP_WIDTH ) {
            this->RAM[ row * (DISP_WIDTH/8) + col] ^= this->RAM[I + row] & lowMask;
         }
         /* Sets only the bits necessary using mask */
         if ( col + 1 < DISP_WIDTH ) {
            this->RAM[ row * (DISP_WIDTH/8) + col + 1] ^= this->RAM[I + row] & highMask;
         }
      }
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xE09E )
   {
      /* Skips the next instruction if the key stored in VX ( 0x0X00 ) 
       * is pressed
       */
      /* TODO fix so asynchronous */
      char X = (code & 0x0F00) >> 8;
      unsigned char VX = this->V[X];
      unsigned char c;
      std::cin >> c;
      /* Converts characters to number from numpad */
      c = this->numpad(c);
      if ( c == VX ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xE0A1 )
   {
      /* Skips the next instruction if the key stored in VX ( 0x0X00 ) 
       * not is pressed
       */
      /* TODO fix so asynchronous */
      char X = (code & 0x0F00) >> 8;
      unsigned char VX = this->V[X];
      unsigned char c;
      std::cin >> c;
      /* Converts characters to number from numpad */
      c = this->numpad(c);

      if ( c != VX ) {
         this->PC += 2;
      }
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF007 )
   {
      /* Sets VX ( 0x0X00 ) to the value of the delay timer 
       */
      char X = (code & 0x0F00) >> 8;
      this->V[X] = this->delay;
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF00A )
   {
      /* A key press is awaited then stored in VX ( 0x0X00 )
       */
      char X = (code & 0x0F00) >> 8;
      std::cin >> this->V[X];
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF015 )
   {
      /* Sets delay timer to VX ( 0x0X00 )
       */
      char X = (code & 0x0F00) >> 8;
      this->delay = this->V[X];
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF018 )
   {
      /* Sets sound timer to VX ( 0x0X00 )
       */
      char X = (code & 0x0F00) >> 8;
      this->sound = this->V[X];
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF01E )
   {
      /* Adds VX ( 0x0X00 ) to I. VF is unaffected. 
       */
      char X = (code & 0x0F00) >> 8;
      this->I = this->I + this->V[X];
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF029 )
   {
      /* Sets I to the location of the sprite for the character 
       * in VX. Characters 0-F (hex) are represented by a 4x5 font. 
       * Since sprites are drawn using XOR, characters can be drawn 
       * next to each other by leaving a blank space. The sprite 
       * is still 8X5 but only the leftmost 4X5 section has set pixels
       */
      char X = (code & 0x0F00) >> 8;
      this->I = this->V[X] * 5;
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF033 )
   {
      /* Stores the binary coded decimal (BCD) of VX ( 0x0X00 ) with 
       * the hundreds decimal digit in I, and the least significant 
       * in I+2
       *
       * This implements the Double Dabble algorithm
       */
      char X = (code & 0x0F00) >> 8;
      unsigned char VX = this-> V[X];

      // Initialize the result
      this->RAM[this->I + 0] = 0;
      this->RAM[this->I + 1] = 0;
      this->RAM[this->I + 2] = 0;
      
      while ( VX > 0 ) {
         // Mask ensures that only 4 bits are used per decimal place
         this->RAM[this->I + 0] = ( this->RAM[this->I + 0] << 1 ) & 0b1111;
         // This statement ensures that the MSB gets shifted properly
         if ( this->RAM[this->I + 1] & 0b1000 ) {
            this->RAM[this->I + 0] += 1;
         }

         this->RAM[this->I + 1] = ( this->RAM[ this->I + 1] << 1 ) & 0b1111;
         if ( this->RAM[ this->I + 2] & 0b1000 ) {
            this->RAM[ this->I + 1] += 1;
         }

         this->RAM[this->I + 2] = ( this->RAM[this->I + 2] << 1 ) & 0b1111;
         if ( VX & 0x8 ) {
            this->RAM[this->I + 2] += 1;
         }
         // This modifies a copy of VX, the register is still intact
         VX = VX << 1;
      }
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF055 )
   {
      /* Stores from V0 to VX ( 0x0X00 ) in memory starting at address 
       * I. I remains unmodified. 
       */
      for (int i=0; i<NUM_REGISTERS; i++) {
         this->RAM[this->I + i] = this->V[i];
      }
      this->PC += 2;
   }
   else if ( ( 0xF0FF & code ) == 0xF065 )
   {
      /* Fills from V0 to VX ( 0x0X00 ) with values starting from I,
       * leaving I unmodified
       */
      for (int i=0; i<NUM_REGISTERS; i++) {
         this->V[i] = this->RAM[this->I + i];
      }
      this->PC += 2;
   }
}

/* Stack methods */

void chip8::push( unsigned int addr ){
   if ( this->stack_ind + 1 < MAX_STACK ) {
      this->stack_ind++;
      this->stack[ this->stack_ind ];
   }
}

unsigned int chip8::pop() {
   if ( this->stack_ind > 0 ) {
      return this->stack[ this->stack_ind-- ];
   }
   return -1;
}

/* Input control */
int chip8::enableInput() {
   tcgetattr(STDIN_FILENO, &(this->tOptsOld));
   this->tOptsNew = this->tOptsOld;
   this->tOptsNew.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &(this->tOptsNew));
   return 0;
}

int chip8::disableInput() {
   tcsetattr(STDIN_FILENO, TCSANOW, &(this->tOptsOld));
   return 0;
}

unsigned char chip8::numpad( unsigned char c ) {
      /************************************************
       *
       * 16 key nnumpad:
       *
       * 7 8 9 0             1 2 3 A 
       * u i o p   maps to   4 5 6 B 
       * j k l ;  ---------> 7 8 9 C 
       * m , . /             E 0 F D 
       *
       ************************************************/
      /* Mapping table */
      switch(c) {
         case '7': return 1; break;
         case '8': return 2; break;
         case '9': return 3; break;
         case '0': return 0xA; break;
         case 'u': return 4; break;
         case 'i': return 5; break;
         case 'o': return 6; break;
         case 'p': return 0xB; break;
         case 'j': return 7; break;
         case 'k': return 8; break;
         case 'l': return 9; break;
         case ';': return 0xC; break;
         case 'm': return 0xE; break;
         case ',': return 0; break;
         case '.': return 0xF; break;
         case '/': return 0xD; break;
         default: return '*';
      }

}

/* Function reads the key set by the asynch input then clears it for the 
 * next key
 */
unsigned char chip8::readKey() {
   unsigned char key = this->key;
   this->key = '\0';
   return key;
}

void chip8::listenKey() {
   while ( running ) {
      std::cin >> this->key;
   }
}

void chip8::enableDisplay() {
   this->dispEnable = true;
}

void chip8::disableDisplay() {
   this->dispEnable = false;
}

int chip8::run() {
   /* TODO implement the clock cycle */
   /* Setup execution */
   this->PC = 0x200;
   while ( true ) {
      /* Start timer for this frame */
      std::chrono::time_point start = std::chrono::steady_clock::now();
      /* Draw the display */
      if ( this->dispEnable ) {
         std::cout << "\e[1J";
         for ( int row = 0; row < DISP_HEIGHT; row++ ) {
            for ( int col = 0; col < DISP_WIDTH/8; col++ ) {
               for ( int bit = 1; bit & 0b11111111; bit << 1 ) {
                  if ( bit & this->RAM[ row * DISP_WIDTH + col ] ) {
                     /* on pixel */
                     std::cout << "\e[47m  \e[0m";
                  } else {
                     /* off pixel */
                     std::cout << "\e[40m  \e[0m";
                  }
               }
               std::cout << "\n";
            }
         }
      }
      /* Read the current opcode. PC is incremented/set in this function */
      this->opcode( this->RAM[ this->PC ] );
      /* Wait for the next clock cycle */
      std::chrono::time_point end = std::chrono::steady_clock::now();
      std::this_thread::sleep_for( std::chrono::milliseconds( 
               ( 1000 / FRAME_RATE ) - 
               std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count()
               ) );
   }
}

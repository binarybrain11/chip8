#ifndef CHIP8 
#define CHIP8 

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

/* UNIX stuff */
#include <termios.h>

#define MAX_STACK 16
#define DISP_HEIGHT 32
#define DISP_WIDTH 64
#define NUM_REGISTERS 16
#define FRAME_RATE 60

class chip8 {
   private: 
      std::istream* ROM;
      /* The first 512 bytes are traditionally reserved for 
       * the chip8 interpreter, that is 0x000 - 0x200, though
       * in this implementation that's not necessary so we 
       * can use it for some static data like fonts. 
       * 0xF00 - 0xFFF are typically reserved for the display
       * Each byte covers 8 pixels
       * Row major 
       * 0xEA0 - 0xEFF are typically reserved for the call stack 
       * and variables. 
       */
      unsigned char* RAM;
      void openROM( std::string );
      /* Registers, labeled V0 - VF. Use hex for indexing. 
       * VF doubles as a flag register so should be avoided. 
       */
      unsigned char V[NUM_REGISTERS]; 
      /* Address register */
      unsigned int I;
       /* Stack could be its own class 
       */
      unsigned int stack[ MAX_STACK ];
      unsigned int stack_ind;
      void push( unsigned int );
      unsigned int pop();
      /* Timers */
      unsigned char delay;
      unsigned char sound;

      /* Program Counter */
      unsigned int PC;

      /* Input control */
      struct termios tOptsNew;
      struct termios tOptsOld;
      unsigned char key;
      unsigned char readKey();
      void listenKey();
      std::thread keyListenThread;
      bool running;

      unsigned char numpad( unsigned char );

      bool dispEnable;

      void opcode( unsigned int );

   public:
      chip8();
      chip8(std::string);
      ~chip8();

      int run();
      /* Chip8 input won't be echoed to the console. On by default. */
      int enableInput();
      /* Input will be echoed to the console. This may mess up the display */
      int disableInput();
      /* Display will be printed. On by default. */
      void enableDisplay();
      /* Display will be hidden so chip8 may be run silently */
      void disableDisplay();
};

#endif

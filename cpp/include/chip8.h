#ifndef CHIP8 
#define CHIP8 

#include <iostream>
#include <fstream>
#include <string>

class chip8 {
   private: 
      std::istream* ROM;
   public:
      chip8();
      chip8(std::string);
      ~chip8();
      void echoROM();
      int open_ROM( std::string );
      int run();
};

#endif

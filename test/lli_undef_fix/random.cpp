#include <APInt.h>
#include <iostream>
#include <ostream>

int main ( int argc, char* argv[] )
{{
  std::cout << "generating random value... \n";
  APInt george= llvm::APInt::getRandom( 32, false );
  std::cout << "val=\"" << george.toString( 10, false ) << "\"\n";
}}

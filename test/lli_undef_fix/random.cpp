#include "llvm/ADT/APInt.h"
#include <iostream>
#include <ostream>
#include <cstdlib>

int main ( int argc, char* argv[] )
{{
  int bitsizes[]= { 32, 16, 48, 64, 72, -1 };
  int ii;
  llvm::APInt george;

  srand(45);

  for ( ii= 0; bitsizes[ii] != -1; ii++ )  {
    std::cout << "generating random value of " << bitsizes[ii] << "bits... \n";
    george= llvm::APInt::getRandom( bitsizes[ii], false );
    std::cout << "   val=\"" << george.toString( 10, false ) << "\"\n";
  }
}}

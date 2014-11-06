//===-- APInt.cpp - Implement APInt class ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a class to represent arbitrary precision integer
// constant values and provide a variety of arithmetic operations on them.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APIntPoison.h"

#if 0
  #include "llvm/ADT/FoldingSet.h"
  #include "llvm/ADT/Hashing.h"
  #include "llvm/ADT/SmallString.h"
  #include "llvm/ADT/StringRef.h"
  #include "llvm/Support/Debug.h"
  #include "llvm/Support/ErrorHandling.h"
  #include "llvm/Support/MathExtras.h"
  #include "llvm/Support/raw_ostream.h"
  #include <cmath>
  #include <cstdlib>
  #include <cstring>
  #include <limits>
#endif

using namespace llvm;

// ############################################################################
namespace llvm {

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_uadd()
 * ----------------------------------------------------------------------------
 * Description: if unsigned wraparound is forbidden, mark the
 *	destination as poisoned if the given add operands would create a
 *	poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the sum to check
 *   lhs, rhs: the two operands to check
 *   noWrap: true if a "no wrap" flag was present on the LLVM instruction.
 *	If this is false, no poison is possible, so no checking is performed.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_uadd( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap )
{{
  if ( ! noWrap )  { return; }
  if ( dest.ult(lhs) || dest.ult(rhs) )  {
    // an unallowed wrap happened
    dest.orPoisoned(true);
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_sadd()
 * ----------------------------------------------------------------------------
 * Description: if signed wraparound is forbidden, mark the
 *	destination as poisoned if the given add operands would create a
 *	poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the sum to check
 *   lhs, rhs: the two operands to check
 *   noWrap: true if a "no wrap" flag was present on the LLVM instruction.
 *	If this is false, no poison is possible, so no checking is performed.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_sadd( APInt& dest, APInt& lhs, APInt& rhs, bool noWrap )
{{
  if ( ! noWrap )  { return; }
  if ( rhs.slt(0) ? dest.sgt(lhs) : dest.slt(lhs) )  {
    // an unallowed wrap happened
    dest.orPoisoned(true);
  }
  return;
}}




} // end namespace llvm
// ############################################################################

// template is 16 lines long
/*** --------------------------------------------------------------------------
   * function name()
   * --------------------------------------------------------------------------
   * Description: 
   *
   * Method: 
   *
   * Reentrancy: 
   *
   * Inputs: 
   *    
   * Outputs: 
   *
   * Return Value: 
   *
   */
//void name()



#define DEBUG_TYPE "apint"


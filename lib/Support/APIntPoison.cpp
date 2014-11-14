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

// ############################################################################
namespace APIntPoison {

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_add()
 * ----------------------------------------------------------------------------
 * Description: if signed and/or unsigned wraparound is forbidden, mark the
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
 *   nsw, nuw: true if the "no signed wrap" (nsw) or "no unsigned wrap" (nuw) 
 *	flag was present on the LLVM instruction.
 *	If one of these is false, no poison can be generated, so no
 *	checking is performed for that kind of wrap.  Of course, if
 *	the result was already poisoned (probably because one of the
 *	operands was poisoned), that poison remains.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
  void poisonIfNeeded_add( APInt& dest, APInt& lhs, APInt& rhs, 
			   bool nsw, bool nuw )
{{
  if ( nsw )  { 
    if ( rhs.slt(0) ? dest.sgt(lhs) : dest.slt(lhs) )  {
      // an unallowed signed wrap happened
      dest.orPoisoned(true);
    }
  }
  if ( nuw )  { 
    if ( dest.ult(lhs) || dest.ult(rhs) )  {
      // an unallowed unsigned wrap happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_sub()
 * ----------------------------------------------------------------------------
 * Description: if signed and/or unsigned wraparound is forbidden, mark the
 *	destination as poisoned if the given sub operands would create a
 *	poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the sum to check
 *   lhs, rhs: the two operands to check
 *   nsw, nuw: true if the "no signed wrap" (nsw) or "no unsigned wrap" (nuw) 
 *	flag was present on the LLVM instruction.
 *	If one of these is false, no poison can be generated, so no
 *	checking is performed for that kind of wrap.  Of course, if
 *	the result was already poisoned (probably because one of the
 *	operands was poisoned), that poison remains.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
  void poisonIfNeeded_sub( APInt& dest, APInt& lhs, APInt& rhs, 
			   bool nsw, bool nuw )
{{
  if ( nsw )  { 
    if ( false /* TODO: fill this in */  )  {
      // an unallowed signed wrap happened
      dest.orPoisoned(true);
    }
  }
  if ( nuw )  { 
    if ( false /* TODO: fill this in */  )  {
      // an unallowed unsigned wrap happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

} // end namespace APIntPoison
// ############################################################################


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


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

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DebugLoc.h"

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
 *   dest: the difference to check
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
    if ( rhs.sgt(0) ? dest.sgt(lhs) : dest.slt(lhs) )  {
      // an unallowed signed wrap happened
      dest.orPoisoned(true);
    }
  }
  if ( nuw )  { 
    if ( lhs.ult(rhs) )  {
      // an unallowed unsigned wrap happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_mul()
 * ----------------------------------------------------------------------------
 * Description: if signed and/or unsigned wraparound is forbidden, mark the
 *	destination as poisoned if the given mul operands would create a
 *	poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the product to check
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
void poisonIfNeeded_mul( APInt& dest, APInt& lhs, APInt& rhs, 
			 bool nsw, bool nuw )
{{
  if ( nsw )  { 
    // algorithm from:
    // https://www.securecoding.cert.org/confluence/display/seccode/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

    APInt int_max= APInt::getSignedMaxValue( dest.getBitWidth() );
    APInt int_min= APInt::getSignedMinValue( dest.getBitWidth() );
    if ( lhs.sgt(0) ) {  /* lhs is positive */
      if (rhs.sgt(0) > 0) {  /* lhs and rhs are positive */
	if (lhs.sgt( (int_max.sdiv(rhs) )) )  {
	  // an unallowed signed wrap happened
	  dest.orPoisoned(true);
	}
      } else { /* lhs positive, rhs nonpositive */
	if (rhs.slt( (int_min.sdiv(lhs) )) )  {
	  // an unallowed signed wrap happened
	  dest.orPoisoned(true);
	}
      } /* lhs positive, rhs nonpositive */
    } else { /* lhs is nonpositive */
      if (rhs.sgt(0) )  { /* lhs is nonpositive, rhs is positive */
	if (lhs.slt( (int_min.sdiv(rhs) )) )  {
	  // an unallowed signed wrap happened
	  dest.orPoisoned(true);
	}
      } else { /* lhs and rhs are nonpositive */
	if ( (lhs != 0) && (rhs.slt( (int_max.sdiv(lhs) )) ) )  {
	  // an unallowed signed wrap happened
	  dest.orPoisoned(true);
	}
      } /* End if lhs and rhs are nonpositive */
    } /* End if lhs is nonpositive */
  }
  if ( nuw )  { 
    // algorithm from: 
    // http://stackoverflow.com/questions/199333/best-way-to-detect-integer-overflow-in-c-c
    unsigned lhs_digits= lhs.getBitWidth()- lhs.countLeadingZeros();
    unsigned rhs_digits= rhs.getBitWidth()- rhs.countLeadingZeros();
    if ( (lhs_digits + rhs_digits) > dest.getBitWidth() )  {
      // an unallowed unsigned wrap happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_div()
 * ----------------------------------------------------------------------------

 * Description: if a remainder is forbidden, mark the destination as poisoned
 *	if the given div operands would create a poison value.
 *	This works for both the sdiv and udiv instructions.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the quotient to check
 *   lhs, rhs: the two operands to check
 *   exact: true if the "exact" flag was present on the LLVM instruction.
 *	If it is false, no poison can be generated, so no
 *	checking is performed for remainders.  Of course, if
 *	the result was already poisoned (probably because one of the
 *	operands was poisoned), that poison remains.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_div( APInt& dest, APInt& lhs, APInt& rhs, 
			 bool exact )
{{
  if ( exact )  { 
    if ( (rhs * dest) != lhs )  {
      // an unallowed remainder happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_shl()
 * ----------------------------------------------------------------------------
 * Description: if signed and/or unsigned overshift is forbidden, mark the
 *	destination as poisoned if the given shl operands would create a
 *	poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the result to check
 *   lhs: the APInt that got shifted
 *   shiftAmt: the number of places to shift left
 *   nsw, nuw: true if the "no signed wrap" (nsw) or "no unsigned wrap" (nuw) 
 *	flag was present on the LLVM instruction.  "Wrap" for shifting 
 *	purposes means "overshift". If one of these is false, no poison can be
 *	generated, so no checking is performed for that kind of wrap.  Of
 *	course, if the result was already poisoned (probably because one of
 *	the operands was poisoned), that poison remains.
 *    
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_shl( APInt& dest, APInt& lhs, unsigned shiftAmt,
			 bool nsw, bool nuw )
{{
  // if nothing was shifted, no poison can be generated.
  if ( shiftAmt == 0 )  { return; }

  if ( nsw )  { 
    if ( dest.isNegative() )  {
      // did any 0 bits get shifted out?
      if ( ! lhs.getHiBits(shiftAmt).trunc(shiftAmt).isAllOnesValue()  )  {
      	// an unallowed signed wrap happened
      	dest.orPoisoned(true);
      }
    } else {
      // did any 1 bits get shifted out?
      /* CAS TODO3: this trunc(~) call here may be unnecessary, but it
	 is here for now to guarantee accuracy.
       */
      if ( lhs.getHiBits(shiftAmt).trunc(shiftAmt) != 0 )  {
      	// an unallowed signed wrap happened
      	dest.orPoisoned(true);
      }
    }
  }
  if ( nuw )  { 
    // did any 1 bits get shifted out?
    /* CAS TODO3: this trunc(~) call here may be unnecessary, but it
       is here for now to guarantee accuracy.
     */
    if ( lhs.getHiBits(shiftAmt).trunc(shiftAmt) != 0 )  {
      // an unallowed unsigned wrap happened
      dest.orPoisoned(true);
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_lshr()
 * ----------------------------------------------------------------------------
 * Description: if an exact shift is required, mark the destination as
 *	poisoned if the given lshr operands would create a poison value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the result to check
 *   lhs: the APInt that got shifted
 *   shiftAmt: the number of places to shift left
 *   exact: true if the "exact" flag was present on the LLVM instruction.  
 *	If this is false, no poison can be generated, so no checking is
 *	performed.  Of course, if the result was already poisoned (probably
 *	because one of the operands was poisoned), that poison remains.
 *     
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_lshr( APInt& dest, APInt& lhs, unsigned shiftAmt,
			  bool exact )
{{
  if ( exact )  { 
    // did any 1 bits get shifted out?
    /* CAS TODO3: this trunc(~) call here may be unnecessary, but it
       is here for now to guarantee accuracy.
     */
    if ( shiftAmt != 0 )  {
      if ( lhs.getLoBits(shiftAmt).trunc(shiftAmt) != 0 )  {
	// an unallowed unsigned wrap happened
	dest.orPoisoned(true);
      }
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
 * function poisonIfNeeded_ashr()
 * ----------------------------------------------------------------------------
 * Description: if an inbounds result is required, mark the destination as
 *	poisoned if the given getelementptr operands would create a poison 
 *	value.
 *
 * Method: 
 *
 * Reentrancy: 
 *
 * Inputs: 
 *   dest: the result to check
 *   lhs: the APInt that got shifted
 *   shiftAmt: the number of places to shift left
 *   exact: true if the "exact" flag was present on the LLVM instruction.  
 *	If this is false, no poison can be generated, so no checking is
 *	performed.  Of course, if the result was already poisoned (probably
 *	because one of the operands was poisoned), that poison remains.
 *     
 * Outputs: 
 *   dest: write the poison result here
 *
 * Return Value: none
 *
 */
void poisonIfNeeded_ashr( APInt& dest, APInt& lhs, unsigned shiftAmt,
			  bool exact )
{{
  if ( exact )  { 
    // did any 1 bits get shifted out?
    /* CAS TODO3: this trunc(~) call here may be unnecessary, but it
       is here for now to guarantee accuracy.
     */
    if ( shiftAmt != 0 )  {
      if ( lhs.getLoBits(shiftAmt).trunc(shiftAmt) != 0 )  {
	// an unallowed unsigned wrap happened
	dest.orPoisoned(true);
      }
    }
  }
  return;
}}

/*** --------------------------------------------------------------------------
   * function poisonIfNeeded_getelementptr()
   * --------------------------------------------------------------------------
   * Description: CAS TODO3: implement this sometime.
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
//void poisonIfNeeded_getelementptr()

/*** --------------------------------------------------------------------------
   * function printIfPoison()
   * --------------------------------------------------------------------------
   * Description: prints to stdout the source file and line number if the given
   *	APInt is poisoned.  Only prints if option opt_print_new_poison is set.
   *
   * Method: 
   *
   * Reentrancy: 
   *
   * Inputs: 
   *   In: the Instruction where the poison was generated
   *   val: the APInt to check
   *    
   * Outputs: none
   *
   * Return Value: void
   *
   */
void printIfPoison( Instruction& In, APInt& val )
{{
  if ( ! llvm::lli_undef_fix::opt_print_new_poison ) { return; }
  static raw_fd_ostream stream( 2, false, true );

  if ( ! val.getPoisoned() )  { return; }

  DebugLoc dl= In.getDebugLoc();
  dl.print( stream );
   
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


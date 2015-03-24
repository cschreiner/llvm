//===-- APInt.cpp - Implement APInt class ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements functions to help class APInt track poison
// propogation through its arithmetic operations.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DebugLoc.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APIntPoison.h"

#include "llvm/Support/LUF_opts.h"

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

// ----------------------------------------------------------------------------
///  \fn poisonIfNeeded_bitAnd()
// ----------------------------------------------------------------------------
/*** \brief determines if the result of a "bitwise and" operation is poisoned
   *
   * \b Detailed_Description: 
   *
   * \b Method: If option opt_antidote_and_or is set, uses
   *	short-circuit poison propogation (i.e. 0 and poison= unpoisoned
   *	0).  Otherwise propogates poison if either operand is
   *	poisoned.
   *
   * \b Reentrancy: 
   *
   * \param lhs, rhs (input) the left and right operands
   *    
   * \param dest (output) the result of the AND operation
   *
   * \return void
   *
   */
void poisonIfNeeded_bitAnd( APInt& dest, const APInt& lhs, const APInt& rhs )
{{
  if ( llvm::lli_undef_fix::opt_antidote_and_or )  {
    if ( lhs.getPoisoned() && rhs.getPoisoned() )  {
       dest.setPoisoned( true );
       return;
    }
    if ( (!lhs.getPoisoned()) && (!rhs.getPoisoned()) )  {
       dest.setPoisoned( false );
       return;
    }
    // check for lhs is poisoned, rhs is unpoisoned and zero.
    if ( lhs.getPoisoned() && (!rhs.getPoisoned()) && (!rhs) )  {
       // a corrupted value in lhs does not affect the result.
       dest.setPoisoned( false );
       return;
    }
    // check for lhs is unpoisoned and zero, rhs is poisoned.
    if ( (!lhs.getPoisoned()) && (!lhs) && rhs.getPoisoned() )  {
       // a corrupted value in rhs does not affect the result.
       dest.setPoisoned( false );
       return;
    }
  } 

  // use the classical definition of poison
  dest.setPoisoned( lhs.getPoisoned() || rhs.getPoisoned() );
  return;
}}


// ----------------------------------------------------------------------------
///  \fn poisonIfNeeded_bitOr()
// ----------------------------------------------------------------------------
/*** \brief determines if the result of a "bitwise or" operation is poisoned
   *
   * \b Detailed_Description: 
   *
   * \b Method: If option opt_antidote_and_or is set, uses
   *	short-circuit poison propogation (i.e. 1 or poison= unpoisoned
   *	1).  Otherwise propogates poison if either operand is
   *	poisoned.
   *
   * \b Reentrancy: 
   *
   * \param lhs, rhs (input) the left and right operands
   *    
   * \param dest (output) the result of the AND operation
   *
   * \return void
   *
   */
void poisonIfNeeded_bitOr( APInt& dest, const APInt& lhs, const APInt& rhs )
{{
  if ( llvm::lli_undef_fix::opt_antidote_and_or )  {
    if ( lhs.getPoisoned() && rhs.getPoisoned() )  {
       dest.setPoisoned( true );
       return;
    }
    if ( (!lhs.getPoisoned()) && (!rhs.getPoisoned()) )  {
       dest.setPoisoned( false );
       return;
    }
    // check for lhs is poisoned, rhs is unpoisoned and one.
    if ( lhs.getPoisoned() && (!rhs.getPoisoned()) && rhs.isAllOnesValue() )  {
       // a corrupted value in lhs does not affect the result.
       dest.setPoisoned( false );
       return;
    }
    // check for lhs is unpoisoned and zero, rhs is poisoned.
    if ( (!lhs.getPoisoned()) && lhs.isAllOnesValue() && rhs.getPoisoned() )  {
       // a corrupted value in rhs does not affect the result.
       dest.setPoisoned( false );
       return;
    }
  } 

  // use the classical definition of poison
  dest.setPoisoned( lhs.getPoisoned() || rhs.getPoisoned() );
  return;
}}


} // end namespace APIntPoison
// ############################################################################


} // end namespace llvm
// ############################################################################

// template is 21 lines long
// ----------------------------------------------------------------------------
///  \fn name()
// ----------------------------------------------------------------------------
/*** \brief 
   *
   * \b Detailed_Description: 
   *
   * \b Method: 
   *
   * \b Reentrancy: 
   *
   * \param xx (input) 
   *    
   * \param yy (output) 
   *
   * \return 
   *
   */
//void name()
//{{
//}}
	
// CAS TODO2: figure out what this DEBUG_TYPE is for -- 2015mar14
#define DEBUG_TYPE "apint"


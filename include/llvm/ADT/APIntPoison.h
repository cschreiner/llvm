//===-- llvm/ADT/APIntPoison.h - Arbitrary Precision Integer Poison issues --*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a namespace of functions that detect when an
///   APInt has become poisoned. 
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_APINTPOISON_H
#define LLVM_ADT_APINTPOISON_H

#include "llvm/ADT/APInt.h"
#include <cassert>
#include <climits>
#include <cstring>
#include <string>

namespace llvm {

//===----------------------------------------------------------------------===//
//                              APIntPoison Namespace
namespace APIntPoison {

/* There should be exactly one function here for each LLVM instruction
   that can generate a poison value.
 */
// TODO: add const designations here as appropriate.

// TODO: add overloads here as appropriate, for example, one operand
// may be an integer (signed or not).

void poisonIfNeeded_add( APInt& dest, APInt& lhs, APInt& rhs, 
			 bool nsw, bool nuw );
/* Looks like we need separate checks for ++ vs a full add.
   Add such a function if needed.
*/
void poisonIfNeeded_sub( APInt& dest, APInt& lhs, APInt& rhs, 
			   bool nsw, bool nuw );
void poisonIfNeeded_mul( APInt& dest, APInt& lhs, APInt& rhs, 
			   bool nsw, bool nuw );

void poisonIfNeeded_udiv( APInt& dest, APInt& lhs, APInt& rhs, bool exact );
void poisonIfNeeded_sdiv( APInt& dest, APInt& lhs, APInt& rhs, bool exact );

void poisonIfNeeded_shl( APInt& dest, APInt& lhs, APInt& rhs, 
			 bool nsw, bool nuw );

void poisonIfNeeded_lshr( APInt& dest, APInt& lhs, APInt& rhs, bool exact );
void poisonIfNeeded_ashr( APInt& dest, APInt& lhs, APInt& rhs, bool exact );

void poisonIfNeeded_getelementptr( 
    APInt& dest, APInt& lhs, APInt& rhs, bool inbounds );

} // End of APIntPoison namespace

} // End of llvm namespace

#endif

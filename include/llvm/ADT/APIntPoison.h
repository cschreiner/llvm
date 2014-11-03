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

poisonIfNeeded_uadd( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_sadd( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_usub( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_ssub( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_umul( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_smul( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_udiv( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_sdiv( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );

poisonIfNeeded_ushl( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_sshl( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_ulshr( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_slshr( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_uashr( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );
poisonIfNeeded_sashr( APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );

poisonIfNeeded_getelementptr( 
    APInt& dest, APInt& LHS, APInt& RHS, boolean noWrap );

} // End of APIntPoison namespace

} // End of llvm namespace

#endif

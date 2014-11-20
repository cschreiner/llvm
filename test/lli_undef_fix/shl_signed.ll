; RUN: 

; TODO: find some way to add lit code here to run this, and compare
;	its printed output against an expected value... which may be
;	affected by the intent that a poisoned value should crash this 
;	program when output.

; Add two signed numbers together with wrapping, once with a restriction that
;	generates a poison value, and once without that restriction.

; Declare the printf() control strings as global constants.
@unpoison_st = private unnamed_addr constant [21 x i16] c"unpoisoned: '0x%x' \0A\00"
@poison_st = private unnamed_addr constant [19 x i16] c"poisoned: '0x%x' \0A\00"

; External declaration of the puts function
declare i32 @printf(i16* nocapture readonly, ...)

; Definition of main function
define i32 @main() {   ; i32()*
  ; Convert [19 x i16]* to i16  *...
  %unpoison_st_i16 = getelementptr [21 x i16]* @unpoison_st, i64 0, i64 0
  %poison_st_i16 = getelementptr [19 x i16]* @poison_st, i64 0, i64 0

  %nowrap1= shl i16 ff11, 7; was 178, 7
  %nowrap2= shl nsw i16 ff11, 7 ; was 178, 7

  ; Call puts function to write out the string to stdout.
  call i32 (i16*, ...)* @printf(i16* %unpoison_st_i16, i16 %nowrap1 )
  call i32 (i16*, ...)* @printf(i16* %unpoison_st_i16, i16 %nowrap2 )

  %unpoisoned_1= shl i16 fe02, 7 ; was 122, 7
  %poisoned_1= shl nsw i16 fe02, 7 ; was 122, 7

  ; Call puts function to write out the string to stdout.
  call i32 (i16*, ...)* @printf(i16* %unpoison_st_i16, i16 %unpoisoned_1 )
  call i32 (i16*, ...)* @printf(i16* %poison_st_i16, i16 %poisoned_1 )

  ; clean up and return
  ret i32 0
}


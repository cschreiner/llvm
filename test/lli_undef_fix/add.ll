; RUN: llvm-as %s -o - | llvm-dis > %t1.ll
; RUN: llvm-as %t1.ll -o - | llvm-dis > %t2.ll
; RUN: diff %t1.ll %t2.ll

; Declare the string constant as a global constant.
@poison_st = private unnamed_addr constant [19 x i8] c"poisoned: '0x%x' \0A\00"

; External declaration of the puts function
declare i32 @printf(i8* nocapture readonly, ...)

; Definition of main function
define i32 @main() {   ; i32()*
  ; Convert [19 x i8]* to i8  *...
  %cast210 = getelementptr [19 x i8]* @poison_st, i64 0, i64 0

  ; Call puts function to write out the string to stdout.
  call i32 (i8*, ...)* @printf(i8* %cast210, i32 42 )
  ret i32 0
}

; Named metadata
!0 = metadata !{i32 42, null, metadata !"string"}
!foo = !{!0}

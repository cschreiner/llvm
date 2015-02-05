; DONT_RUN: llvm-as %s -o - | llvm-dis > %t1.ll
; DONT_RUN: llvm-as %t1.ll -o - | llvm-dis > %t2.ll
; DONT_RUN: diff %t1.ll %t2.ll

; Declare the string constant as a global constant.
@.str = private unnamed_addr constant [16 x i8] c"hello %x world\0A\00"

; External declaration of the puts function
;declare i32 @puts(i8* nocapture) nounwind
declare i32 @printf(i8* nocapture readonly, ...)

; Definition of main function
define i32 @main() {   ; i32()*
  ; Convert [16 x i8]* to i8  *...
  %cast210 = getelementptr [16 x i8]* @.str, i64 0, i64 0

  ; Call puts function to write out the string to stdout.
  call i32 (i8*, ...)* @printf(i8* %cast210, i32 42 )
  ret i32 0
}

; Named metadata
;; metadata is commented out because lli barfs on it:
;; TODO2: resolve this.  This happened with one of the changes made
;;   between 25 Jan and 3 Feb.
;;!0 = metadata !{i32 42, null, metadata !"string"}
;;!foo = !{!0}

.3ds
.thumb

.include "../utils/adr.s"

.open "code.bin", "build/patched_code.bin", 0x100000

.include "src/account_url.s"

; Prevent unused accounts from being deleted on act start
.org 0x114f8c
  mov r0, #1
  bx lr

.close

      .ORIG x3000
      LEA R4, BEGIN
      LDW R5, R4, #0
      LDW R5, R5, #0
      AND R4, R4, #0
      ADD R1, R1, #10
      ADD R1, R1, #9
LOOP  LDB R2, R5, #0
      ADD R5, R5, #1
      ADD R4, R4, R2
      ADD R1, R1, #-1
      BRZP LOOP
      LEA R1, STORE
      LDW R5, R1, #0
      STW R4, R5, #0
      JMP R4
      HALT

BEGIN .FILL xC001
STORE .FILL xC014
.END

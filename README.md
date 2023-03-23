# SIC-XE
模擬SIC指令運作

SIC machine architecture:

Memory記憶體:
- 8-bit bytes
- Word:24 bits(連續3個位元組形成一個字組)

Registers暫存器:（五種 每個24bits）
- A(Accumulator) 累加器，用於算術運算
- X(Index register) 索引暫存器，用於定址
- L(Linkage register) 連結暫存器，JSUB指令會將返回的位址儲存在此
- PC(Program counter) 程式計數器，儲存下一個要取出來執行指令的位址
- SW(Status word) 狀態字組，儲存不同資訊

指令集:
- 載入和儲存暫存器(LDA、LDX、STA、STX)
- 算術運算(ADD、SUB、MUL、DIV)
- 控制(條件跳耀if)
- 輸入輸出(I/O)

SIC虛指令:
- START ：指定程式名稱和起始位置
- END : 結束處並指定程式中第一個執行指令
- BYTE : 定義字元或十六進位常數 ex:
- C 'EOF' = 3 (c計算引號內有多少字元)
- X 'F1' = 1 (x：計算每2個hex為1 byte, 此例長度為1)
- WORD : 定義一個字組的整數常數 
- RESB : 保留所示數量位元組 
- RESW : 保留所示數量字組

; loader_hybrid_com.asm
;
; This is a DOS command-line loader for RTEMS executables running on
; the Technologic Systems TS-1325 Embedded PC.
;
; It loads a DOS file given on the command line to the address `KernelBase',
; and then transfers control there. It uses DOS file I/O commands to read from
; the A: flash disk, and direct memory access to read from the C: ramdisk.
;
; Copying uses protected flat mode, so kernelbase could be above 1MB.
; It does not initialize protected mode before transferring control
; to the RTEMS executable image.
;
; Compile with: nasm -o loader.com loader_hybrid_com.asm
;
; Tony Ambardar (c) 1999
; E.C.E. Department
; University of British Columbia

%include "ts1325.inc"    ; Some useful LED and button macros

; IMPORTANT: [org xxx] MUST be the same as RelocAddr below.

[org E000h]
[bits 16]

; Only these three definitions may need to change

KernelBase equ  08000h   ; Where (32-bit) to locate and run RTEMS executable

RelocSeg   equ  9000h    ; Segment to relocate code.
RelocAddr  equ  0E000h   ; Address to relocate code, same as "org" above

; Next three used in GDT

RelocBase  equ  RelocSeg*16
Reloc15    equ  RelocBase & 0FFFFh
Reloc23    equ  RelocBase / 10000h

Buffer     equ  RelocAddr+400h   ; In same segment as RelocSeg
BuffSiz    equ  200h             ; Size of disk read + copy

StackSeg   equ  RelocSeg
StackSiz   equ  40h
StackAddr  equ  Buffer+BuffSiz+StackSiz

; Used to jump to kernel in real mode

KernelAddr equ  KernelBase & 0FFFFh
KernelSeg  equ  (KernelBase - KernelAddr) / 16

; Used to load from the ramdisk

Extended   equ  100000h  ; Start of extended memory / C: ramdisk
OffsetBPB  equ  0Bh      ; Start of BIOS param block in bootsector

; Command-line parameters

ParamLen   equ  80h      ; Byte length of command line params
ParamStr   equ  82h      ; Start of param string

; The ORG address above means pre-relocation addresses are wrong. The
; following macro fixes these up.

%define PRE_RELOC_ADDR(addr) (addr-CodeStart+100h)

CodeStart:

mov  dx, PRE_RELOC_ADDR(Greet)
mov  ah, 9h
int  21h

mov  ax, 0b021h       ; Exit to DOS if push-button switch pressed
int  15h
and  al, 01h          ; Bit 0 == 0 if button pressed
jz   ButtonExit

xor cx, cx
mov cl, [ParamLen]   ; See if there is a command line arg
jcxz NameError

dec  cx              ; Nix leading space. Is this standard?
cmp  cx, 12          ; Limit to 12 chars: e.g. ABCDEFGH.IJK
jg   NameError
                     ; Damn. Should make sure of no ':' or '\' chars too.

; Required by "relocated" [org] statement above

mov  di, PRE_RELOC_ADDR(FName)  
mov  si, ParamStr
repne
movsb                ; Copy command line arg

; Make sure no ':' in filename. This forces using the default dir.

mov  di, PRE_RELOC_ADDR(FName)
mov  al, ':'
mov  cx, 12
repne
scasb
je   NameError

jmp  Relocate

ButtonExit:
mov  dx, PRE_RELOC_ADDR(Button)
jmp short  DosPrint

NameError:
mov  dx, PRE_RELOC_ADDR(FError)
jmp short  DosPrint

DosError:            ; Only call this AFTER relocation
mov  dx, RError

DosPrint:
mov  ah, 9h
int  21h

DosExit:
mov  ax, 04C00h      ; DOS Function: Exit program
int  21h             ; Call DOS. Terminate Program

Relocate:            ; Move this code down to RelocAddr

cld
mov  ax, RelocSeg
mov  es, ax           ; Set destination = RelocSeg:RelocAddr
mov  di, RelocAddr
mov  si, 100h         ; Source is ds:0100h i.e. a COM file
mov  cx, CodeEnd - CodeStart ; Size of all code

repne
movsb

; continue in copied code

jmp  RelocSeg:RelocAddr + (RelocStart - CodeStart) 

RelocStart:
cli
mov  ax, StackSeg
mov  ss, ax
mov  sp, StackAddr
mov  ax, cs
mov  ds, ax
mov  es, ax          ; Setup segments and stack
sti

mov  ah, 19h
int  21h
mov  [DDrive], al    ; Save current default drive

mov  ax, 3d00h       ; DOS Function: Open the file for reading
mov  dx, FName       ; Presume DS points at filename segment
int  21h
jc   DosError

GoodOpen:
mov  [FHndl], ax     ; Save file handle

mov  al, [DDrive]    ; Check if loading from C: drive (ramdisk)
cmp  al, 2
je   LoadRamdisk

LoadDosdisk:

; Here we are loading from A: drive. Use DOS calls to load the file into
; extended memory. Then copy from extended memory to `KernelBase'. This way
; we avoid overwriting DOS file I/O structures if reading directly into
; conventional (<640K) memory.

mov  edi, Extended   ; Destination for code read @ 1 Meg

ReadLoop:

mov  ah,3fh          ; DOS Function: Read data from the file
mov  bx, [FHndl]
mov  dx, Buffer      ; Address of data buffer
mov  cx, BuffSiz     ; Request BuffSiz bytes
int  21h
jc   DosError

GoodRead:

cmp  ax, cx          ; EOF reached? AX = # bytes read
pushf

add  ax, 3
shr  ax, 2           ; Copy buffer by dwords, # = (ax + 3)/4
movzx  ecx, ax
mov  esi, RelocBase + Buffer     ; Source for copy, destination is in edi

call CopyData32      ; Do protected-mode copy

popf
je   ReadLoop        ; Still data left, so read next chunk

mov  esi, Extended   ; Source for copy @ 1 Meg
mov  ecx, edi        ; Make count in dwords
sub  ecx, esi
add  ecx, 3
shr  ecx, 2
mov  edi, KernelBase ; Destination copy

call CopyData32      ; Move code into conventional memory
jmp  RunKernel

LoadRamdisk:

; Here we are loading from C: drive. Use protected mode to directly access
; the virtual disk sectors in extended memory and copy to `KernelBase'. 
; This way we avoid using DOS file I/O calls, except for an `open' earlier
; which tells us the file exists.

; Copy C: "bootsector" to buffer and save the BIOS parameter block

mov  esi, Extended
mov  edi, RelocBase + Buffer     ; Must be a 32-but address...
mov  ecx, 80h
call CopyData32

mov  si, Buffer + OffsetBPB
mov  di, SavBPB
mov  cx, EndBPB - SavBPB
repne
movsb

; Calculate  FAT, root dir, and data start addresses for the ramdisk

xor  eax, eax
mov  ebx, eax
mov  ecx, ebx

mov  ax, [ResSec]

mov  bl, [NumFAT]
imul bx, [SecFAT]

mov  cx, [NRoot]
shr  cx, 4           ; 10h directory entries per sector

add  bx, ax
add  cx, bx

mov  dx, [BpSect]
imul ax, dx
imul bx, dx
imul cx, dx

add  eax, Extended
add  ebx, Extended
add  ecx, Extended

mov  [BegFAT], eax
mov  [BegRoot], ebx
mov  [BegData], ecx

; Convert the saved filename to format used in directory entry. Assume
; there's a `.' in it. Hopefully this won't haunt us later...

mov  di, FName       ; Find the `.'
mov  al, '.'
mov  cx, 12
repne
scasb

mov  bx, di          ; di points to filename extension

mov  di, DirName
mov  si, FName
mov  cx, bx          ; Make count
sub  cx, si
dec cx
repne                ; Copy initial part of filename
movsb

mov  di, bx          ; Find the terminating zero
xor  al,al
mov  cx, 4
repne
scasb

mov  cx, di          ; Make count
sub  cx, bx
dec  cx
mov  si, bx
mov  di, DirName + 8
repne                ; Copy filename extension
movsb

mov  si, DirName     ; Convert the stupid thing to upper case
mov  di, si
mov  cx, 11

Cvt2Upper:

lodsb
cmp  al, 'a'
jb   NotLow
cmp  al, 'z'
ja   NotLow
xor  al, 20h

NotLow:

stosb
loop Cvt2Upper

; Now load in the root directory (temporarily) to find the first cluster
; of our file. Use KernelSeg:KernelAddr as temporary storage.

mov  esi, [BegRoot]
mov  edi, KernelBase
xor  ecx, ecx
mov  cx, [NRoot]
shl  cx, 3           ; Each root entry is 8 dwords
call CopyData32

mov  dx, [NRoot]     ; Max # of dir entries

mov  cx, KernelSeg   ; Setup segment selector for comparison
mov  es, cx
mov  di, KernelAddr

FindEntry:

mov  cx, 11
mov  si, DirName
push di
rep  cmpsb
pop  di
je   GotEntry
add  di, 20h         ; Point to next dir entry
dec  dx
jnz  FindEntry

int  3h              ; Should never get here...

GotEntry:

mov  eax, KernelBase ; Setup initial address for copy
mov  [CurrDst], eax

add  di, 32 - 6      ; Load first cluster number
mov  ax, [es:di]
mov  cx, ds          ; Fix `es' selector just in case
mov  es, cx

LoadKernel:

call LoadCluster     ; Load cluster `ax' to [CurrDst], update [CurrDst]

call NextCluster     ; Get next cluster number in ax

cmp  ax, 0FF8h       ; Repeat until EOF
jb   LoadKernel

RunKernel:

mov  ax, KernelSeg   ; Setup data segment and transfer control
mov  ds, ax

jmp  KernelSeg:KernelAddr  ; Huzzah!!


; Load cluster `ax' to [CurrDst], update [CurrDst]

LoadCluster:

push ax
sub  ax, 2           ; Cluster numbers start at 2
movzx eax, ax

xor  ecx, ecx        ; Calculate bytes in a cluster
mov  cl, [SpClst]
imul cx, [BpSect]

imul eax, ecx
add  eax, [BegData]  ; Start of cluster

shr  ecx, 2          ; Cluster size in dwords
mov  esi, eax        ; Copy source
mov  edi, [CurrDst]  ; Copy destination
call CopyData32

mov  [CurrDst], edi  ; Update dest
pop  ax              ; Restore cluster number

ret

; Search FAT (FAT12 format) for next cluster in file after `ax'.

NextCluster:

movzx ecx, ax        ; Calculate offset into FAT
shr  ax, 1
pushf
add  cx, ax

mov  esi, [BegFAT]   ; Copy word containing next cluster to buffer
add  esi, ecx
mov  edi, RelocBase + Buffer
xor  ecx, ecx
inc  ecx
call CopyData32

mov  ax, [Buffer]    ; Handle odd/even cluster numbers
popf
jnc  EvenCluster
shr  ax, 4

EvenCluster:

and  ax, 0FFFh
ret

; Enable the A20 line for accesses to extended memory.

EnableA20:
        in al,92h
        or al,2
        jmp short $+2
        jmp short $+2
        jmp short $+2
        out 92h,al
        ret

; The CopyData32 routine copies ecx dwords from esi to edi. Both esi
; and edi hold 32-bit values. CopyData32 runs in 32-bit protected mode.

CopyData32:
        cli

	call EnableA20   ; Put here in case file I/O screws with this
	                 ; or with the GDTR

	lgdt [GDTStart]  ; Initialize GDTR for 32-bit protected mode

        mov eax, cr0
        or al, 1
        mov cr0, eax     ;go to real flat mode

;	LED_GRN
;	PSW_WAIT

        jmp  dword 8h : RelocBase+ProtJmp
[bits 32]
ProtJmp:
;	LED_YEL
;	PSW_WAIT

        mov ax, 10h
        mov ds, ax
        mov es, ax
        mov ss, ax

        rep movsd       ;copy the sector to where it should be

        mov ax, 20h
        mov ds, ax
        mov es, ax
        mov ss, ax

;	LED_RED
;	PSW_WAIT

        jmp 18h : RealJmp1  ;use code segment with 64K limit
[bits 16]
RealJmp1:
;	LED_OFF
;	PSW_WAIT

        mov eax, cr0      ;back to real segmented mode
        and eax, 0fffffffeh
        mov cr0, eax

        jmp  RelocSeg : RealJmp2
RealJmp2:
;	LED_GRN
;	PSW_WAIT

        mov ax, cs       
        mov es, ax
        mov ds, ax
        mov ss, ax

        sti
ret

; Storage for a Dos 3+ BIOS Parameter Block (for the C: ramdisk)

SavBPB:

BpSect	dw  0h       ; Bytes per sector, always 512
SpClst	db  0h       ; Sectors per cluster
ResSec	dw  0h       ; Num of reserved sectors
NumFAT	db  0h       ; Num of FATs
NRoot	dw  0h       ; Num of root directory entries
TotSec	dw  0h       ; Total sectors
Media	db  0h       ; Media descriptor byte
SecFAT	dw  0h       ; Sectors per FAT

EndBPB:

CurrDst	dd  0h       ; Current destination address for copying RTEMS exec

; Important (32-bit) address for the C: ramdisk

BegFAT	dd  0h       ; Start of the FAT
BegRoot	dd  0h       ; Start of root directory
BegData	dd  0h       ; Start of data clusters

DDrive	db  0h       ; Default drive: 0h = A:, 2h = C:

DirName	times 11 db 32 ; Room for 8.3 directory entry name

FName   times 13 db 0  ; Room for a 12 character null-terminated string
FHndl   dw  0000h

Greet	db  "RTEMS DOS Loader (c) 1999 Tony R. Ambardar",13,10,"$"
Button	db  "Button pressed -- Aborting.",13,10,"$"
FError	db  "Missing or incorrect file name.",13,10,"$"
RError	db  "Error opening or reading file.",13,10,"$"

; Global Descriptor Table used for protectd mode.
; Store the GDTR in the first null GDT entry

GDTStart:

dw GDTEnd - GDTStart - 1
dd RelocBase + GDTStart
dw 0

; base=0h, limit=4Gb, present, code, exec/read, conform, 32-bit 

dw 0ffffh   ;seg. lim. [15:0]
dw 0        ;base[15:0]
db 0        ;base[23:16]
db 9eh      ;p=1,dpl=0,s=1 ; code: execute/read, conforming
db 0cfh     ;c: gran=4K, D/B=1(32-bit) ; f: seg. lim. [19:16] 
db 0        ;base[31:24]

; base=0h, limit=4Gb, present, data, read/write exp. up, 32-bit SP

dw 0ffffh   ;seg. lim. [15:0]
dw 0        ;base[15:0]
db 0        ;base[23:16]
db 92h      ;p=1,dpl=0,s=1 ; data: read/write expand-up
db 0cfh     ;c: gran=4K, D/B=1(32-bit) ; f: seg. lim. [19:16]
db 0        ;base[31:24]

; base=0h, limit=ffffh, present, code, exec/read, conform, 16-bit 
; NOTE: this descriptor is used to change back to real mode.

dw 0ffffh   ;seg. lim. [15:0]
dw Reloc15  ;base[15:0]
db Reloc23  ;base[23:16]
db 9eh      ;p=1,dpl=0,s=1 ; code: execute/read, conforming
db 000h     ;4: gran=1 byte, D/B=0(16-bit) ; 0: seg. lim. [19:16] 
db 0        ;base[31:24]

; base=0h, limit=ffffh, present, data, read/write exp. up, 16-bit SP
; NOTE: this descriptor is used to change back to real mode.

dw 0ffffh   ;seg. lim. [15:0]
dw Reloc15  ;base[15:0]
db Reloc23  ;base[23:16]
db 92h      ;p=1,dpl=0,s=1 ; data: read/write expand-up
db 000h     ;0: gran=1 byte, D/B=0(16-bit) ; 0: seg. lim. [19:16]
db 0        ;base[31:24]

GDTEnd:

CodeEnd:                    ; end-of-code marker for copy

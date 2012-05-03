/* Exception Vector definitions follow */

	/* !!! Warning !!! This table is not tested, and
	   the user must make sure it is complete. */

	/* If we use TRAP #15 for reboot, note that group 0 and 1 exceptions
	   will have priority. */

	/* This is the "magic word" that CPU32bug uses to indicate that
	   there is a bootable image here. */
	.long   0xBEEFBEEF

	/* Vector 0:	RESET:	Initial SSP */
	.long	_RamEnd
	/* Vector 1:	RESET:	Initial PC */
	.long	start

	/* default action for undefined vectors is to re-boot */

	/* Note group 0 and 1 exception (like trace) have priority
	   over other exceptions (like trap #15) that may call this. */

	/* Vectors 2-255 */
	.long reboot   /* exception vector:   2 */
	.long reboot   /* exception vector:   3 */
	.long reboot   /* exception vector:   4 */
	.long reboot   /* exception vector:   5 */
	.long reboot   /* exception vector:   6 */
	.long reboot   /* exception vector:   7 */
	.long reboot   /* exception vector:   8 */
	.long reboot   /* exception vector:   9 */
	.long reboot   /* exception vector:  10 */
	.long reboot   /* exception vector:  11 */
	.long reboot   /* exception vector:  12 */
	.long reboot   /* exception vector:  13 */
	.long reboot   /* exception vector:  14 */
	.long reboot   /* exception vector:  15 */
	.long reboot   /* exception vector:  16 */
	.long reboot   /* exception vector:  17 */
	.long reboot   /* exception vector:  18 */
	.long reboot   /* exception vector:  19 */
	.long reboot   /* exception vector:  20 */
	.long reboot   /* exception vector:  21 */
	.long reboot   /* exception vector:  22 */
	.long reboot   /* exception vector:  23 */
	.long reboot   /* exception vector:  24 */
	.long reboot   /* exception vector:  25 */
	.long reboot   /* exception vector:  26 */
	.long reboot   /* exception vector:  27 */
	.long reboot   /* exception vector:  28 */
	.long reboot   /* exception vector:  29 */
	.long reboot   /* exception vector:  30 */
	.long reboot   /* exception vector:  31 */
	.long reboot   /* exception vector:  32 */
	.long reboot   /* exception vector:  33 */
	.long reboot   /* exception vector:  34 */
	.long reboot   /* exception vector:  35 */
	.long reboot   /* exception vector:  36 */
	.long reboot   /* exception vector:  37 */
	.long reboot   /* exception vector:  38 */
	.long reboot   /* exception vector:  39 */
	.long reboot   /* exception vector:  40 */
	.long reboot   /* exception vector:  41 */
	.long reboot   /* exception vector:  42 */
	.long reboot   /* exception vector:  43 */
	.long reboot   /* exception vector:  44 */
	.long reboot   /* exception vector:  45 */
	.long reboot   /* exception vector:  46 */
	.long _reboot  /* the reboot trap:   47 */
	.long reboot   /* exception vector:  48 */
	.long reboot   /* exception vector:  49 */
	.long reboot   /* exception vector:  50 */
	.long reboot   /* exception vector:  51 */
	.long reboot   /* exception vector:  52 */
	.long reboot   /* exception vector:  53 */
	.long reboot   /* exception vector:  54 */
	.long reboot   /* exception vector:  55 */
	.long reboot   /* exception vector:  56 */
	.long reboot   /* exception vector:  57 */
	.long reboot   /* exception vector:  58 */
	.long reboot   /* exception vector:  59 */
	.long reboot   /* exception vector:  60 */
	.long reboot   /* exception vector:  61 */
	.long reboot   /* exception vector:  62 */
	.long reboot   /* exception vector:  63 */
	.long reboot   /* exception vector:  64 */
	.long reboot   /* exception vector:  65 */
	.long reboot   /* exception vector:  66 */
	.long reboot   /* exception vector:  67 */
	.long reboot   /* exception vector:  68 */
	.long reboot   /* exception vector:  69 */
	.long reboot   /* exception vector:  70 */
	.long reboot   /* exception vector:  71 */
	.long reboot   /* exception vector:  72 */
	.long reboot   /* exception vector:  73 */
	.long reboot   /* exception vector:  74 */
	.long reboot   /* exception vector:  75 */
	.long reboot   /* exception vector:  76 */
	.long reboot   /* exception vector:  77 */
	.long reboot   /* exception vector:  78 */
	.long reboot   /* exception vector:  79 */
	.long reboot   /* exception vector:  80 */
	.long reboot   /* exception vector:  81 */
	.long reboot   /* exception vector:  82 */
	.long reboot   /* exception vector:  83 */
	.long reboot   /* exception vector:  84 */
	.long reboot   /* exception vector:  85 */
	.long reboot   /* exception vector:  86 */
	.long reboot   /* exception vector:  87 */
	.long reboot   /* exception vector:  88 */
	.long reboot   /* exception vector:  89 */
	.long reboot   /* exception vector:  90 */
	.long reboot   /* exception vector:  91 */
	.long reboot   /* exception vector:  92 */
	.long reboot   /* exception vector:  93 */
	.long reboot   /* exception vector:  94 */
	.long reboot   /* exception vector:  95 */
	.long reboot   /* exception vector:  96 */
	.long reboot   /* exception vector:  97 */
	.long reboot   /* exception vector:  98 */
	.long reboot   /* exception vector:  99 */
	.long reboot   /* exception vector: 100 */
	.long reboot   /* exception vector: 101 */
	.long reboot   /* exception vector: 102 */
	.long reboot   /* exception vector: 103 */
	.long reboot   /* exception vector: 104 */
	.long reboot   /* exception vector: 105 */
	.long reboot   /* exception vector: 106 */
	.long reboot   /* exception vector: 107 */
	.long reboot   /* exception vector: 108 */
	.long reboot   /* exception vector: 109 */
	.long reboot   /* exception vector: 110 */
	.long reboot   /* exception vector: 111 */
	.long reboot   /* exception vector: 112 */
	.long reboot   /* exception vector: 113 */
	.long reboot   /* exception vector: 114 */
	.long reboot   /* exception vector: 115 */
	.long reboot   /* exception vector: 116 */
	.long reboot   /* exception vector: 117 */
	.long reboot   /* exception vector: 118 */
	.long reboot   /* exception vector: 119 */
	.long reboot   /* exception vector: 120 */
	.long reboot   /* exception vector: 121 */
	.long reboot   /* exception vector: 122 */
	.long reboot   /* exception vector: 123 */
	.long reboot   /* exception vector: 124 */
	.long reboot   /* exception vector: 125 */
	.long reboot   /* exception vector: 126 */
	.long reboot   /* exception vector: 127 */
	.long reboot   /* exception vector: 128 */
	.long reboot   /* exception vector: 129 */
	.long reboot   /* exception vector: 130 */
	.long reboot   /* exception vector: 131 */
	.long reboot   /* exception vector: 132 */
	.long reboot   /* exception vector: 133 */
	.long reboot   /* exception vector: 134 */
	.long reboot   /* exception vector: 135 */
	.long reboot   /* exception vector: 136 */
	.long reboot   /* exception vector: 137 */
	.long reboot   /* exception vector: 138 */
	.long reboot   /* exception vector: 139 */
	.long reboot   /* exception vector: 140 */
	.long reboot   /* exception vector: 141 */
	.long reboot   /* exception vector: 142 */
	.long reboot   /* exception vector: 143 */
	.long reboot   /* exception vector: 144 */
	.long reboot   /* exception vector: 145 */
	.long reboot   /* exception vector: 146 */
	.long reboot   /* exception vector: 147 */
	.long reboot   /* exception vector: 148 */
	.long reboot   /* exception vector: 149 */
	.long reboot   /* exception vector: 150 */
	.long reboot   /* exception vector: 151 */
	.long reboot   /* exception vector: 152 */
	.long reboot   /* exception vector: 153 */
	.long reboot   /* exception vector: 154 */
	.long reboot   /* exception vector: 155 */
	.long reboot   /* exception vector: 156 */
	.long reboot   /* exception vector: 157 */
	.long reboot   /* exception vector: 158 */
	.long reboot   /* exception vector: 159 */
	.long reboot   /* exception vector: 160 */
	.long reboot   /* exception vector: 161 */
	.long reboot   /* exception vector: 162 */
	.long reboot   /* exception vector: 163 */
	.long reboot   /* exception vector: 164 */
	.long reboot   /* exception vector: 165 */
	.long reboot   /* exception vector: 166 */
	.long reboot   /* exception vector: 167 */
	.long reboot   /* exception vector: 168 */
	.long reboot   /* exception vector: 169 */
	.long reboot   /* exception vector: 170 */
	.long reboot   /* exception vector: 171 */
	.long reboot   /* exception vector: 172 */
	.long reboot   /* exception vector: 173 */
	.long reboot   /* exception vector: 174 */
	.long reboot   /* exception vector: 175 */
	.long reboot   /* exception vector: 176 */
	.long reboot   /* exception vector: 177 */
	.long reboot   /* exception vector: 178 */
	.long reboot   /* exception vector: 179 */
	.long reboot   /* exception vector: 180 */
	.long reboot   /* exception vector: 181 */
	.long reboot   /* exception vector: 182 */
	.long reboot   /* exception vector: 183 */
	.long reboot   /* exception vector: 184 */
	.long reboot   /* exception vector: 185 */
	.long reboot   /* exception vector: 186 */
	.long reboot   /* exception vector: 187 */
	.long reboot   /* exception vector: 188 */
	.long reboot   /* exception vector: 189 */
	.long reboot   /* exception vector: 190 */
	.long reboot   /* exception vector: 191 */
	.long reboot   /* exception vector: 192 */
	.long reboot   /* exception vector: 193 */
	.long reboot   /* exception vector: 194 */
	.long reboot   /* exception vector: 195 */
	.long reboot   /* exception vector: 196 */
	.long reboot   /* exception vector: 197 */
	.long reboot   /* exception vector: 198 */
	.long reboot   /* exception vector: 199 */
	.long reboot   /* exception vector: 200 */
	.long reboot   /* exception vector: 201 */
	.long reboot   /* exception vector: 202 */
	.long reboot   /* exception vector: 203 */
	.long reboot   /* exception vector: 204 */
	.long reboot   /* exception vector: 205 */
	.long reboot   /* exception vector: 206 */
	.long reboot   /* exception vector: 207 */
	.long reboot   /* exception vector: 208 */
	.long reboot   /* exception vector: 209 */
	.long reboot   /* exception vector: 210 */
	.long reboot   /* exception vector: 211 */
	.long reboot   /* exception vector: 212 */
	.long reboot   /* exception vector: 213 */
	.long reboot   /* exception vector: 214 */
	.long reboot   /* exception vector: 215 */
	.long reboot   /* exception vector: 216 */
	.long reboot   /* exception vector: 217 */
	.long reboot   /* exception vector: 218 */
	.long reboot   /* exception vector: 219 */
	.long reboot   /* exception vector: 220 */
	.long reboot   /* exception vector: 221 */
	.long reboot   /* exception vector: 222 */
	.long reboot   /* exception vector: 223 */
	.long reboot   /* exception vector: 224 */
	.long reboot   /* exception vector: 225 */
	.long reboot   /* exception vector: 226 */
	.long reboot   /* exception vector: 227 */
	.long reboot   /* exception vector: 228 */
	.long reboot   /* exception vector: 229 */
	.long reboot   /* exception vector: 230 */
	.long reboot   /* exception vector: 231 */
	.long reboot   /* exception vector: 232 */
	.long reboot   /* exception vector: 233 */
	.long reboot   /* exception vector: 234 */
	.long reboot   /* exception vector: 235 */
	.long reboot   /* exception vector: 236 */
	.long reboot   /* exception vector: 237 */
	.long reboot   /* exception vector: 238 */
	.long reboot   /* exception vector: 239 */
	.long reboot   /* exception vector: 240 */
	.long reboot   /* exception vector: 241 */
	.long reboot   /* exception vector: 242 */
	.long reboot   /* exception vector: 243 */
	.long reboot   /* exception vector: 244 */
	.long reboot   /* exception vector: 245 */
	.long reboot   /* exception vector: 246 */
	.long reboot   /* exception vector: 247 */
	.long reboot   /* exception vector: 248 */
	.long reboot   /* exception vector: 249 */
	.long reboot   /* exception vector: 250 */
	.long reboot   /* exception vector: 251 */
	.long reboot   /* exception vector: 252 */
	.long reboot   /* exception vector: 253 */
	.long reboot   /* exception vector: 254 */
	.long reboot   /* exception vector: 255 */

_reboot:
	move	#0x2700,%sr	/* mask interrupts */
	movea.l	(0x0).w,%a7	/* load stack */
	movea.l	(0x4).w,%a0	/* jmp to location of reset vector */
	jmp	(%a0)

reboot:
	trap	#15		/* use trap exception to enter supervisor
				   state. Trace mode ( and other group 0
				   and 1 exceptions) *could* screw this up if
				   not vectored to reboot or did not return. */

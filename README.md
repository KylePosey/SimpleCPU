# SimpleCPU
 A very simple 8-bit CPU software implementation. 
 
 ##Features##
 <ul>
	<li>-15 complete instructions</li>
	<li>8-bit registers, with 16-bit addressing to access up to 64k addresses</li>
 </ul>
 
 ##Memory Map##
 **Note:** *This memory map layout is certaintly not the best or most efficient way to do this, but it has worked without much issue thus far.*
 
	|0x0000 - 0xFFFF | Full range of addresable ram. 64k|
	|----------------|----------------------------------|
	|0x0000 - 0x3E80 | Rom load section. 16k			|
	|0x3E81 - 0x4268 | Stack. 1k						|
	|0x4269 - 0xFFFF | Free ram. 46k					|
	
 ##Goals##
 <ul> 
	<li>Add remaining needed instructions such as branching and bitwise operations</li>
	<li>Write an assembler so code can be written in primitive assembly rather than byte code</li>
	<li>Create a terminal application that can be manipulated through code with mapped io address space</li>
 </ul> 

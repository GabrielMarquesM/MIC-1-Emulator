# MIC-1 Emulator & IJVM Assembler

This [MIC-1](https://en.wikipedia.org/wiki/MIC-1) Emulator and [IJVM](https://en.wikipedia.org/wiki/IJVM) Assembler was made as project for the Computer Architecture course.

Most of the comments are in Portuguese, and could possibly be translated later.

- IJVM:
 Put the instructions into instructionsread.txt as in this example:
 ```	
      bipush 9 
      istore x
      bipush 7
      istore y

loop bipush 1
	  iload z
	  iadd 
      istore z 
      iload z
      iload y
      if_icmpeq end
      iload result
      iload x
      iadd
      istore result
      goto loop

end  iload result
      iload x 
      iadd
      istore result
      iload result
      nop		
  ```
 Run main.py, instructionsfinal.bin is now ready to be used on the MIC-1.
  
 - MIC-1:
    - Put instructionsfinal.bin in the same folder as main.cpp.
    - Compile and run main.cpp
    - Keep pressing Enter and you'll see it working.
    
   ![""](/mic-1.png)
    
  

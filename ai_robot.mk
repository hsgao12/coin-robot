SHELL=cmd
CC = xc32-gcc
OBJCPY = xc32-bin2hex
ARCH = -mprocessor=32MX130F064B
PORTN=$(shell type COMPORT.inc)

OBJ = ai_robot.o

ai_robot.elf: $(OBJ)
	$(CC) $(ARCH) -o ai_robot.elf ai_robot.o -mips16 -DXPRJ_default=default -legacy-libc -Wl,-Map=ai_robot.map
	$(OBJCPY) ai_robot.elf
	@echo Success!
   
ai_robot.o: ai_robot.c
	$(CC) -g -x c -mips16 -Os -c $(ARCH) -MMD -o ai_robot.o ai_robot.c -DXPRJ_default=default -legacy-libc

clean:
	@del *.o *.d *.map *.elf *.hex 2>NUL

FlashLoad:
	@Taskkill /IM putty.exe /F 2>NUL | wait 500
	pro32 -p -v ai_robot.hex
	putty.exe -serial $(PORTN) -sercfg 115200,8,n,1,N -v

putty:
	@Taskkill /IM putty.exe /F 2>NUL | wait 500
	putty.exe -serial $(PORTN) -sercfg 115200,8,n,1,N -v

dummy: ai_robot.hex ai_robot.map
	@echo ;-)
	
explorer:
	explorer .

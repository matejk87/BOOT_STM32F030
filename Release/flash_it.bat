SET proj=boot_STM32f030
SET dir=E:\projekti\smarhouse\bootloader\BOOT_STM32F030\Release\
SET dirproj=%dir%%proj%
rm %dirproj%.bin %dirproj%.hex
arm-none-eabi-objcopy -O binary %dirproj%.elf %dirproj%.bin
arm-none-eabi-objcopy -O ihex %dirproj%.elf %dirproj%.hex
ST-LINK_CLI.exe -ME
ST-LINK_CLI.exe -P "%dirproj%.bin" 0x08000000 -Rst
ST-LINK_CLI.exe -HardRst
sleep 2


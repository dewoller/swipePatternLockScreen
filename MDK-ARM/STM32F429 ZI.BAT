SET PATH=C:\Keil_v5\ARM\ARMCC\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\CooCox\CoIDE\gcc\bin;C:\Program Files (x86)\GNU Tools ARM Embedded\4.8 2014q2\bin
SET CPU_TYPE=STM32F429ZI
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=STM32F429 ZI
SET CPU_CLOCK=0x007A1200
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\system_stm32f4xx.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmAsm" --Via ".\startup_stm32f429_439xx._ia"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\misc.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_usart.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_spi.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_adc.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_fmc.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_i2c.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_ili9341.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_spi.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_fonts.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_delay.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_ili9341_button.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_stmpe811.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_i2c.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\tm_stm32f4_disco.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\main.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmCC" --Via ".\stm32f4xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\bin\ArmLink" --Via ".\project.lnp"
"C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe" ".\project.axf" --i32combined --output ".\project.hex"

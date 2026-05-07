sftp -P 50288 zx@frp-ski.com:/home/zx/WD_disk_0/STMicroelectronics/STM32F103C8T6/00-LED-Blink/led_test.bin
st-flash --format binary write /home/radxa/myfile/led_test.bin 0x08000000

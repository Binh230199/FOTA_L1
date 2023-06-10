# FOTA_L1
Update firmware over the Air - Bootloader and OTA firmware

Update the firmware for the IoT device over 4G.

Bootloader: used to jump to the valid application firmware.
OTA Application: used to process, write, and validate the new firmware. After that, the MCUs reset to go to the bootloader.

Devices:
- Module SIM7600.
- MCU STM32L1 - ARM Cortex - M3
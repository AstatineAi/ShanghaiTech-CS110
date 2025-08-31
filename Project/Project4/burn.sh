#!/bin/bash
sudo dfu-util -a 0 --dfuse-address 0x08000000:leave -D ./.pio/build/sipeed-longan-nano/firmware.bin -S 3CBJ

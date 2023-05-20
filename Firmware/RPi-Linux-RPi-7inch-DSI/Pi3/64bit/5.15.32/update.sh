#!/bin/bash

echo -e "\nInfo: RaspBerryPi-3, kernel 5.15.32, 64-bit OS."

path_dts=/boot/overlays
path_touch=/lib/modules/$(uname -r)/kernel/drivers/input/touchscreen
path_dis=/lib/modules/$(uname -r)/kernel/drivers/gpu/drm/panel

echo -e "\nCopy files...\n"
sudo rm $path_dts/vc4-kms-dsi-7inch.dtbo
sudo rm $path_dts/edt-ft5406.dtbo
sudo cp vc4-kms-dsi-7inch.dtbo $path_dts -frv
sudo cp edt-ft5406.dtbo $path_dts -frv

sudo rm $path_touch/edt-ft5x06.ko
sudo cp edt-ft5x06.ko $path_touch -frv

sudo rm $path_dis/panel-simple.ko
sudo cp panel-simple.ko $path_dis -frv

sync

echo -e "\nInstall modules..."
sudo depmod
sudo modprobe -r panel_simple
sudo modprobe -r edt_ft5x06

sudo modprobe panel_simple
[ $? != 0 ] && echo -e "\nInstall panel_simple.ko failed! \n" && exit 1

sudo modprobe edt_ft5x06
[ $? != 0 ] && echo -e "\nInstall edt_ft5x06.ko failed! \n" && exit 1

echo -e "\nInstallation completed!\nRebooting now..."

sync

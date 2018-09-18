# UEFI FAT partition with bootable EFI application.
dd if=/dev/zero of=uefi.img bs=512 count=262144
parted uefi.img -s "mktable gpt"
parted uefi.img -s "mkpart p 2048s 200000s"
parted uefi.img -s "name 1 UEFI"

dd if=/dev/zero of=fat.img bs=512 count=131072
mkfs.vfat -n UEFI-TOOLS fat.img

mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mcopy -i fat.img BOOTIA32.EFI ::/EFI/BOOT/BOOTIA32.EFI

dd if=fat.img of=uefi.img bs=512 seek=2048 count=131072 conv=notrunc


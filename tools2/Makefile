
all : rootfs.img

rootfs.img : mkfs2 fdisk2 proto.rootfs
	./fdisk2 $@ 10
	sync
	./mkfs2 $@ proto.rootfs

mkfs2 : mkfs2.c
	gcc -DHD -o $@ $<

fdisk2 : fdisk2.c
	gcc -DHD -o $@ $<

clean :
	rm -fr mkfs2 fdisk2 rootfs.img

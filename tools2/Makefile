
all : rootfs.img

rootfs.img : chmem2 mkfs2 fdisk2 proto.rootfs
	./chmem2 =2000 ../cmds/cc.sep
	./chmem2 =2000 ../cmds/time.sep
	./chmem2 =2000 ../cmds/tail.sep
	./chmem2 =30000 ../cmds/sort.sep
	./chmem2 =16000 ../cmds/mkfs.sep
	./chmem2 =8000 ../cmds/dd.sep
	./chmem2 =8000 ../cmds/rmdir.sep
	./chmem2 =8000 ../cmds/mkdir.sep
	./chmem2 =32000 ../cmds/dosread.sep
	./chmem2 =20000 ../cmds/make.sep
	./chmem2 =8000 ../cmds/gres.sep
	./chmem2 =8000 ../cmds/grep.sep
	./chmem2 =8000 ../cmds/sh.sep
	./chmem2 =8000 ../cmds/gres.sep
	./chmem2 =8000 ../cmds/mv.sep
	./chmem2 =8000 ../cmds/rm.sep
	./chmem2 =8000 ../cmds/pr.sep
	./chmem2 =64000 ../cmds/libpack.sep
	./chmem2 =64000 ../cmds/libupack.sep
	./chmem2 =64000 ../cmds/mined.sep
	./chmem2 =500 ../cmds/update.sep
	sync
	./fdisk2 $@ 10
	sync
	./mkfs2 $@ proto.rootfs
	sync

mkfs2 : mkfs2.c
	gcc -DHD -o $@ $<

fdisk2 : fdisk2.c
	gcc -DHD -o $@ $<

chmem2 : chmem2.c
	gcc -DHD -o $@ $<

clean :
	rm -fr chmem2 mkfs2 fdisk2 rootfs.img

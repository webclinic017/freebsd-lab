KMOD= hello
SRCS= hello.c

.include <bsd.kmod.mk>

i:
	kldload ./hello.ko

r:
	kldunload ./hello.ko

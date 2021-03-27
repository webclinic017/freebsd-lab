/*
 * Simple Echo pseudo-device KLD
 *
 * Murray Stokely
 * Søren (Xride) Straarup
 * Eitan Adler
 */

#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>  /* uprintf */
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */
#include <sys/conf.h>   /* cdevsw struct */
#include <sys/uio.h>    /* uio struct */
#include <sys/malloc.h>

#define BUFFERSIZE 255

/* Function prototypes */
static d_open_t      echo_open;
static d_close_t     echo_close;
static d_read_t      echo_read;
static d_write_t     echo_write;

/* Character device entry points */
static struct cdevsw echo_cdevsw = {
	.d_version = D_VERSION,
	.d_open = echo_open,
	.d_close = echo_close,
	.d_read = echo_read,
	.d_write = echo_write,
	.d_name = "echo",
};

struct s_echo {
	char msg[BUFFERSIZE + 1];
	int len;
};

/* vars */
static struct cdev *echo_dev;
static struct s_echo *echomsg;

MALLOC_DECLARE(M_ECHOBUF);
MALLOC_DEFINE(M_ECHOBUF, "echobuffer", "buffer for echo module");

/*
 * This function is called by the kld[un]load(2) system calls to
 * determine what actions to take when a module is loaded or unloaded.
 */
static int
echo_loader(struct module *m __unused, int what, void *arg __unused)
{
	int error = 0;

	switch (what) {
	case MOD_LOAD:                /* kldload */
		error = make_dev_p(MAKEDEV_CHECKNAME | MAKEDEV_WAITOK,
		    &echo_dev,
		    &echo_cdevsw,
		    0,
		    UID_ROOT,
		    GID_WHEEL,
		    0600,
		    "echo");
		if (error != 0)
			break;

		echomsg = malloc(sizeof(*echomsg), M_ECHOBUF, M_WAITOK |
		    M_ZERO);
		printf("Echo device loaded.\n");
		break;
	case MOD_UNLOAD:
		destroy_dev(echo_dev);
		free(echomsg, M_ECHOBUF);
		printf("Echo device unloaded.\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}
	return (error);
}

static int
echo_open(struct cdev *dev __unused, int oflags __unused, int devtype __unused,
    struct thread *td __unused)
{
	int error = 0;

	uprintf("Opened device \"echo\" successfully.\n");
	return (error);
}

static int
echo_close(struct cdev *dev __unused, int fflag __unused, int devtype __unused,
    struct thread *td __unused)
{

	uprintf("Closing device \"echo\".\n");
	return (0);
}

/*
 * The read function just takes the buf that was saved via
 * echo_write() and returns it to userland for accessing.
 * uio(9)
 */
static int
echo_read(struct cdev *dev __unused, struct uio *uio, int ioflag __unused)
{
	size_t amt;
	int error;

	/*
	 * How big is this read operation?  Either as big as the user wants,
	 * or as big as the remaining data.  Note that the 'len' does not
	 * include the trailing null character.
	 */
	amt = MIN(uio->uio_resid, uio->uio_offset >= echomsg->len + 1 ? 0 :
	    echomsg->len + 1 - uio->uio_offset);

	if ((error = uiomove(echomsg->msg, amt, uio)) != 0)
		uprintf("uiomove failed!\n");

	return (error);
}

/*
 * echo_write takes in a character string and saves it
 * to buf for later accessing.
 */
static int
echo_write(struct cdev *dev __unused, struct uio *uio, int ioflag __unused)
{
	size_t amt;
	int error;

	/*
	 * We either write from the beginning or are appending -- do
	 * not allow random access.
	 */
	if (uio->uio_offset != 0 && (uio->uio_offset != echomsg->len))
		return (EINVAL);

	/* This is a new message, reset length */
	if (uio->uio_offset == 0)
		echomsg->len = 0;

	/* Copy the string in from user memory to kernel memory */
	amt = MIN(uio->uio_resid, (BUFFERSIZE - echomsg->len));

	error = uiomove(echomsg->msg + uio->uio_offset, amt, uio);

	/* Now we need to null terminate and record the length */
	echomsg->len = uio->uio_offset;
	echomsg->msg[echomsg->len] = 0;

	if (error != 0)
		uprintf("Write failed: bad address!\n");
	return (error);
}

DEV_MODULE(echo, echo_loader, NULL);

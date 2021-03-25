#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

static int
hello_modevent(module_t mod __unused, int event, void *arg __unused)
{
   int error = 0;
   switch (event)
   {
      case MOD_LOAD:
         uprintf("Hello, world!\n");
         break;
      case MOD_UNLOAD:
         uprintf("Good-bye, cruel world!\n");
         break;
      default:
         error = EOPNOTSUPP;
         break;
   }
   return (error);
}

static moduledata_t hello_mod =
{
   "hello",
   hello_modevent,
   NULL
};

DECLARE_MODULE(hello, hello_mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

/*
 * From: /usr/src/sys/sys/module.h <sys/module.h>
 * ----------------------------------------------
typedef enum modeventtype {
   MOD_LOAD,        // Set when module is loaded.
   MOD_UNLOAD,      // Set when module is unloaded.
   MOD_SHUTDOWN,    // Set on shutdown.
   MOD_QUIESCE      // Set on quiesce.
} modeventtype_t;


#define	DECLARE_MODULE(name, data, sub, order)				\
	DECLARE_MODULE_WITH_MAXVER(name, data, sub, order, __FreeBSD_version)



*/

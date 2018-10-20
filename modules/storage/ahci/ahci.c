#include "kernel/include/modules/export.h"
#include "kernel/include/video/print.h"

K_MODULE_NAME("Advanced Host Controller Interface");

K_MODULE_INIT()
{
	k_printf("I am here :)\n");
}

K_MODULE_UNINIT()
{

}


#ifndef K_MODULES_EXPORT_H
#define K_MODULES_EXPORT_H

#define K_MODULE_MAX_NAME_LENGTH	64

#define K_MODULE_NAME(name)						\
	static const char __k_mod_name[K_MODULE_MAX_NAME_LENGTH]	\
	__attribute__((section(".k_mod_name"), used)) = { name }

#define K_MODULE_INIT()		void k_module_init(void)
#define K_MODULE_UNINIT()	void k_module_uninit(void)

#endif


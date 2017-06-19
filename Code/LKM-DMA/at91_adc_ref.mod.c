#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("platform:at91sam9260-adc");
MODULE_ALIAS("platform:at91sam9rl-adc");
MODULE_ALIAS("platform:at91sam9g45-adc");
MODULE_ALIAS("platform:at91sam9x5-adc");
MODULE_ALIAS("of:N*T*Catmel,at91sam9260-adc*");
MODULE_ALIAS("of:N*T*Catmel,at91sam9rl-adc*");
MODULE_ALIAS("of:N*T*Catmel,at91sam9g45-adc*");
MODULE_ALIAS("of:N*T*Catmel,at91sam9x5-adc*");

#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x9479a1e8, "strnlen" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xf64ac983, "__copy_overflow" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x534ed5f3, "__msecs_to_jiffies" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xbbb26ca8, "cdev_init" },
	{ 0xfaadbd72, "cdev_add" },
	{ 0xb3d7f998, "class_create" },
	{ 0x01f403fc, "device_create" },
	{ 0x962cecbf, "class_destroy" },
	{ 0x881f0858, "cdev_del" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0x3124a198, "device_destroy" },
	{ 0x1c9578f7, "param_ops_int" },
	{ 0x1c9578f7, "param_array_ops" },
	{ 0xd272d446, "__fentry__" },
	{ 0x058c185a, "jiffies" },
	{ 0x546c19d9, "validate_usercopy_range" },
	{ 0x984622ae, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xa61fd7aa,
	0x092a35a2,
	0x90a48d82,
	0xe8213e80,
	0xd272d446,
	0x9479a1e8,
	0xe54e0a6b,
	0xf64ac983,
	0x092a35a2,
	0x534ed5f3,
	0x9f222e1e,
	0xbbb26ca8,
	0xfaadbd72,
	0xb3d7f998,
	0x01f403fc,
	0x962cecbf,
	0x881f0858,
	0x0bc5fb0d,
	0x3124a198,
	0x1c9578f7,
	0x1c9578f7,
	0xd272d446,
	0x058c185a,
	0x546c19d9,
	0x984622ae,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__check_object_size\0"
	"_copy_from_user\0"
	"__ubsan_handle_out_of_bounds\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"strnlen\0"
	"__fortify_panic\0"
	"__copy_overflow\0"
	"_copy_to_user\0"
	"__msecs_to_jiffies\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"cdev_add\0"
	"class_create\0"
	"device_create\0"
	"class_destroy\0"
	"cdev_del\0"
	"unregister_chrdev_region\0"
	"device_destroy\0"
	"param_ops_int\0"
	"param_array_ops\0"
	"__fentry__\0"
	"jiffies\0"
	"validate_usercopy_range\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "6F85FD42E154EEED57CFAD8");

#include <linux/module.h>	/* Debe incluirse en todos los módulos de kernel */
#include <linux/kernel.h>	/* Incluido para KERN_INFO */
#include <linux/init.h>	/* Contiene las macro de inicio y fin */

static int hello_init(void){
	printk(KERN_INFO "Hello, World\n");
	return 0;
} 

static void hello_exit(void){
	printk(KERN_INFO "Good bye World\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Esteban Tissot <egtissot@gmail.com>");
MODULE_DESCRIPTION("\"Hello World\" - \"Good bye World\"");

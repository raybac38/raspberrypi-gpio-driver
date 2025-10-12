#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>

MODULE_LICENSE("MIT");

#define GPIO_BASE_PHYS 0xFE200000
#define GPIO_SIZE 0xF4

volatile unsigned char * GPIO_BASE;

#define GPFSEL0  0x0
#define GPSET0   0x1c
#define GPCLR0   0x28

void set_pin_to_output(void);
void set_output(bool state);

static int controler_init(void){
    printk(KERN_ALERT "Controler gpio up\n");
    printk(KERN_INFO "The process is \"%s\" (pid %i)\n", current->comm, current->pid);

    /// Reservation de l'acces physique

    GPIO_BASE = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);

    if(GPIO_BASE)
    {
        printk(KERN_INFO "plage adresse GPIO reserved\n");
    }
    else {
        printk(KERN_ERR "Erreur ioremap\n");
        return -ENOMEM;
    }

    set_pin_to_output();
    set_output(true);

    return 0;
}

void set_pin_to_output(void){
    unsigned int * ptr = (unsigned int *) (GPIO_BASE + GPFSEL0);

    (*ptr) &= ~(0b111 << 6); /// clear 3 bit
    (*ptr) |= (0b001 << 6); /// Set the value
}

void set_output(bool state){

    if(state) {
        unsigned int * ptr = (unsigned int *) (GPIO_BASE + GPSET0);
        *(ptr) = (0b1 << 2);
    }
    else {
        unsigned int * ptr = (unsigned int *) (GPIO_BASE + GPCLR0);
        *(ptr) = (0b1 << 2);
    }
    
}

static void controler_exit(void){

    set_output(false);
    printk(KERN_INFO "Liberation de la plage d'adresse GPIO");
    if (GPIO_BASE){
        iounmap(GPIO_BASE);
    }

    printk(KERN_ALERT "Controler gpio down\n");
}

module_init(controler_init);
module_exit(controler_exit);

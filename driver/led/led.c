#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/io.h>


void __iomem   *virtual_address;
struct resource* requestested_mem_region;

#define GREEN_LED_GPIO      (47)            // GPIO pin of green LED
#define GPIO_ADDRESS        (0x3F200000)    // Start of GPIO address space
#define GPIO_ADDRESS_LEN    (0xB4)          // Length of GPIO address space (0x3F2000B4 - 0x3F200000)
#define GPFSEL4_OFFSET      (0x00000010)    // GPIO pin of greed LED is 47 and so we need to use GPFSEL4
#define GPIO47_OFFSET       (0x00000007)    // In GPFSEL4, the offset of GPIO47 is 7 (47-40)
#define GPIO_DIRECTION_OUT  (1)             // Output direction is 1 (input is 0)
#define GPSET1_OFFSET       (0x00000020)    // RPi has 54 GPIOs which are devided in two sets, GPIO47 is in set 1 (from 32 to 53)
#define GPCLR1_OFFSET       (0x0000002C)    // Clear pin offset for set 1

static void TurnOnLED(void);
static void TurnOffLED(void);

static int __init hello_init(void)
{
    u32 tmp;
    u32 output_mask;

    requestested_mem_region = request_mem_region(GPIO_ADDRESS, GPIO_ADDRESS_LEN, "GPIO47");
    if(requestested_mem_region == NULL)
    {
        pr_info("Info: Requested memory region is also used by GPIO driver!\n");
    }

    virtual_address = ioremap(GPIO_ADDRESS, GPIO_ADDRESS_LEN);
    if(virtual_address == NULL)
    {
        pr_info("Error: Virtual address is NULL!\n");
    }

    //Set GPIO47 to output mode
    tmp = ioread32(virtual_address + GPFSEL4_OFFSET);
    output_mask = 0x1 << (GPIO47_OFFSET*3);
    tmp |= output_mask;

    iowrite32(tmp, virtual_address + GPFSEL4_OFFSET);

    TurnOnLED();
    
    return 0;
}

static void __exit hello_exit(void)
{
    TurnOffLED();
    pr_info("Goodbye\n");
}

static void TurnOnLED(void)
{
    u32 tmp;
    /* Turn on GPIO. */
    tmp = 0x1 << (GREEN_LED_GPIO - 32);
    iowrite32(tmp, virtual_address + GPSET1_OFFSET);
}

static void TurnOffLED(void)
{
    u32 tmp;
    /* Turn off GPIO. */
    tmp = 0x1 << (GREEN_LED_GPIO - 32);
    iowrite32(tmp, virtual_address + GPCLR1_OFFSET);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Group 1");
MODULE_DESCRIPTION("Embedded Linux course - hello module");
MODULE_LICENSE("GPL");


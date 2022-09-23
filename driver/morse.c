#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/delay.h>

#include "ioctl_interface.h"

MODULE_AUTHOR("Group 1");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_LICENSE("GPLv3");


/* MACROS
   ====== */

#define BUFFER_SIZE 50
#define BUFFER_SIZE_MORSE 1000
#define DEVICE_NAME "morse"

/* LED handling macros */

#define GREEN_LED_GPIO      (47)            // GPIO pin of green LED
#define GPIO_ADDRESS        (0x3F200000)    // Start of GPIO address space
#define GPIO_ADDRESS_LEN    (0xB4)          // Length of GPIO address space (0x3F2000B4 - 0x3F200000)
#define GPFSEL4_OFFSET      (0x00000010)    // GPIO pin of greed LED is 47 and so we need to use GPFSEL4
#define GPIO47_OFFSET       (0x00000007)    // In GPFSEL4, the offset of GPIO47 is 7 (47-40)
#define GPIO_DIRECTION_OUT  (1)             // Output direction is 1 (input is 0)
#define GPSET1_OFFSET       (0x00000020)    // RPi has 54 GPIOs which are devided in two sets, GPIO47 is in set 1 (from 32 to 53)
#define GPCLR1_OFFSET       (0x0000002C)    // Clear pin offset for set 1


/* FUNCTION DECLARATIONS
   ===================== */

int __init init_dev(void);
void __exit exit_dev(void);

static int open_device(struct inode*, struct file*);
static int close_device(struct inode*, struct file*);
static ssize_t read_from_device(struct file*, char*, size_t, loff_t*);
static ssize_t write_to_device(struct file*, const char*, size_t, loff_t*);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static void encode(void);
static void char2morse(char, int*);

static void initialize_led_memory(void);
static void set_led_output_mode(void);
static void turn_on_led(void);
static void turn_off_led(void);
static void signal_morse_code(void);


/* GLOBALS
   ======= */

struct cdev *character_device;
static char device_buffer[BUFFER_SIZE];
static char device_buffer_morse[BUFFER_SIZE_MORSE];
dev_t device_number;

enum mode current_mode = MODE_NORMAL;

void __iomem   *virtual_address;
struct resource* requestested_mem_region;

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_device,
	.release = close_device,
	.write = write_to_device,
	.read = read_from_device,
	.unlocked_ioctl = etx_ioctl
};

static void inject_error(void)
{
        if(strlen(device_buffer_morse) < 1)
        return;

        if (device_buffer_morse[0] == '.'){
                device_buffer_morse[0] = '-';
        }
	else {
                device_buffer_morse[0] = '.';
	}
}


static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&current_mode ,(int32_t *) arg, sizeof(current_mode)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
						pr_info("Value = %d\n", current_mode);
                        // TODO: here we can handle the received test_error_data
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &current_mode, sizeof(current_mode)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}


/* FUNCTION DEFINITIONS
   ==================== */


static int open_device(struct inode *pinode, struct file *fp) {
	pr_info("[%s] Device is opened.\n", __func__);
	return 0;
}


static int close_device(struct inode *pinode, struct file *fp) {
	pr_info("[%s] Device is closed.\n", __func__);
	return 0;
}


static ssize_t read_from_device(
	struct file *fp,
	char *buf,
	size_t buf_size,
	loff_t *ppos) {

	int max_bytes;
	ssize_t bytes_read, bytes_to_read;

	max_bytes = BUFFER_SIZE - *ppos;

	if(max_bytes > buf_size)
		bytes_to_read = buf_size;
	else
		bytes_to_read = max_bytes;

	if(bytes_to_read == 0)
		pr_info("[%s] Reached the end of the device.\n", __func__);

	bytes_read = bytes_to_read - copy_to_user(buf, device_buffer + *ppos, bytes_to_read);
	*ppos += bytes_read;

	pr_info("[%s] Bytes has been read successfully.\n", __func__);

	return bytes_read;
}


static ssize_t write_to_device(
	struct file *fp,
	const char *buf,
	size_t buf_size,
	loff_t *ppos) {

	ssize_t bytes_written;
	int i;

	bytes_written = 0;
	*ppos = 0;

	// Clear buffer
	for(i = 0; i < BUFFER_SIZE; i++)
		device_buffer[i] = (char)0;

	for(i = 0; i < BUFFER_SIZE_MORSE; i++)
		device_buffer_morse[i] = (char)0;

	if(buf_size > BUFFER_SIZE)
		pr_info(
			"[%s] Buffer size too large (%lu), max size: %d\n",
			__func__,
			buf_size,
			BUFFER_SIZE);
	else {
		bytes_written = buf_size - copy_from_user(device_buffer, buf, buf_size);
		*ppos += bytes_written;
		pr_info("[%s] Bytes written successfully.\n", __func__);

		encode();
		signal_morse_code();
	}

	return bytes_written;
}


static void encode(void) {
	int i;
	int pos;

	pos = 0;

	for(i = 0; device_buffer[i] != '\0'; i++) {
		if(device_buffer[i] != ' ')
			char2morse(device_buffer[i], &pos);

		if(device_buffer[i + 1] != '\0' && device_buffer[i + 1] != ' ') {
			if(device_buffer[i] == ' '){
				device_buffer_morse[pos] = '_';
			}
				
			else {
				device_buffer_morse[pos] = '*';
			}
				
			pos++;
		}
	}

	/* Set null-terminator at the end of a string*/
	device_buffer_morse[pos]= '\0';

	pr_info("Received string: %s\n", device_buffer);
	pr_info("Encoded string: %s\n", device_buffer_morse);
}


static void char2morse(char c, int *offset) {
	int j;
	j = *offset;

	switch(c) {
		case 'A':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'B':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case 'C':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'D':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case 'E':
		device_buffer_morse[j]='.';
		break;

		case 'F':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'G':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'H':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		 case 'I':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case 'J':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case 'K':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'L':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case 'M':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case 'N':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'O':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case 'P':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'Q':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'R':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;

		case 'S':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case 'T':
		device_buffer_morse[j]='-';
		break;

		case 'U':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'V':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'W':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case 'X':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case 'Y':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case 'Z':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case '0':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case '1':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case '2':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case '3':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='-';
		break;

		case '4':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='-';
		break;

		case '5':
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case '6':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case '7':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case '8':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='.';
		device_buffer_morse[j]='.';
		break;

		case '9':
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j++]='-';
		device_buffer_morse[j]='.';
		break;;

		default:
			pr_info("[%s] Invalid character %c\n", __func__, c);
			break;
	}

	*offset = j + 1;
}

static void initialize_led_memory(void) {
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

}

static void set_led_output_mode(void){
	u32 tmp;
    u32 output_mask;

	tmp = ioread32(virtual_address + GPFSEL4_OFFSET);
    output_mask = 0x1 << (GPIO47_OFFSET*3);
    tmp |= output_mask;

    iowrite32(tmp, virtual_address + GPFSEL4_OFFSET);
}

static void turn_on_led(void){
    u32 tmp;
    /* Turn on GPIO. */
    tmp = 0x1 << (GREEN_LED_GPIO - 32);
    iowrite32(tmp, virtual_address + GPSET1_OFFSET);
}

static void turn_off_led(void){
    u32 tmp;
    /* Turn off GPIO. */
    tmp = 0x1 << (GREEN_LED_GPIO - 32);
    iowrite32(tmp, virtual_address + GPCLR1_OFFSET);
}

static void signal_morse_code(void){
	int i;
	for(i = 0; device_buffer_morse[i] != '\0'; i++) {
		if (device_buffer_morse[i] == '.'){
			turn_on_led();
			msleep(300);
			if (device_buffer_morse[i+1] == '.' || device_buffer_morse[i+1] == '-'){
				turn_off_led();
				msleep(300);
			}
		}
		else if (device_buffer_morse[i] == '-'){
			turn_on_led();
			msleep(900);
			if (device_buffer_morse[i+1] == '.' || device_buffer_morse[i+1] == '-'){
				turn_off_led();
				msleep(300);
			}
		}
		else if (device_buffer_morse[i] == '*'){
			turn_off_led();
			msleep(900);
		}
		else if (device_buffer_morse[i] == '_'){
			turn_off_led();
			msleep(2100);
		}
	}
}


int __init init_dev(void) {
	int ret;

	// Allocate character device and dynamically assign major number
	ret = alloc_chrdev_region(&device_number, 0, 1, DEVICE_NAME);

	if(ret != 0)
		pr_info("[%s] Failed to allocate major number!\n", __func__);
	else {
		character_device = cdev_alloc();
		character_device->ops = &fops;
		character_device->owner = THIS_MODULE;

		// Add character device to kernel
		ret = cdev_add(character_device, device_number, 1);

		if(ret != 0) {
			pr_info("[%s] Failed to add device to kernel!\n", __func__);
			unregister_chrdev_region(device_number, 1);
		}
		else
			pr_info("[%s] Device added to kernel successfully.\n", __func__);

	}

	initialize_led_memory();
	set_led_output_mode();

    /* ----------LED ------*/

	return ret;
}


void __exit exit_dev(void) {
	cdev_del(character_device);
	unregister_chrdev_region(device_number, 1);
	pr_info("Exiting the device...\n");
}


module_init(init_dev);
module_exit(exit_dev);

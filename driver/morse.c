#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

#include "ioctl_interface.h"

MODULE_AUTHOR("Group 1");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_LICENSE("GPLv3");


/* MACROS
   ====== */

#define BUFFER_SIZE 50
#define BUFFER_SIZE_MORSE 1000
#define DEVICE_NAME "morse"



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


/* GLOBALS
   ======= */

struct cdev *character_device;
static char device_buffer[BUFFER_SIZE];
static char device_buffer_morse[BUFFER_SIZE_MORSE];
dev_t device_number;

test_error_mode_data_t test_error_data;

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_device,
	.release = close_device,
	.write = write_to_device,
	.read = read_from_device,
	.unlocked_ioctl = etx_ioctl
};


static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&test_error_data ,(int32_t *) arg, sizeof(test_error_data)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
						pr_info("Value = %d\n", test_error_data.char_index_to_change);
                        // TODO: here we can handle the received test_error_data
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &test_error_data, sizeof(test_error_data)) )
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

		if(device_buffer[i + 1] != '\0') {
			if(device_buffer[i] == ' ')
				device_buffer_morse[pos] = '_';
			else
				device_buffer_morse[pos] = '*';

			pos++;
		}
	}

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
			pr_info("[%s] Dvice added to kernel successfully.\n", __func__);

	}

	return ret;
}


void __exit exit_dev(void) {
	cdev_del(character_device);
	unregister_chrdev_region(device_number, 1);
	pr_info("Exiting the device...\n");
}


module_init(init_dev);
module_exit(exit_dev);

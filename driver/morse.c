#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Group 1");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_LICENSE("GPLv3");


/* MACROS
   ====== */

#define BUFFER_SIZE 50
#define BUFFER_SIZE_MORSE 250
#define DEVICE_NAME "morse"


/* FUNCTION DECLARATIONS
   ===================== */

int __init init_dev(void);
void __exit exit_dev(void);

static int open_device(struct inode*, struct file*);
static int close_device(struct inode*, struct file*);
static ssize_t read_from_device(struct file*, char*, size_t, loff_t*);
static ssize_t write_to_device(struct file*, const char*, size_t, loff_t*);


/* GLOBALS
   ======= */

struct cdev *character_device;
static char device_buffer[BUFFER_SIZE];
dev_t device_number;

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_device,
	.release = close_device,
	.write = write_to_device,
	.read = read_from_device
};



/* FUNCTION DEFINITIONS
   ==================== */


static int open_device(struct inode *pinode, struct file *fp) {
	return 0;
}


static int close_device(struct inode *pinode, struct file *fp) {
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
	}

	return bytes_written;
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

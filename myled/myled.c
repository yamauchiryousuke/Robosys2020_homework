#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

MODULE_AUTHOR("Ryosuke Yamauchi Ryuichi Ueda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static void wait(const unsigned int count)
{
    volatile unsigned int vcount = count;
    while (vcount--);
	return 1;
}

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	char c;
	int i=0;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

	//printk(KERN_INFO "receive %c\n", c);
	
	if (c == '0'){
		gpio_base[10] = 1<<23;
		gpio_base[10] = 1<<24;
		gpio_base[10] = 1<<25;
	}else if(c == '1'){
		if(i<10){
			while(1){
				gpio_base[7] = 1<<23;
				wait(100000000);
				gpio_base[10] = 1<<23;
				gpio_base[7] = 1<<24;
				wait(100000000);
				gpio_base[10] = 1<<24;
				gpio_base[7] = 1<<25;
				wait(100000000);
				gpio_base[10] = 1<<25;
				if(i==9){
					return 1;
				}else i++;
			}
		}
	}
	return 1;
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xF0, 0x9F, 0x8D, 0xA3, 0x0A};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_ERR "sushi : copy_to_user failed\n");
		return -EFAULT;
	}

	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void)
{
	int retval;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	
	printk(KERN_INFO "%s is loaded. major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev), MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}

	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 led0 = 23;
	const u32 index0 = led0/10;
	const u32 shift0 = (led0%10)*3;
	const u32 mask0 = ~(0x7<<shift0);
	gpio_base[index0] = (gpio_base[index0] & mask0) | (0x1<<shift0);
	const u32 led = 24;
	const u32 index = led/10;
	const u32 shift = (led%10)*3;
	const u32 mask = ~(0x7<<shift);
	gpio_base[index] = (gpio_base[index] & mask) | (0x1<<shift);
	const u32 led1 = 25;
	const u32 index1 = led1/10;
	const u32 shift1 = (led1%10)*3;
	const u32 mask1 = ~(0x7<<shift1);
	gpio_base[index1] = (gpio_base[index1] & mask1) | (0x1<<shift1);


	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);

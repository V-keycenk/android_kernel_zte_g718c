


#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/console.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/nmi.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>			
#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/security.h>
#include <linux/bootmem.h>
#include <linux/syscalls.h>
#include <linux/kexec.h>
#include <linux/kdb.h>
#include <linux/ratelimit.h>
#include <linux/kmsg_dump.h>
#include <linux/syslog.h>
#include <linux/cpu.h>
#include <linux/notifier.h>
#include <asm/uaccess.h>



extern int get_rtc_alarm_status(void);

typedef struct 
{
	unsigned long alarm_triggered;	
 	char value[5];
} zte_power_off_alarm_type;

static zte_power_off_alarm_type zte_power_off_alarm_triggered_value[]=
{
	
    	{0,"FALSE"},
	{1,"TRUE"}	
};

static struct kobject *zte_power_off_alarm_kobj;

int zte_get_power_off_alarm_status(void)
{
	int rtl = 0;
	int status = 0;
	
	printk( "zhangji:Enter zte_get_power_off_alarm_status.\r\n");
	status = get_rtc_alarm_status();
	if (status == 1)
	{
	 	pr_debug( " PM_DEBUG_MXP:RTC ALARM trigger detect.\r\n");
	 	rtl = 1;
	}
	else
	{
		pr_debug( " PM_DEBUG_MXP:NO RTC ALARM trigger detect.\r\n");
		rtl = 0;
	}
	return rtl;
}
static ssize_t zte_power_off_alarm_show(struct kobject *kobj,
				 struct kobj_attribute *attr, char *buf)
{	
	int power_off_alarm_triggered;
	int return_value = 0;
	
	power_off_alarm_triggered = zte_get_power_off_alarm_status();
	switch(power_off_alarm_triggered)
	{
		case 0:
		return_value = sprintf(buf, "%s\n", zte_power_off_alarm_triggered_value[0].value);
		break;

		case 1:
		return_value = sprintf(buf, "%s\n", zte_power_off_alarm_triggered_value[1].value);
		break;

		default:
		return_value = sprintf(buf, "%s\n", zte_power_off_alarm_triggered_value[0].value);
		break;	
	}
         return return_value;
}

static ssize_t zte_power_off_alarm_store(struct kobject *kobj,
				  struct kobj_attribute *attr,
				  const char *buf, size_t n)
{
		return n;
}

static struct kobj_attribute zte_power_off_alarm_attribute =
	__ATTR(power_off_alarm, 0755, zte_power_off_alarm_show, zte_power_off_alarm_store);


static struct attribute *attrs[] = {
	&zte_power_off_alarm_attribute.attr,
	NULL,	
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init zte_power_off_alarm_init(void)
{
	int retval;

	

	 
	printk("zhangji  enter zte_power_off_alarm_init\n");
	zte_power_off_alarm_kobj = kobject_create_and_add("zte_power_off_alarm", NULL);
	if (!zte_power_off_alarm_kobj)
		return -ENOMEM;

	
	retval = sysfs_create_group(zte_power_off_alarm_kobj, &attr_group);
	if (retval)
		kobject_put(zte_power_off_alarm_kobj);

	return retval;	
	
}

static void __exit zte_power_off_alarm_exit(void)
{	
	kobject_put(zte_power_off_alarm_kobj);
}

module_init(zte_power_off_alarm_init);
module_exit(zte_power_off_alarm_exit);



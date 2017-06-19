// http://yurovsky.github.io/2014/11/13/atmel-soc-clock-output/

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/module.h>
#include <linux/clk.h>

// access to PMC register
#include <linux/clk/at91_pmc.h>
#include <linux/io.h>

// cdev MACRO
#include <linux/kdev_t.h>


static struct clk *slowclock;


// CDEV stuff
// -------------
static struct class*  myclk_class  = NULL; ///< The device-driver class struct pointer
static struct device* myclk_device = NULL; ///< The device-driver device struct pointer
#define CLASS_NAME  "myclk_class"
#define DEV_NAME		"myclk_module"	// Device name in /dev


// SYSFS stuff
// ------------------------------------------------------------------------------
static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	//const struct pwm_device *pwm = child_to_pwm_device(child);
	return sprintf(buf, "myclk:%d\n", 42); //pwm_is_enabled(pwm));
}

static ssize_t enable_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	// struct pwm_device *pwm = child_to_pwm_device(child);
	int val, ret;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	switch (val) {
	case 0:
    printk(KERN_INFO "%s called with %d\n", __FUNCTION__, val);
    clk_disable_unprepare(slowclock);
		//pwm_disable(pwm);
		break;
	case 1:
    printk(KERN_INFO "%s called with %d\n", __FUNCTION__, val);
    clk_prepare_enable(slowclock);
		//ret = pwm_enable(pwm);
		break;
	default:
		//ret = -EINVAL;
		break;
	}

	return ret ? : size;
}

static DEVICE_ATTR_RW(enable);

static struct attribute *myclk_attrs[] = {
	&dev_attr_enable.attr,
	NULL,
};
ATTRIBUTE_GROUPS(myclk);
// ------------------------------------------------------------------------------

static int wclock_probe(struct platform_device *pdev)
{
    slowclock = devm_clk_get(&pdev->dev, "slowclock");
    if (IS_ERR(slowclock)) {
        dev_err(&pdev->dev, "Failed to get slowclock\n");
        return PTR_ERR(slowclock);
    }

    // log reg value
    printk(KERN_INFO "AT91_PMC_PCKR:0x%x", at91_pmc_read(AT91_PMC_PCKR(0)));

    // Register the device class
    myclk_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(myclk_class)) {
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(myclk_class);
    }
    printk(KERN_INFO "Hello: device class registered correctly\n");

    // adding sysfs attr
    myclk_class->dev_groups = myclk_groups;
    // TODO IF ERROR
    
    // register the device driver
    myclk_device = device_create(myclk_class, NULL, MKDEV(0, 0), NULL, DEV_NAME);
    //myclk_device = device_create(myclk_class, &pdev->dev, MKDEV(0, 0), NULL, DEV_NAME);
    // TODO IF ERROR

    return 0;
}

static int wclock_remove(struct platform_device *pdev)
{
  printk(KERN_INFO "try to release %s \n", DEV_NAME);
  class_unregister(myclk_class);                          // unregister the device class
  class_destroy(myclk_class);                             // remove the device class
  return 0;
}

static const struct of_device_id of_wclock_match[] = {
    { .compatible = "wclock", },
    {},
};

MODULE_DEVICE_TABLE(of, of_wclock_match);

static struct platform_driver wclock_driver = {
    .probe	= wclock_probe,
    .remove	= wclock_remove,
    .driver	= {
        .name		= "wclock",
        .owner		= THIS_MODULE,
        .of_match_table = of_match_ptr(of_wclock_match),
    },
};

module_platform_driver(wclock_driver);

MODULE_AUTHOR("Andrey Yurovsky");
MODULE_DESCRIPTION("programmable clock dummy driver");
MODULE_LICENSE("GPL");

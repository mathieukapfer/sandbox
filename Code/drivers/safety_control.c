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

// write only register
/*
  PRINT_REG(dev, AT91_PMC_SCER);                \
  PRINT_REG(dev, AT91_PMC_SCDR);                \
*/

#define DUMP_REGS(dev)                          \
  PRINT_REG(dev, AT91_PMC_SCSR);                \
  PRINT_REG(dev, AT91_PMC_PCKR(0));             \

#define PRINT_REG(dev, reg)                                            \
  {                                                                    \
    int offset = reg;                                                  \
    int val = at91_pmc_read(offset);                                   \
    if (dev)                                                           \
      dev_info(dev, "# %s@%x:%x\n", #reg, offset, val);                \
    else                                                               \
      printk(KERN_INFO "# %s@%x:%x\n", #reg, offset, val);             \
  }


// Private data
// -------------
static int is_enable = 0;
static struct clk *slowclock;


// CDEV stuff
// -------------
static struct class*  myclk_class  = NULL; ///< The device-driver class struct pointer
static struct device* myclk_device = NULL; ///< The device-driver device struct pointer
#define CLASS_NAME  "clk-pck"
#define DEV_NAME		"safety_control"	// Device name in /dev

// SYSFS stuff
// ------------------------------------------------------------------------------
static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	//const struct pwm_device *pwm = child_to_pwm_device(child);
	return sprintf(buf, "%d\n", is_enable); //pwm_is_enabled(pwm));
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
    if (is_enable) {
      clk_disable_unprepare(slowclock);
      is_enable = 0;
    }
		//pwm_disable(pwm);
		break;
	case 1:
    printk(KERN_INFO "%s called with %d\n", __FUNCTION__, val);
    if (!is_enable) {
      clk_prepare_enable(slowclock);
      is_enable = 1;
    }
		//ret = pwm_enable(pwm);
		break;
	default:
		ret = -EINVAL;
		break;
	}

  DUMP_REGS(child);

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

    // PCK0 register configuration
    DUMP_REGS(&pdev->dev);
    at91_pmc_write(AT91_PMC_PCKR(0), AT91_PMC_CSS_SLOW | AT91_PMC_PRES_32);
    DUMP_REGS(&pdev->dev);

    // register the device class
    myclk_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(myclk_class)) {
      printk(KERN_ALERT "Failed to create class %s\n", CLASS_NAME );
      return PTR_ERR(myclk_class);
    }
    printk(KERN_INFO "class registered correctly\n");

    // adding sysfs attr
    myclk_class->dev_groups = myclk_groups;

    // register the device driver
    myclk_device = device_create(myclk_class, NULL, MKDEV(0, 0), NULL, DEV_NAME);
    if (IS_ERR(myclk_device)) {
      class_unregister(myclk_class);                          // unregister the device class
      class_destroy(myclk_class);                             // remove the device class
      printk(KERN_ALERT "Failed to create device\n");
      return PTR_ERR(myclk_device);
    }

    return 0;
}

static int wclock_remove(struct platform_device *pdev) {
  if (is_enable) {
    printk(KERN_INFO "%s: disable clock\n", DEV_NAME);
    clk_disable_unprepare(slowclock);
  }
  printk(KERN_INFO "%s: removing ...\n", DEV_NAME);
  device_destroy(myclk_class, MKDEV(0, 0));             // remove the device
  class_unregister(myclk_class);                          // unregister the device class
  class_destroy(myclk_class);                             // remove the device class
  printk(KERN_INFO "%s: removed\n", DEV_NAME);
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

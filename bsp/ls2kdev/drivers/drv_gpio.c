/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 * Copyright (c) 2020, duhuanpeng<548708880@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2017-10-20      ZYH          add mode open drain and input pull down
 */


#include <rtthread.h>
#include <drivers/pin.h>
#include <rthw.h>

struct loongson_gpio {
	rt_uint64_t GPIO0_OEN;
	rt_uint64_t GPIO1_OEN;    /* Reserved */
	rt_uint64_t GPIO0_O;
	rt_uint64_t GPIO1_O;      /* Reserved */
	rt_uint64_t GPIO0_I;
	rt_uint64_t GPIO1_I;      /* Reserved */
	rt_uint64_t GPIO0_INTEN;
	rt_uint64_t GPIO1_INTEN;  /* Reserved */
};



/*

drivers/pin.h:

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                -1


*/


#define RT_USING_PIN
#ifdef RT_USING_PIN

static void loongson_pin_mode(struct rt_device *device, rt_base_t pin, rt_base_t mode)
{
	volatile struct loongson_gpio *gpio;
	rt_uint64_t m;

	gpio = (void *)device->user_data;
	m = (rt_uint64_t)1 << pin;

	rt_kprintf("GPIO: %p\n", gpio);
	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);

	switch(mode){
		case PIN_MODE_OUTPUT:
			rt_kprintf("PIN_MODE_OUTPUT");
			rt_kprintf("%p", &gpio->GPIO0_OEN);
			gpio->GPIO0_OEN &= ~m;
			break;
		case PIN_MODE_INPUT:
			rt_kprintf("PIN_MODE_INPUT");
			gpio->GPIO0_OEN |= m;
			break;
		case PIN_MODE_INPUT_PULLUP:
			rt_kprintf("PIN_MODE_INPUT_PULLUP");
			gpio->GPIO0_OEN |= m;
			break;
		case PIN_MODE_INPUT_PULLDOWN:
			rt_kprintf("PIN_MODE_INPUT_PULLDOWN");
			gpio->GPIO0_OEN |= m;
			break;
		case PIN_MODE_OUTPUT_OD:
			rt_kprintf("PIN_MODE_OUTPUT_OD");
			gpio->GPIO0_OEN &= ~m;
			break;
		default:
			rt_kprintf("WTF?");
	}
	rt_kprintf("\n");

	return;
}

static void loongson_pin_write(struct rt_device *device, rt_base_t pin, rt_base_t value)
{
	struct loongson_gpio *gpio;
	rt_uint64_t m;

	if(pin<0 || pin>=60){
		rt_kprintf("WTF!");
		return;
	}

	gpio = (void *)device->user_data;
	m = (rt_uint64_t)1 << pin;

	if(value)
		gpio->GPIO0_O |= m;
	else
		gpio->GPIO0_O &= ~m;

//	rt_kprintf("priv:[%p], pin:[%d], value[%d]\n", gpio, pin, value);
//	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);
	return;
}
static int loongson_pin_read(struct rt_device *device, rt_base_t pin)
{
	struct loongson_gpio *gpio;
	int rc;

	gpio = (void *)device->user_data;
	rt_uint64_t m;

	m = (rt_uint64_t)1 << pin;


	m  = gpio->GPIO1_I;
	m &= (rt_uint64_t)1 << pin;

	rc = !!m;

//	rt_kprintf("priv:[%p], pin:[%d]\n", gpio, pin);	
//	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);
	return rc;
}

    /* TODO: add GPIO interrupt */
static rt_err_t loongson_pin_attach_irq(struct rt_device *device, rt_int32_t pin, rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
	struct loongson_gpio *gpio;

	gpio = (void *)device->user_data;

	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);
	return RT_EOK;
}
static rt_err_t loongson_pin_detach_irq(struct rt_device *device, rt_int32_t pin)
{
	struct loongson_gpio *gpio;

	gpio = (void *)device->user_data;

	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);
	return RT_EOK;
}
static rt_err_t loongson_pin_irq_enable(struct rt_device *device, rt_base_t pin, rt_uint32_t enabled)
{
	struct loongson_gpio *gpio;

	gpio = (void *)device->user_data;

	rt_kprintf("%s: [%d] %s\n", __FILE__, __LINE__, __func__);
	return RT_EOK;
}



static struct rt_pin_ops loongson_pin_ops =
{
    .pin_mode  = loongson_pin_mode,
    .pin_write = loongson_pin_write,
    .pin_read  = loongson_pin_read,

    /* TODO: add GPIO interrupt */
    .pin_attach_irq = loongson_pin_attach_irq,
    .pin_detach_irq = loongson_pin_detach_irq,
    .pin_irq_enable = loongson_pin_irq_enable,
};


int loongson_pin_init(void)
{
	int rc;
	static struct loongson_gpio *loongson_gpio_priv;

	rt_int64_t *p;
	p = (void *)0xFFFFFFFFbfe10420;
	*p &= 0xFFFFFFFFffff0fff;

	loongson_gpio_priv = (void *)0xffffffffbfe10500;
	rc = rt_device_pin_register("pin", &loongson_pin_ops, loongson_gpio_priv);

	return rc;
}
INIT_BOARD_EXPORT(loongson_pin_init);

#endif /*RT_USING_PIN */

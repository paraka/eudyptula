#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static void __exit task5_exit(void)
{
	pr_debug("Bye!\n");
}

static int __init task5_init(void)
{
	pr_debug("Hello USB!\n");
	return 0;
}

static struct usb_device_id usb_kbd_id_table[] = {
	{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			    USB_INTERFACE_SUBCLASS_BOOT,
			    USB_INTERFACE_PROTOCOL_KEYBOARD)},
	{} /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_kbd_id_table);

module_init(task5_init);
module_exit(task5_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task5");
MODULE_AUTHOR("Sergio Paracuellos");

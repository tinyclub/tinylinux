/*
 * Silicon Motion SM712 frame buffer device
 *
 * Copyright (C) 2006 Silicon Motion Technology Corp.
 * Authors: Ge Wang, gewang@siliconmotion.com
 *	    Boyod boyod.yang@siliconmotion.com.cn
 *
 * Copyright (C) 2009 Lemote, Inc.
 * Author: Wu Zhangjin, wuzhangjin@gmail.com
 *
 * Copyright (C) 2010 Wu Zhangjin, wuzhangjin@gmail.com
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 *
 * - Remove the buggy 2D support for Lynx, 2010/01/06, Wu Zhangjin
 *
 * Version 0.10.26192.21.01
 *	- Add PowerPC/Big endian support
 *	- Add 2D support for Lynx
 *	- Verified on2.6.19.2  Boyod.yang <boyod.yang@siliconmotion.com.cn>
 *
 * Version 0.09.2621.00.01
 *	- Only support Linux Kernel's version 2.6.21.
 *	Boyod.yang  <boyod.yang@siliconmotion.com.cn>
 *
 * Version 0.09
 *	- Only support Linux Kernel's version 2.6.12.
 *	Boyod.yang <boyod.yang@siliconmotion.com.cn>
 */

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/io.h>
#include <linux/fb.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/pm.h>

#include "sm712fb.h"

static void sm712_setmode(struct fb_info *info)
{
#if 0
	int i;

	smtc_mmiowb(0x0, 0x3c6);
	smtc_seqw(0, 0x1);
	smtc_mmiowb(VGAMode.Init_MISC, 0x3c2);

	/* init SEQ register SR00 - SR04 */
	for (i = 0; i < SIZE_SR00_SR04; i++)
		smtc_seqw(i, VGAMode.Init_SR00_SR04[i]);

	/* init SEQ register SR10 - SR24 */
	for (i = 0; i < SIZE_SR10_SR24; i++)
		smtc_seqw(i + 0x10, VGAMode.Init_SR10_SR24[i]);

	/* init SEQ register SR30 - SR75 */
	for (i = 0; i < SIZE_SR30_SR75; i++)
		if (((i + 0x30) != 0x62) \
			&& ((i + 0x30) != 0x6a) \
			&& ((i + 0x30) != 0x6b))
			smtc_seqw(i + 0x30, VGAMode.Init_SR30_SR75[i]);

	/* init SEQ register SR80 - SR93 */
	for (i = 0; i < SIZE_SR80_SR93; i++)
		smtc_seqw(i + 0x80, VGAMode.Init_SR80_SR93[i]);

	/* init SEQ register SRA0 - SRAF */
	for (i = 0; i < SIZE_SRA0_SRAF; i++)
		smtc_seqw(i + 0xa0, VGAMode.Init_SRA0_SRAF[i]);

	/* init Graphic register GR00 - GR08 */
	for (i = 0; i < SIZE_GR00_GR08; i++)
		smtc_grphw(i, VGAMode.Init_GR00_GR08[i]);

	/* init Attribute register AR00 - AR14 */
	for (i = 0; i < SIZE_AR00_AR14; i++)
		smtc_attrw(i, VGAMode.Init_AR00_AR14[i]);

	/* init CRTC register CR00 - CR18 */
	for (i = 0; i < SIZE_CR00_CR18; i++)
		smtc_crtcw(i, VGAMode.Init_CR00_CR18[i]);

	/* init CRTC register CR30 - CR4D */
	for (i = 0; i < SIZE_CR30_CR4D; i++)
		smtc_crtcw(i + 0x30, VGAMode.Init_CR30_CR4D[i]);

	/* init CRTC register CR90 - CRA7 */
	for (i = 0; i < SIZE_CR90_CRA7; i++)
		smtc_crtcw(i + 0x90, VGAMode.Init_CR90_CRA7[i]);

	smtc_mmiowb(0x67, 0x3c2);
#endif
}

static void sm712_setpalette(int regno, unsigned red, unsigned green,
			     unsigned blue, struct fb_info *info)
{
	/* primary display palette. Enable LCD RAM only, 6-bit RAM */
	smtc_seqw(0x66, (smtc_seqr(0x66) & 0xC3) | 0x10);
	smtc_mmiowb(regno, 0x3c8);
	smtc_mmiowb(red >> 10, 0x3c9);
	smtc_mmiowb(green >> 10, 0x3c9);
	smtc_mmiowb(blue >> 10, 0x3c9);
}

static struct fb_var_screeninfo sm712_var = {
	.xres = SCREEN_X_RES,
	.yres = SCREEN_Y_RES,
	.xres_virtual = SCREEN_X_RES,
	.yres_virtual = SCREEN_Y_RES,
	.bits_per_pixel = SCREEN_BPP,
	.red = {11, 5, 0},
	.green = {5, 6, 0},
	.blue = {0, 5, 0},
	.activate = FB_ACTIVATE_NOW,
	.height = -1,
	.width = -1,
	.vmode = FB_VMODE_NONINTERLACED,
	.nonstd = 0,
	/* text mode acceleration */
	.accel_flags = FB_ACCELF_TEXT,
	.vmode = FB_VMODE_NONINTERLACED,
};

static struct fb_fix_screeninfo sm712_fix = {
	.id = "sm712fb",
	.type = FB_TYPE_PACKED_PIXELS,
	.type_aux = 0,
	.xpanstep = 0,
	.ypanstep = 0,
	.accel = FB_ACCEL_SMI_LYNX,
	.visual = FB_VISUAL_TRUECOLOR,
	.line_length = SCREEN_X_RES * 2,
	.accel = FB_ACCEL_SMI_LYNX,
	.smem_len = SM712_VIDEOMEMORYSIZE,
};

/*
 * convert a colour value into a field position
 */

static unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	return ((chan & 0xffff) >> (16 - bf->length)) << bf->offset;
}

static int cfb_blank(int blank_mode, struct fb_info *info)
{
	/* clear DPMS setting */
	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
		/* Screen On: HSync: On, VSync : On */
		smtc_seqw(0x01, (smtc_seqr(0x01) & (~0x20)));
		smtc_seqw(0x6a, 0x16);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x21, (smtc_seqr(0x21) & 0x77));
		smtc_seqw(0x22, (smtc_seqr(0x22) & (~0x30)));
		smtc_seqw(0x23, (smtc_seqr(0x23) & (~0xc0)));
		smtc_seqw(0x24, (smtc_seqr(0x24) | 0x01));
		smtc_seqw(0x31, (smtc_seqr(0x31) | 0x03));
		break;
	case FB_BLANK_NORMAL:
		/* Screen Off: HSync: On, VSync : On   Soft blank */
		smtc_seqw(0x01, (smtc_seqr(0x01) & (~0x20)));
		smtc_seqw(0x6a, 0x16);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x22, (smtc_seqr(0x22) & (~0x30)));
		smtc_seqw(0x23, (smtc_seqr(0x23) & (~0xc0)));
		smtc_seqw(0x24, (smtc_seqr(0x24) | 0x01));
		smtc_seqw(0x31, ((smtc_seqr(0x31) & (~0x07)) | 0x00));
		break;
	case FB_BLANK_VSYNC_SUSPEND:
		/* Screen On: HSync: On, VSync : Off */
		smtc_seqw(0x01, (smtc_seqr(0x01) | 0x20));
		smtc_seqw(0x20, (smtc_seqr(0x20) & (~0xB0)));
		smtc_seqw(0x6a, 0x0c);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x21, (smtc_seqr(0x21) | 0x88));
		smtc_seqw(0x22, ((smtc_seqr(0x22) & (~0x30)) | 0x20));
		smtc_seqw(0x23, ((smtc_seqr(0x23) & (~0xc0)) | 0x20));
		smtc_seqw(0x24, (smtc_seqr(0x24) & (~0x01)));
		smtc_seqw(0x31, ((smtc_seqr(0x31) & (~0x07)) | 0x00));
		smtc_seqw(0x34, (smtc_seqr(0x34) | 0x80));
		break;
	case FB_BLANK_HSYNC_SUSPEND:
		/* Screen On: HSync: Off, VSync : On */
		smtc_seqw(0x01, (smtc_seqr(0x01) | 0x20));
		smtc_seqw(0x20, (smtc_seqr(0x20) & (~0xB0)));
		smtc_seqw(0x6a, 0x0c);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x21, (smtc_seqr(0x21) | 0x88));
		smtc_seqw(0x22, ((smtc_seqr(0x22) & (~0x30)) | 0x10));
		smtc_seqw(0x23, ((smtc_seqr(0x23) & (~0xc0)) | 0xD8));
		smtc_seqw(0x24, (smtc_seqr(0x24) & (~0x01)));
		smtc_seqw(0x31, ((smtc_seqr(0x31) & (~0x07)) | 0x00));
		smtc_seqw(0x34, (smtc_seqr(0x34) | 0x80));
		break;
	case FB_BLANK_POWERDOWN:
		/* Screen On: HSync: Off, VSync : Off */
		smtc_seqw(0x01, (smtc_seqr(0x01) | 0x20));
		smtc_seqw(0x20, (smtc_seqr(0x20) & (~0xB0)));
		smtc_seqw(0x6a, 0x0c);
		smtc_seqw(0x6b, 0x02);
		smtc_seqw(0x21, (smtc_seqr(0x21) | 0x88));
		smtc_seqw(0x22, ((smtc_seqr(0x22) & (~0x30)) | 0x30));
		smtc_seqw(0x23, ((smtc_seqr(0x23) & (~0xc0)) | 0xD8));
		smtc_seqw(0x24, (smtc_seqr(0x24) & (~0x01)));
		smtc_seqw(0x31, ((smtc_seqr(0x31) & (~0x07)) | 0x00));
		smtc_seqw(0x34, (smtc_seqr(0x34) | 0x80));
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int smtc_setcolreg(unsigned regno, unsigned red, unsigned green,
			  unsigned blue, unsigned trans, struct fb_info *info)
{
	u32 val;

	if (regno > 255)
		return 1;

	switch (info->fix.visual) {
	case FB_VISUAL_DIRECTCOLOR:
	case FB_VISUAL_TRUECOLOR:
		/*
		 * 16/32 bit true-colour, use pseuo-palette for 16 base color
		 */
		if (regno < 16) {
			u32 *pal = info->pseudo_palette;
			val = chan_to_field(red, &(info->var.red));
			val |= chan_to_field(green, &(info->var.green));
			val |= chan_to_field(blue, &(info->var.blue));
			pal[regno] = val;
		}
		break;

	case FB_VISUAL_PSEUDOCOLOR:
		/* color depth 8 bit */
		sm712_setpalette(regno, red, green, blue, info);
		break;

	default:
		return 1;	/* unknown type */
	}

	return 0;

}

static struct fb_ops sm712_ops = {
	.owner = THIS_MODULE,
	.fb_setcolreg = smtc_setcolreg,
	.fb_blank = cfb_blank,
	.fb_fillrect = cfb_fillrect,
	.fb_imageblit = cfb_imageblit,
	.fb_copyarea = cfb_copyarea,
};

static void framebuffer_free(struct fb_info *info)
{
	if (info) {
		if (info->cmap.len)
			fb_dealloc_cmap(&info->cmap);
		framebuffer_release(info);
	}
}


/*
 * Unmap in the screen memory
 */
static void smtc_unmap_smem(struct fb_info *info)
{
	if (info && info->screen_base) {
		iounmap(info->screen_base);
		info->screen_base = NULL;
	}
}

/*
 * We need to wake up the LynxEM+, and make sure its in linear memory mode.
 */
static void sm712_init_hw(struct fb_info *info)
{
	char *pVPR = info->screen_base + 0x0040c000;
	u32 screenstride;

	outb_p(0x18, 0x3c4);
	outb_p(0x11, 0x3c5);

	/* set MCLK = 14.31818 * (0x16 / 0x2) */
	smtc_seqw(0x6a, 0x16);
	smtc_seqw(0x6b, 0x02);
	smtc_seqw(0x62, 0x3e);
	/* enable PCI burst */
	smtc_seqw(0x17, 0x20);

	/* set VPR registers */
	writel(0x0, pVPR + 0x0C);
	writel(0x0, pVPR + 0x40);

	/* set data width */
	screenstride = (info->var.xres * info->var.bits_per_pixel) / 64;
	writel(0x00020000, pVPR + 0x0);
	writel(((screenstride + 2) << 16) | screenstride, pVPR + 0x10);

	sm712_setmode(info);
}

char __iomem *smtc_RegBaseAddress;	/* Memory Map IO starting address */
static u32 sm712_colreg[17];

/* Jason (08/13/2009)
 * Original init function changed to probe method to be used by pci_drv
 * process used to detect chips replaced with kernel process in pci_drv
 */
static int __devinit sm712_pci_probe(struct pci_dev *pdev,
				   const struct pci_device_id *ent)
{
	struct fb_info *info = framebuffer_alloc(0, &pdev->dev);
	int err = -ENOMEM;

	pr_info("Silicon Motion display driver " SMTC_LINUX_FB_VERSION "\n");
	if (!info) {
		pr_err("Fail to alloc framebuffer info\n");
		goto failed;
	}

	/* Init fb_info with default value */
	info->flags = FBINFO_FLAG_DEFAULT;
	info->fbops = &sm712_ops;
	info->var = sm712_var;
	info->fix = sm712_fix;
	info->par = NULL;
	info->pseudo_palette = sm712_colreg;

	err = pci_enable_device(pdev);	/* enable SMTC chip */
	if (err)
		return err;

	/* Map address and memory detection */
	info->fix.smem_start = pci_resource_start(pdev, 0);
	info->screen_base = ioremap(info->fix.smem_start, info->fix.smem_len);
	smtc_RegBaseAddress = info->screen_base + 0x00700000;

	/* Init the hardware */
	sm712_init_hw(info);

	err = register_framebuffer(info);
	if (err < 0)
		goto failed;

	pci_set_drvdata(pdev, info);

	pr_info("fb%d: %s frame buffer device\n", info->node, info->fix.id);

	return 0;

failed:
	pr_info("Silicon Motion, Inc.  primary display init fail\n");

	smtc_unmap_smem(info);
	framebuffer_free(info);

	return err;
}


/* Jason (08/11/2009) PCI_DRV wrapper essential structs */
static const struct pci_device_id sm712_pci_table[] = {
	{0x126f, 0x712, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{0,}
};


/* Jason (08/14/2009)
 * do some clean up when the driver module is removed
 */
static void __devexit sm712_pci_remove(struct pci_dev *pdev)
{
	struct fb_info *info = pci_get_drvdata(pdev);

	pci_set_drvdata(pdev, NULL);
	smtc_unmap_smem(info);
	unregister_framebuffer(info);
	framebuffer_free(info);
}

#ifdef CONFIG_PM
/* Jason (08/14/2009)
 * suspend function, called when the suspend event is triggered
 */
static int sm712_suspend(struct pci_dev *pdev, pm_message_t msg)
{
	struct fb_info *info = pci_get_drvdata(pdev);
	int retv;

	/*
	 * set the hw in sleep mode use externel clock and self memory refresh
	 * so that we can turn off internal PLLs later on
	 */
	smtc_seqw(0x20, (smtc_seqr(0x20) | 0xc0));
	smtc_seqw(0x69, (smtc_seqr(0x69) & 0xf7));

	switch (msg.event) {
	case PM_EVENT_FREEZE:
	case PM_EVENT_PRETHAW:
		pdev->dev.power.power_state = msg;
		return 0;
	}

	/* when doing suspend, call fb apis and pci apis */
	if (msg.event == PM_EVENT_SUSPEND) {
		acquire_console_sem();
		fb_set_suspend(info, 1);
		release_console_sem();
		retv = pci_save_state(pdev);
		pci_disable_device(pdev);
		retv = pci_choose_state(pdev, msg);
		retv = pci_set_power_state(pdev, retv);
	}

	pdev->dev.power.power_state = msg;

	/* additionaly turn off all function blocks including internal PLLs */
	smtc_seqw(0x21, 0xff);

	return 0;
}

static int sm712_resume(struct pci_dev *pdev)
{
	struct fb_info *info = pci_get_drvdata(pdev);

	/* when resuming, restore pci data and fb cursor */
	if (pdev->dev.power.power_state.event != PM_EVENT_FREEZE) {
		pci_set_power_state(pdev, PCI_D0);
		pci_restore_state(pdev);
		if (pci_enable_device(pdev))
			return -1;
		pci_set_master(pdev);
	}

	/* reinit hardware */
	sm712_init_hw(info);

	smtc_seqw(0x34, (smtc_seqr(0x34) | 0xc0));
	smtc_seqw(0x33, ((smtc_seqr(0x33) | 0x08) & 0xfb));

	acquire_console_sem();
	fb_set_suspend(info, 0);
	release_console_sem();

	return 0;
}
#endif	/* CONFIG_PM */

/* Jason (08/13/2009)
 * pci_driver struct used to wrap the original driver
 * so that it can be registered into the kernel and
 * the proper method would be called when suspending and resuming
 */
static struct pci_driver sm712_driver = {
	.name = "sm712",
	.id_table = sm712_pci_table,
	.probe = sm712_pci_probe,
	.remove = __devexit_p(sm712_pci_remove),
#ifdef CONFIG_PM
	.suspend = sm712_suspend,
	.resume = sm712_resume,
#endif
};

static int __init sm712_init(void)
{
	return pci_register_driver(&sm712_driver);
}

static void __exit sm712_exit(void)
{
	pci_unregister_driver(&sm712_driver);
}

module_init(sm712_init);
module_exit(sm712_exit);

MODULE_AUTHOR("Siliconmotion");
MODULE_DESCRIPTION("Framebuffer driver for SM712 Card");
MODULE_LICENSE("GPL");

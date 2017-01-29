/*
 * modify_fat_label.c:
 *
 * This program changes fat volume label via ioctl.
 * It uses FAT_IOCTL_SET_VOLUME_LABEL ioctl for this purpose.
 *
 * Copyright (C) 2016 Sergio Paracuellos
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define FAT_IOCTL_SET_VOLUME_LABEL _IOW('r', 0x14, char *)

#define MAX_LABEL_LEN 20

static void usage(char *arg)
{
	printf("Usage: %s <label>\n", arg);
	exit(-1);
}

static int set_label(char *arg)
{
	int fd;
	char label[MAX_LABEL_LEN] = {0};
	const char *mnt_point = "/mnt/foobar";

	int len = (strlen(arg) >= MAX_LABEL_LEN)
			? MAX_LABEL_LEN : strlen(arg);

	strncpy(label, arg, len);
	label[len] = '\0';

	printf("Label to set: %s\n", label);

	fd = open(mnt_point, 0);
	if (fd < 0) {
		printf("Open %s directory failed\n", mnt_point);
		exit(-2);
	}

	return (ioctl(fd, FAT_IOCTL_SET_VOLUME_LABEL, label));
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		usage(argv[0]);

	if (set_label(argv[1]))
		printf("Set label failed\n");
	else
		printf("Label changed successfully\n");

	return 0;
}

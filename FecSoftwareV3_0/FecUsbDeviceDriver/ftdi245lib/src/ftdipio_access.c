/*******************************************************************/ 
/*by Wojciech BIALAS, CERN, Geneva , 2005,2006        **************/
/*havely based on code by  opensource@intra2net.com, Fri Apr 4 2003*/
/*(C) 2003 by Intra2net AG*/   

/* original Intra2net AG description: */
/***************************************************************************
                          ftdi.c  -  description
                             -------------------
    begin                : Fri Apr 4 2003
    copyright            : (C) 2003 by Intra2net AG
    email                : opensource@intra2net.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>     /* getenv */
#include <stdio.h>      /* stderr */
#include <errno.h>


#include <usb.h>

#include <string.h>

#include "ftdipio_access.h"

/* ftdi_init return codes:
   0: all fine
  -1: couldn't allocate read buffer
*/

/* these function are CC of libusb 0.1.7 original functions , as ver. 0.1.6 
does not implement them */

int usb_get_string(usb_dev_handle *dev, int index, int langid, char *buf, size_t buflen)
{
  return usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
                        (USB_DT_STRING << 8) + index, langid, buf, buflen, 1000);
}


int usb_get_string_simple(usb_dev_handle *dev, int index, char *buf, size_t buflen)
{
  char tbuf[256];
  int ret, langid, si, di;
 
  ret = usb_get_string(dev, index, 0, tbuf, sizeof(tbuf));
  if (ret < 0)
    return ret;
 
  if (ret < 4)
    return -EIO;
 
  langid = tbuf[2] | (tbuf[3] << 8);
 
  ret = usb_get_string(dev, index, langid, tbuf, sizeof(tbuf));
  if (ret < 0)
    return ret;
 
  for (di = 0, si = 2; si < ret; si += 2) {
    if (di >= (buflen - 1))
      break;
 
    if (tbuf[si + 1])   /* high byte */
      buf[di++] = '?';
    else
      buf[di++] = tbuf[si];
  }
                                                                                
  buf[di] = 0;
                                                                                
  return di;
}







int ftdi_init(struct ftdi_context *ftdi)
{
    ftdi->usb_dev = NULL;
    ftdi->usb_read_timeout = 5000;
    ftdi->usb_write_timeout = 5000;

    ftdi->type = TYPE_BM;    /* chip type */
    ftdi->baudrate = -1;
    ftdi->bitbang_enabled = 0;

    ftdi->readbuffer = NULL;
    ftdi->readbuffer_offset = 0;
    ftdi->readbuffer_remaining = 0;
    ftdi->writebuffer_chunksize = 4096;

    ftdi->interface = 0;
    ftdi->index = 0;
    ftdi->in_ep = 0x02;
    ftdi->out_ep = 0x81;
    ftdi->bitbang_mode = 1; /* 1: Normal bitbang mode, 2: SPI bitbang mode */

    ftdi->error_str = NULL;

    /* All fine. Now allocate the readbuffer */
    return ftdi_read_data_set_chunksize(ftdi, 4096);
}


void ftdi_deinit(struct ftdi_context *ftdi)
{
    if (ftdi->readbuffer != NULL) {
        free(ftdi->readbuffer);
        ftdi->readbuffer = NULL;
    }
}


void ftdi_set_usbdev (struct ftdi_context *ftdi, usb_dev_handle *usb)
{
    ftdi->usb_dev = usb;
}


/* ftdi_usb_open return codes:
   0: all fine
  -1: usb_find_busses() failed
  -2: usb_find_devices() failed
  -3: usb device not found
  -4: unable to open device
  -5: unable to claim device
  -6: reset failed
  -7: set baudrate failed
  -8: get product description failed
  -9: get serial number failed
  -10: unable to close device
*/
int ftdi_usb_open(struct ftdi_context *ftdi, int vendor, int product)
{
    return ftdi_usb_open_desc(ftdi, vendor, product, NULL, NULL);
}

int ftdi_usb_open_desc(struct ftdi_context *ftdi, int vendor, int product,
                       const char* description, const char* serial)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    usb_init();

    if (usb_find_busses() < 0) {
        ftdi->error_str = "usb_find_busses() failed";
        return -1;
    }

    if (usb_find_devices() < 0) {
        ftdi->error_str = "usb_find_devices() failed";
        return -2;
    }

    for (bus = usb_busses; bus; bus = bus->next) {
      /* printf("bus: %s \n",bus->dirname); */
        for (dev = bus->devices; dev; dev = dev->next) {
	  /* printf("device: %s \n",dev->filename);
	  printf("device vendor: 0x%04X prod: 0x%04X \n",
		 dev->descriptor.idVendor,
		 dev->descriptor.idProduct ) ;*/ 
            if (dev->descriptor.idVendor == vendor
                && dev->descriptor.idProduct == product) {
                if (!(ftdi->usb_dev = usb_open(dev))) {
                    ftdi->error_str = "usb_open() failed";
                    return -4;
                }
                
                char string[256];
                if (description != NULL) {
                    if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iProduct, string, sizeof(string)) <= 0) {
                        ftdi->error_str = "unable to fetch product description\n";
                        if (usb_close (ftdi->usb_dev) != 0)
                            return -10;
                        return -8;
                    }
                    if (strncmp(string, description, sizeof(string)) != 0) {
                        ftdi->error_str = "product description not matching\n";
                        if (usb_close (ftdi->usb_dev) != 0)
                            return -10;
                        continue;
                    }
                }
                if (serial != NULL) {
                    if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iSerialNumber, string, sizeof(string)) <= 0) {
                        ftdi->error_str = "unable to fetch serial number\n";
                        if (usb_close (ftdi->usb_dev) != 0)
                            return -10;
                        return -9;
                    }
                    if (strncmp(string, serial, sizeof(string)) != 0) {
                        ftdi->error_str = "serial number not matching\n";
                        if (usb_close (ftdi->usb_dev) != 0)
                            return -10;
                        continue;
                    }
                }

                if (usb_claim_interface(ftdi->usb_dev, ftdi->interface) != 0) {
                    ftdi->error_str = "unable to claim usb device. Make sure ftdi_sio is unloaded!";
                    if (usb_close (ftdi->usb_dev) != 0)
                        return -10;
                    return -5;
                }

                if (ftdi_usb_reset (ftdi) != 0) {
                    if (usb_close (ftdi->usb_dev) != 0)
                        return -10;
                    return -6;
                }
                    
                if (ftdi_set_baudrate (ftdi, 9600) != 0) {
                    if (usb_close (ftdi->usb_dev) != 0)
                        return -10;
                    return -7;
                }

                // Try to guess chip type
                // Bug in the BM type chips: bcdDevice is 0x200 for serial == 0
                if (dev->descriptor.bcdDevice == 0x400 || (dev->descriptor.bcdDevice == 0x200
                        && dev->descriptor.iSerialNumber == 0))
                    ftdi->type = TYPE_BM;
                else if (dev->descriptor.bcdDevice == 0x200)
                    ftdi->type = TYPE_AM;
                else if (dev->descriptor.bcdDevice == 0x500)
                    ftdi->type = TYPE_2232C;

                return 0;
            }
        }
    }

    // device not found
    return -3;
}


int ftdi_usb_reset(struct ftdi_context *ftdi)
{
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 0, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "FTDI reset failed";
        return -1;
    }
    // Invalidate data in the readbuffer
    ftdi->readbuffer_offset = 0;
    ftdi->readbuffer_remaining = 0;

    return 0;
}

int ftdi_usb_purge_buffers(struct ftdi_context *ftdi)
{
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 1, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "FTDI purge of RX buffer failed";
        return -1;
    }
    // Invalidate data in the readbuffer
    ftdi->readbuffer_offset = 0;
    ftdi->readbuffer_remaining = 0;

    if (usb_control_msg(ftdi->usb_dev, 0x40, 0, 2, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "FTDI purge of TX buffer failed";
        return -1;
    }


    return 0;
}

/* ftdi_usb_close return codes
    0: all fine
   -1: usb_release failed
   -2: usb_close failed
*/
int ftdi_usb_close(struct ftdi_context *ftdi)
{
    int rtn = 0;

    if (usb_release_interface(ftdi->usb_dev, ftdi->interface) != 0)
        rtn = -1;

    if (usb_close (ftdi->usb_dev) != 0)
        rtn = -2;

    return rtn;
}


/*
    ftdi_convert_baudrate returns nearest supported baud rate to that requested.
    Function is only used internally
*/
static int ftdi_convert_baudrate(int baudrate, struct ftdi_context *ftdi,
                                 unsigned short *value, unsigned short *index)
{
    static const char am_adjust_up[8] = {0, 0, 0, 1, 0, 3, 2, 1};
    static const char am_adjust_dn[8] = {0, 0, 0, 1, 0, 1, 2, 3};
    static const char frac_code[8] = {0, 3, 2, 4, 1, 5, 6, 7};
    int divisor, best_divisor, best_baud, best_baud_diff;
    unsigned long encoded_divisor;
    int i;

    if (baudrate <= 0) {
        // Return error
        return -1;
    }

    divisor = 24000000 / baudrate;

    if (ftdi->type == TYPE_AM) {
        // Round down to supported fraction (AM only)
        divisor -= am_adjust_dn[divisor & 7];
    }

    // Try this divisor and the one above it (because division rounds down)
    best_divisor = 0;
    best_baud = 0;
    best_baud_diff = 0;
    for (i = 0; i < 2; i++) {
        int try_divisor = divisor + i;
        int baud_estimate;
        int baud_diff;

        // Round up to supported divisor value
        if (try_divisor < 8) {
            // Round up to minimum supported divisor
            try_divisor = 8;
        } else if (ftdi->type != TYPE_AM && try_divisor < 12) {
            // BM doesn't support divisors 9 through 11 inclusive
            try_divisor = 12;
        } else if (divisor < 16) {
            // AM doesn't support divisors 9 through 15 inclusive
            try_divisor = 16;
        } else {
            if (ftdi->type == TYPE_AM) {
                // Round up to supported fraction (AM only)
                try_divisor += am_adjust_up[try_divisor & 7];
                if (try_divisor > 0x1FFF8) {
                    // Round down to maximum supported divisor value (for AM)
                    try_divisor = 0x1FFF8;
                }
            } else {
                if (try_divisor > 0x1FFFF) {
                    // Round down to maximum supported divisor value (for BM)
                    try_divisor = 0x1FFFF;
                }
            }
        }
        // Get estimated baud rate (to nearest integer)
        baud_estimate = (24000000 + (try_divisor / 2)) / try_divisor;
        // Get absolute difference from requested baud rate
        if (baud_estimate < baudrate) {
            baud_diff = baudrate - baud_estimate;
        } else {
            baud_diff = baud_estimate - baudrate;
        }
        if (i == 0 || baud_diff < best_baud_diff) {
            // Closest to requested baud rate so far
            best_divisor = try_divisor;
            best_baud = baud_estimate;
            best_baud_diff = baud_diff;
            if (baud_diff == 0) {
                // Spot on! No point trying
                break;
            }
        }
    }
    // Encode the best divisor value
    encoded_divisor = (best_divisor >> 3) | (frac_code[best_divisor & 7] << 14);
    // Deal with special cases for encoded value
    if (encoded_divisor == 1) {
        encoded_divisor = 0;	// 3000000 baud
    } else if (encoded_divisor == 0x4001) {
        encoded_divisor = 1;	// 2000000 baud (BM only)
    }
    // Split into "value" and "index" values
    *value = (unsigned short)(encoded_divisor & 0xFFFF);
    if(ftdi->type == TYPE_2232C) {
        *index = (unsigned short)(encoded_divisor >> 8);
        *index &= 0xFF00;
        *index |= ftdi->interface;
    }
    else
        *index = (unsigned short)(encoded_divisor >> 16);
    
    // Return the nearest baud rate
    return best_baud;
}

/*
    ftdi_set_baudrate return codes:
     0: all fine
    -1: invalid baudrate
    -2: setting baudrate failed
*/
int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate)
{
    unsigned short value, index;
    int actual_baudrate;

    if (ftdi->bitbang_enabled) {
        baudrate = baudrate*4;
    }

    actual_baudrate = ftdi_convert_baudrate(baudrate, ftdi, &value, &index);
    if (actual_baudrate <= 0) {
        ftdi->error_str = "Silly baudrate <= 0.";
        return -1;
    }

    // Check within tolerance (about 5%)
    if ((actual_baudrate * 2 < baudrate /* Catch overflows */ )
            || ((actual_baudrate < baudrate)
                ? (actual_baudrate * 21 < baudrate * 20)
                : (baudrate * 21 < actual_baudrate * 20))) {
        ftdi->error_str = "Unsupported baudrate. Note: bitbang baudrates are automatically multiplied by 4";
        return -1;
    }

    if (usb_control_msg(ftdi->usb_dev, 0x40, 3, value, index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "Setting new baudrate failed";
        return -2;
    }

    ftdi->baudrate = baudrate;
    return 0;
}


int ftdi_write_data(struct ftdi_context *ftdi, unsigned char *buf, int size)
{
    int ret;
    int offset = 0;
    int total_written = 0;
    while (offset < size) {
        int write_size = ftdi->writebuffer_chunksize;

        if (offset+write_size > size)
            write_size = size-offset;

        ret = usb_bulk_write(ftdi->usb_dev, ftdi->in_ep, buf+offset, write_size, ftdi->usb_write_timeout);
	/*printf("size: %d , wrote bytes: %d\n", size, ret);*/
        if (ret < 0) {
            if (ret == -1)
                ftdi->error_str = "bulk write failed";
            else
                ftdi->error_str = "usb failed";
            return ret;
        }
        total_written += ret;

        offset += write_size;
    }

    return total_written;
}


int ftdi_write_data_set_chunksize(struct ftdi_context *ftdi, unsigned int chunksize)
{
    ftdi->writebuffer_chunksize = chunksize;
    return 0;
}


int ftdi_write_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize)
{
    *chunksize = ftdi->writebuffer_chunksize;
    return 0;
}


int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size)
{
    int offset = 0, ret = 1, i, num_of_chunks, chunk_remains;

    // everything we want is still in the readbuffer?
    if (size <= ftdi->readbuffer_remaining) {
        memcpy (buf, ftdi->readbuffer+ftdi->readbuffer_offset, size);

        // Fix offsets
        ftdi->readbuffer_remaining -= size;
        ftdi->readbuffer_offset += size;

        //printf("Returning bytes from buffer: %d - remaining: %d\n", size, ftdi->readbuffer_remaining); 

        return size;
    }
    // something still in the readbuffer, but not enough to satisfy 'size'?
    if (ftdi->readbuffer_remaining != 0) {
        memcpy (buf, ftdi->readbuffer+ftdi->readbuffer_offset, ftdi->readbuffer_remaining);

        // Fix offset
        offset += ftdi->readbuffer_remaining;
    }
    // do the actual USB read
    while (offset < size && ret > 0) {
        ftdi->readbuffer_remaining = 0;
        ftdi->readbuffer_offset = 0;
        /* returns how much received */
        ret = usb_bulk_read (ftdi->usb_dev, ftdi->out_ep, ftdi->readbuffer, ftdi->readbuffer_chunksize, ftdi->usb_read_timeout);

        if (ret < 0) {
            if (ret == -1)
                ftdi->error_str = "bulk read failed";
            else
                ftdi->error_str = "usb failed";
            return ret;
        }

        if (ret > 2) {
            // skip FTDI status bytes.
            // Maybe stored in the future to enable modem use
            num_of_chunks = ret / 64;
            chunk_remains = ret % 64;
            /*printf("ret = %X, num_of_chunks = %X, chunk_remains = %X, readbuffer_offset = %X\n", ret, num_of_chunks, chunk_remains, ftdi->readbuffer_offset); */

            ftdi->readbuffer_offset += 2;
            ret -= 2;

            if (ret > 64) {
                for (i = 1; i < num_of_chunks; i++)
                    memmove (ftdi->readbuffer+ftdi->readbuffer_offset+62*i,
                             ftdi->readbuffer+ftdi->readbuffer_offset+64*i,
                             62);
                if (chunk_remains > 2) {
                    memmove (ftdi->readbuffer+ftdi->readbuffer_offset+62*i,
                             ftdi->readbuffer+ftdi->readbuffer_offset+64*i,
                             chunk_remains-2);
                    ret -= 2*num_of_chunks;
                } else
                    ret -= 2*(num_of_chunks-1)+chunk_remains;
            }
        } else if (ret <= 2) {
            // no more data to read?
           return offset;
        }
        if (ret > 0) {
            // data still fits in buf?
            if (offset+ret <= size) {
                memcpy (buf+offset, ftdi->readbuffer+ftdi->readbuffer_offset, ret);
                /* printf("buf[0] = %X, buf[1] = %X\n", buf[0], buf[1]);*/
                offset += ret;

                /* Did we read exactly the right amount of bytes? */
                if (offset == size)
                    return offset;
            } else {
                // only copy part of the data or size <= readbuffer_chunksize
                int part_size = size-offset;
                memcpy (buf+offset, ftdi->readbuffer+ftdi->readbuffer_offset, part_size);

                ftdi->readbuffer_offset += part_size;
                ftdi->readbuffer_remaining = ret-part_size;
                offset += part_size;

                /* printf("Returning part: %d - size: %d - offset: %d - ret: %d - remaining: %d\n",
		   part_size, size, offset, ret, ftdi->readbuffer_remaining); */

                return offset;
            }
        }
    }
    // never reached
    return -127;
}


int ftdi_read_data_set_chunksize(struct ftdi_context *ftdi, unsigned int chunksize)
{
    unsigned char *new_buf;

    // Invalidate all remaining data
    ftdi->readbuffer_offset = 0;
    ftdi->readbuffer_remaining = 0;

    if ((new_buf = (unsigned char *)realloc(ftdi->readbuffer, chunksize)) == NULL) {
        ftdi->error_str = "out of memory for readbuffer";
        return -1;
    }

    ftdi->readbuffer = new_buf;
    ftdi->readbuffer_chunksize = chunksize;

    return 0;
}


int ftdi_read_data_get_chunksize(struct ftdi_context *ftdi, unsigned int *chunksize)
{
    *chunksize = ftdi->readbuffer_chunksize;
    return 0;
}



int ftdi_enable_bitbang(struct ftdi_context *ftdi, unsigned char bitmask)
{
    unsigned short usb_val;

    usb_val = bitmask; // low byte: bitmask
    /* FT2232C: Set bitbang_mode to 2 to enable SPI */
    usb_val |= (ftdi->bitbang_mode << 8);

    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x0B, usb_val, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "Unable to enter bitbang mode. Perhaps not a BM type chip?";
        return -1;
    }
    ftdi->bitbang_enabled = 1;
    return 0;
}


int ftdi_disable_bitbang(struct ftdi_context *ftdi)
{
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x0B, 0, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "Unable to leave bitbang mode. Perhaps not a BM type chip?";
        return -1;
    }

    ftdi->bitbang_enabled = 0;
    return 0;
}


int ftdi_read_pins(struct ftdi_context *ftdi, unsigned char *pins)
{
    unsigned short usb_val;
    if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x0C, 0, ftdi->index, (char *)&usb_val, 1, ftdi->usb_read_timeout) != 1) {
        ftdi->error_str = "Read pins failed";
        return -1;
    }

    *pins = (unsigned char)usb_val;
    return 0;
}


int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency)
{
    unsigned short usb_val;

    if (latency < 1) {
        ftdi->error_str = "Latency out of range. Only valid for 1-255";
        return -1;
    }

    usb_val = latency;
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x09, usb_val, ftdi->index, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "Unable to set latency timer";
        return -2;
    }
    return 0;
}


int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency)
{
    unsigned short usb_val;
    if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x0A, 0, ftdi->index, (char *)&usb_val, 1, ftdi->usb_read_timeout) != 1) {
        ftdi->error_str = "Reading latency timer failed";
        return -1;
    }

    *latency = (unsigned char)usb_val;
    return 0;
}


void ftdi_eeprom_initdefaults(struct ftdi_eeprom *eeprom)
{
    eeprom->vendor_id = 0x0403;
    eeprom->product_id = 0x6001;

    eeprom->self_powered = 1;
    eeprom->remote_wakeup = 1;
    eeprom->BM_type_chip = 1;

    eeprom->in_is_isochronous = 0;
    eeprom->out_is_isochronous = 0;
    eeprom->suspend_pull_downs = 0;

    eeprom->use_serial = 0;
    eeprom->change_usb_version = 0;
    eeprom->usb_version = 0x0400;
    eeprom->max_power = 0;

    eeprom->manufacturer = NULL;
    eeprom->product = NULL;
    eeprom->serial = NULL;
}


/*
    ftdi_eeprom_build return codes:
    positive value: used eeprom size
    -1: eeprom size (128 bytes) exceeded by custom strings
*/
int ftdi_eeprom_build(struct ftdi_eeprom *eeprom, unsigned char *output)
{
    unsigned char i, j;
    unsigned short checksum, value;
    unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
    int size_check;

    if (eeprom->manufacturer != NULL)
        manufacturer_size = strlen(eeprom->manufacturer);
    if (eeprom->product != NULL)
        product_size = strlen(eeprom->product);
    if (eeprom->serial != NULL)
        serial_size = strlen(eeprom->serial);

    size_check = 128; // eeprom is 128 bytes
    size_check -= 28; // 28 are always in use (fixed)
    size_check -= manufacturer_size*2;
    size_check -= product_size*2;
    size_check -= serial_size*2;

    // eeprom size exceeded?
    if (size_check < 0)
        return (-1);

    // empty eeprom
    memset (output, 0, 128);

    // Addr 00: Stay 00 00
    // Addr 02: Vendor ID
    output[0x02] = eeprom->vendor_id;
    output[0x03] = eeprom->vendor_id >> 8;

    // Addr 04: Product ID
    output[0x04] = eeprom->product_id;
    output[0x05] = eeprom->product_id >> 8;

    // Addr 06: Device release number (0400h for BM features)
    output[0x06] = 0x00;

    if (eeprom->BM_type_chip == 1)
        output[0x07] = 0x04;
    else
        output[0x07] = 0x02;

    // Addr 08: Config descriptor
    // Bit 1: remote wakeup if 1
    // Bit 0: self powered if 1
    //
    j = 0;
    if (eeprom->self_powered == 1)
        j = j | 1;
    if (eeprom->remote_wakeup == 1)
        j = j | 2;
    output[0x08] = j;

    // Addr 09: Max power consumption: max power = value * 2 mA
    output[0x09] = eeprom->max_power;
    ;

    // Addr 0A: Chip configuration
    // Bit 7: 0 - reserved
    // Bit 6: 0 - reserved
    // Bit 5: 0 - reserved
    // Bit 4: 1 - Change USB version
    // Bit 3: 1 - Use the serial number string
    // Bit 2: 1 - Enable suspend pull downs for lower power
    // Bit 1: 1 - Out EndPoint is Isochronous
    // Bit 0: 1 - In EndPoint is Isochronous
    //
    j = 0;
    if (eeprom->in_is_isochronous == 1)
        j = j | 1;
    if (eeprom->out_is_isochronous == 1)
        j = j | 2;
    if (eeprom->suspend_pull_downs == 1)
        j = j | 4;
    if (eeprom->use_serial == 1)
        j = j | 8;
    if (eeprom->change_usb_version == 1)
        j = j | 16;
    output[0x0A] = j;

    // Addr 0B: reserved
    output[0x0B] = 0x00;

    // Addr 0C: USB version low byte when 0x0A bit 4 is set
    // Addr 0D: USB version high byte when 0x0A bit 4 is set
    if (eeprom->change_usb_version == 1) {
        output[0x0C] = eeprom->usb_version;
        output[0x0D] = eeprom->usb_version >> 8;
    }


    // Addr 0E: Offset of the manufacturer string + 0x80
    output[0x0E] = 0x14 + 0x80;

    // Addr 0F: Length of manufacturer string
    output[0x0F] = manufacturer_size*2 + 2;

    // Addr 10: Offset of the product string + 0x80, calculated later
    // Addr 11: Length of product string
    output[0x11] = product_size*2 + 2;

    // Addr 12: Offset of the serial string + 0x80, calculated later
    // Addr 13: Length of serial string
    output[0x13] = serial_size*2 + 2;

    // Dynamic content
    output[0x14] = manufacturer_size*2 + 2;
    output[0x15] = 0x03; // type: string

    i = 0x16, j = 0;

    // Output manufacturer
    for (j = 0; j < manufacturer_size; j++) {
        output[i] = eeprom->manufacturer[j], i++;
        output[i] = 0x00, i++;
    }

    // Output product name
    output[0x10] = i + 0x80;  // calculate offset
    output[i] = product_size*2 + 2, i++;
    output[i] = 0x03, i++;
    for (j = 0; j < product_size; j++) {
        output[i] = eeprom->product[j], i++;
        output[i] = 0x00, i++;
    }

    // Output serial
    output[0x12] = i + 0x80; // calculate offset
    output[i] = serial_size*2 + 2, i++;
    output[i] = 0x03, i++;
    for (j = 0; j < serial_size; j++) {
        output[i] = eeprom->serial[j], i++;
        output[i] = 0x00, i++;
    }

    // calculate checksum
    checksum = 0xAAAA;

    for (i = 0; i < 63; i++) {
        value = output[i*2];
        value += output[(i*2)+1] << 8;

        checksum = value^checksum;
        checksum = (checksum << 1) | (checksum >> 15);
    }

    output[0x7E] = checksum;
    output[0x7F] = checksum >> 8;

    return size_check;
}


int ftdi_read_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom)
{
    int i;

    for (i = 0; i < 64; i++) {
        if (usb_control_msg(ftdi->usb_dev, 0xC0, 0x90, 0, i, eeprom+(i*2), 2, ftdi->usb_read_timeout) != 2) {
            ftdi->error_str = "Reading eeprom failed";
            return -1;
        }
    }

    return 0;
}


int ftdi_write_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom)
{
    unsigned short usb_val;
    int i;

    for (i = 0; i < 64; i++) {
        usb_val = eeprom[i*2];
        usb_val += eeprom[(i*2)+1] << 8;
        if (usb_control_msg(ftdi->usb_dev, 0x40, 0x91, usb_val, i, NULL, 0, ftdi->usb_write_timeout) != 0) {
            ftdi->error_str = "Unable to write eeprom";
            return -1;
        }
    }

    return 0;
}


int ftdi_erase_eeprom(struct ftdi_context *ftdi)
{
    if (usb_control_msg(ftdi->usb_dev, 0x40, 0x92, 0, 0, NULL, 0, ftdi->usb_write_timeout) != 0) {
        ftdi->error_str = "Unable to erase eeprom";
        return -1;
    }

    return 0;
}

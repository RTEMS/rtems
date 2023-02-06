#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/bspIo.h>

#include <bsp/flexbus.h>
#include <bsp/psram.h>

typedef enum
{
    COPY_SRC_BIT7_0_TO_DST_BIT7_0,
    COPY_SRC_BIT7_0_TO_DST_BIT15_8,
    COPY_SRC_BIT15_8_TO_DST_BIT7_0,
    COPY_SRC_BIT15_8_TO_DST_BIT15_8
}protect_option_e;

enum
{
    DESTINATION_BIT7_0,
    DESTINATION_BIT15_8,
    SOURCE_BIT7_0,
    SOURCE_BIT15_8,
};

static unsigned short psram_word[2] = {0xFF, 0xFF};
static char libc_buf[32] = {0xFF};

unsigned short malloc_buf[1024 * 64] RTEMS_SECTION( ".psram" );
static unsigned short malloc_index = 0;
#define OUT_OF_MALLOC_RANGE(addr) ((unsigned int)addr < (unsigned int)malloc_buf || \
                               (unsigned int)addr > ((unsigned int)malloc_buf + sizeof(malloc_buf)))
#define GET_MALLOC_LENGH(addr) *((size_t *)((intptr_t)addr - 2))

static char *kinetis_memcpy_to_psram(char *dst, const char *src, size_t len);
static char *kinetis_memcpy_from_psram(char *dst, const char *src, size_t len);
static char *kinetis_memcpy_within_psram(char *dst, const char *src, size_t len);
static void transfer_data_per_16bits(unsigned short *addr, unsigned short *data, size_t len);
static void set_data_per_16bits(unsigned short *addr, unsigned short data, size_t words);
static void copy_byte_of_16bits_safely(unsigned short *dst, unsigned short *src, protect_option_e opt);
static void transfer_bytes_between_odd_and_even_address(char *dst, const char *src, size_t len);

void kinetis_psram_clear(void)
{
    for (int i = 0;i < sizeof(malloc_buf);i++)
    {
        malloc_buf[i] = 0;
    }
}

char *kinetis_memcpy(char *dst, const char *src, size_t len)
{
    if (BOTH_IN_PSRAM_RANGE(dst, src)) 
    {
        return kinetis_memcpy_within_psram(dst, src, len);
    }
    else if (IS_PSRAM_ADDRESS(dst))
    {
        return kinetis_memcpy_to_psram(dst, src, len);
    }
    else if (IS_PSRAM_ADDRESS(src))
    {
        return kinetis_memcpy_from_psram(dst, src, len);
    }
    else
    {
        return memcpy(dst, src, len);
    }
}


int kinetis_snprintf (char *str, size_t len, const char *fmt, ...)
{
    va_list ap;
    int ret = 0;

    if (IS_PSRAM_ADDRESS(str))
    {
        if (len > sizeof(libc_buf))
        {
            printk("Length %d out of range: %d.\n", len, sizeof(libc_buf));
            return -1;
        }
        else
        {
            va_start(ap, fmt);
            ret = snprintf(libc_buf, len, fmt, va_arg(ap, char *), va_arg(ap, char *));
            va_end(ap);
            kinetis_memcpy_to_psram(str, libc_buf, len);
            return ret;
        }
    }
    else
    {
        va_start(ap, fmt);
        ret = snprintf(str, len, fmt, va_arg(ap, char *), va_arg(ap, char *));
        va_end(ap);
        return ret;
    }
}

char *kinetis_malloc(size_t len)
{
    if (len > (sizeof(malloc_buf) - malloc_index))
    {
        printk("kinetis malloc failed!\n");
        return NULL; 
    }
    size_t length = GET_ALIGNED_LENGTH(len);
    unsigned short index = malloc_index;

    // reserved first 4 bytes to record malloc length
    malloc_buf[index] = length;
    malloc_index = index + (sizeof(length) + len) / sizeof(unsigned short);

    return (char *)(&malloc_buf[index]) + sizeof(length); // start from : latest_address + sizeof(length)
}

void kinetis_free(void *ptr)
{
    if (OUT_OF_MALLOC_RANGE(ptr))
    {
        printk("kinetis free failed!\n");
        return;
    }

    size_t len = GET_MALLOC_LENGH(ptr);
    if (((uintptr_t)ptr + len) < ((uintptr_t)malloc_buf + malloc_index))
    {
        set_data_per_16bits((unsigned short *)ptr, 0xdead, len / sizeof(unsigned short));
    }
    else
    {
        malloc_index -= (sizeof(len) + len) / sizeof(unsigned short);

free_previous_block:
        if (((unsigned short *)ptr)[malloc_index] == 0xdead)
        {
            len = sizeof(unsigned short);
            while (((unsigned short *)ptr)[--malloc_index] == 0xdead)
            {
                len += sizeof(unsigned short);
            }
            malloc_index -= (sizeof(len) + len) / sizeof(unsigned short);
            goto free_previous_block;
        }
    }
}

static char *kinetis_memcpy_to_psram(char *dst, const char *src, size_t len)
{
    int cycles = len / 2;
    int remained = len % 2;
    unsigned short *dst_word = (unsigned short *)dst;
    unsigned short *src_word = (unsigned short *)src;

    if (IS_ODD_ADDRESS(dst))
    {
        dst_word = GET_ALIGNED_ADDRESS(dst);
        copy_byte_of_16bits_safely(dst_word, src_word, COPY_SRC_BIT7_0_TO_DST_BIT15_8);

        dst_word = dst_word + 1;
        src_word = (unsigned short *)(src + 1);
        cycles = (len - 1) / 2;
        remained = (len - 1) % 2;
    }

    transfer_data_per_16bits(dst_word, src_word, cycles);

    if (remained)
    {
        copy_byte_of_16bits_safely(&dst_word[cycles], &src_word[cycles], COPY_SRC_BIT7_0_TO_DST_BIT7_0);
    }

    return dst;
}

static char *kinetis_memcpy_from_psram(char *dst, const char *src, size_t len)
{
    int cycles = len / 2;
    int remained = len % 2;
    unsigned short *dst_word = (unsigned short *)dst;
    unsigned short *src_word = (unsigned short *)src;

    if (IS_ODD_ADDRESS(src))
    {
        src_word = GET_ALIGNED_ADDRESS(src);
        copy_byte_of_16bits_safely(dst_word, src_word, COPY_SRC_BIT15_8_TO_DST_BIT7_0);
        
        dst_word = (unsigned short *)(dst + 1);
        src_word = src_word + 1;
        cycles = (len - 1) / 2;
        remained = (len - 1) % 2;
    }

    transfer_data_per_16bits(dst_word, src_word, cycles);

    if (remained)
    {
        copy_byte_of_16bits_safely(&dst_word[cycles], &src_word[cycles], COPY_SRC_BIT7_0_TO_DST_BIT7_0);
    }

    return dst;
}

static char *kinetis_memcpy_within_psram(char *dst, const char *src, size_t len)
{
    int cycles = len / 2;
    int remained = len % 2;
    unsigned short *dst_word = (unsigned short *)dst;
    unsigned short *src_word = (unsigned short *)src;

    if (BOTH_ODD_ADDRESS(dst, src))
    {
        dst_word = GET_ALIGNED_ADDRESS(dst);
        src_word = GET_ALIGNED_ADDRESS(src);
        copy_byte_of_16bits_safely(dst_word, src_word, COPY_SRC_BIT15_8_TO_DST_BIT15_8);
    }
    else if (EITHER_ODD_ADDRESS(dst, src))
    {
        transfer_bytes_between_odd_and_even_address(dst, src, len);
        return dst;
    }
    else
    {
        transfer_data_per_16bits(dst_word, src_word, 1);
    }
    
    if (cycles >= 1)
    {
        transfer_data_per_16bits(&dst_word[1], &src_word[1], cycles - 1);

        if (remained)
        {
            psram_word[0] = src_word[cycles] & 0x00FF;
            psram_word[1] = dst_word[cycles] & 0xFF00;
            psram_word[0] = psram_word[0] + psram_word[1];
            transfer_data_per_16bits(&dst_word[cycles], psram_word, 1);
        }
    }

    return dst;
}

static void transfer_data_per_16bits(unsigned short *addr, unsigned short *data, size_t len)
{
    for (int i = 0;i < len;i++)
    {
        addr[i] = data[i];
    }
}

static void set_data_per_16bits(unsigned short *addr, unsigned short data, size_t words)
{
    for (int i = 0;i < words;i++)
    {
        addr[i] = data;
    }
}

static void copy_byte_of_16bits_safely(unsigned short *dst, unsigned short *src, protect_option_e opt)
{
    psram_word[0] = *dst;
    psram_word[1] = *src;
    char *byte = (char *)psram_word;

    switch (opt)
    {
        case COPY_SRC_BIT7_0_TO_DST_BIT7_0:
            *dst = byte[SOURCE_BIT7_0] | byte[DESTINATION_BIT15_8] << 8;
            break;
        case COPY_SRC_BIT7_0_TO_DST_BIT15_8:
            *dst = byte[DESTINATION_BIT7_0] | byte[SOURCE_BIT7_0] << 8;
            break;
        case COPY_SRC_BIT15_8_TO_DST_BIT7_0:
            *dst = byte[SOURCE_BIT15_8] | byte[DESTINATION_BIT15_8] << 8;
            break;
        case COPY_SRC_BIT15_8_TO_DST_BIT15_8:
            *dst = byte[DESTINATION_BIT7_0] | byte[SOURCE_BIT15_8] << 8;
            break;
        default:
            break;
    }
}

static void transfer_bytes_between_odd_and_even_address(char *dst, const char *src, size_t len)
{
    unsigned int i;
    unsigned short *dst_word;
    unsigned short *src_word;

    for (i = 0;i < len;i++)
    {
        if (IS_ODD_ADDRESS(dst)) // then src is even
        {
            dst_word = GET_ALIGNED_ADDRESS(dst);
            src_word = (unsigned short *)src;
            copy_byte_of_16bits_safely(dst_word, src_word, COPY_SRC_BIT7_0_TO_DST_BIT15_8);
        }
        else // dst is even, then src is odd
        {
            dst_word = (unsigned short *)dst;
            src_word = GET_ALIGNED_ADDRESS(src);
            copy_byte_of_16bits_safely(dst_word, src_word, COPY_SRC_BIT15_8_TO_DST_BIT7_0);
        }

        dst = dst + 1;
        src = src + 1;
    }
}
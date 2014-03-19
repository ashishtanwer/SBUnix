#ifndef _INLINE_H
#define _INLINE_H

/*

 Code Referenced by : Chris Stone's shovelos - https://code.google.com/p/shovelos - New BSD License
 */

static inline void disable_interrupt()
{

    __asm__ __volatile__("sti");
}

static inline void enable_interrupt()
{

    __asm__ __volatile__("cli");
}

static inline uint8_t inportb(uint16_t port)
{

    uint8_t ret;
    __asm__ __volatile__( "inb  %1, %0;"
            :"=a" (ret)
            :"d" (port) );
    return ret;
}

static inline void outportb(uint16_t port, uint8_t data)
{

    __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));

    //__asm__ __volatile( "outb %0, %1;"
    //		      : /* void */
    //		      : "a" (data), "d" (port));

}

#endif

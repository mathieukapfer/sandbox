MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
    SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 512K
    /* Put 'SHARED' after SRAM definition */
    SHARED     : ORIGIN = 0x20080000, LENGTH = 8k
}


SECTIONS {
}

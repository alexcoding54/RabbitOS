/** 显示时间功能 **/

#include "bootpack.h"

unsigned char read_cmos(unsigned char p)
{
    unsigned char data;
    
    io_out8(cmos_index, p);
    data = io_in8(cmos_data);
    io_out8(cmos_index, 0x80);
    
    return data;
}

unsigned get_hour_hex()
{
    return BCD_HEX(read_cmos(CMOS_CUR_HOUR));
}

unsigned get_min_hex()
{
    return BCD_HEX(read_cmos(CMOS_CUR_MIN));
}

unsigned get_sec_hex()
{
    return BCD_HEX(read_cmos(CMOS_CUR_SEC));
}

unsigned get_dom_hex()
{
    return BCD_HEX(read_cmos(CMOS_MON_DAY));
}

unsigned get_dow_hex()
{
    return BCD_HEX(read_cmos(CMOS_WEEK_DAY));
}

unsigned get_mon_hex()
{
    return BCD_HEX(read_cmos(CMOS_CUR_MON));
}

unsigned get_year_hex()
{
    return (BCD_HEX(read_cmos(CMOS_CUR_CEN))*100)+BCD_HEX(read_cmos(CMOS_CUR_YEAR))-30+2010;
}
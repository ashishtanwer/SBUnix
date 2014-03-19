#include<defs.h>
#include<stdio.h>
#include<inline.h>
#include<idt.h>
#include<handler.h>
#include<stdio.h>
#include<stdarg.h>
#include<process.h>
#include<vmm.h>
#include<scheduler.h>
#include<ahci.h>
#include<e1000.h>

int a = 0;
int time_elapsed = 0;
void handler_timer_routine()
{
    a++;
    if (a % 18 == 0)
    {
        time_elapsed++;
        if (time_elapsed % 1 == 0)
        {
            processing_wait_queue();
        }
        if (time_elapsed == 10)
        {
        }
        if (time_elapsed % 6 == 0)
        {
            schedule();
        }
        //  printingf("Divide by zero \n");
        int previous_position_x = get_position_X();
        int previous_position_y = get_position_Y();
        set_position_X_Y(60, 24);
        printk("%d", time_elapsed);
        set_position_X_Y(previous_position_x, previous_position_y);
    }
}

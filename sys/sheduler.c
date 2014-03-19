#include<defs.h>
#include<stdio.h>
#include<stdarg.h>
#include<pmm.h>
#include<process.h>
#include<scheduler.h>
//struct runqueue *first;
//struct runqueue *last;
struct queue_descriptor * descriptor;
struct queue_descriptor * wait_queue;
struct queue_descriptor * zombie_queue;
//struct file_descriptor * file_descriptor;
//struct queue_descriptor * terminal_queue;
uint8_t length = 0;
void init_queue()
{
    uint64_t physfree = get_physfree();
    // descriptor_address = physfree;
    descriptor = (struct queue_descriptor*) physfree;
    physfree = physfree + 0x50000;
    wait_queue = (struct queue_descriptor*) physfree;
    physfree = physfree + 0x50000;
    // printk("\n Physfree : %x", physfree);
    zombie_queue = (struct queue_descriptor*) physfree;
    physfree = physfree + 0x50000;
    set_physfree(physfree);
    descriptor->count = 0;
    // printk("\n Wait Queue Count %d \n", wait_queue->count);
    wait_queue->count = 0;
    // printk("\n Wait Queue Count %d \n", wait_queue->count);
    int i;
    for (i = 0; i < MAX_PROCESS; i++)
    {
        descriptor->position[i] = -1;
        wait_queue->position[i] = -1;
        zombie_queue->position[i] = -1;
    }
}
void init_process(struct task_struct *t)
{
    // descriptor->count = descriptor->count + 1;
    descriptor->queue[0] = *t;
    descriptor->position[0] = 0;
    descriptor->count = descriptor->count + 1;
}
void print_process()
{
    printk("\n  PROCESS_ID   CMD      PROCESS_STATE");
    int i = 1;
    for (; i < descriptor->count; i++)
    {
        struct task_struct * task2 = &descriptor->queue[descriptor->position[i]];
        printk("\n %d      %s        ", task2->processid, task2->name);
        if (task2->state == 1)
        {
            printk("TASK_NEW");
        }
        else if (task2->state == 2)
        {
            printk("TASK_RUNNING");
        }
        else if (task2->state == 3)
        {
            printk("TASK_INTERRUPTABLE");
        }
        else if (task2->state == 4)
        {
            printk("TASK_UNINTERRUPTABLE");
        }
        else if (task2->state == 5)
        {
            printk("TASK_STOPPED");
        }
        else if (task2->state == 6)
        {
            printk("TASK ZOMBIE");
        }
        else if (task2->state == 7)
        {
            printk("TASK WAITING");
        }
        else if (task2->state == 9)
        {
            printk("TASK TERMINAL");
        }
        else if (task2->state == 10)
        {
            printk("TASK TERMINAL RUNNING");
        }
    }
    for (i = 0; i < wait_queue->count; i++)
    {
        struct task_struct * task = &wait_queue->queue[wait_queue->position[i]];
        printk("\n %d      %s      ", task->processid, task->name);
        if (task->state == 1)
        {
            printk("TASK_NEW");
        }
        else if (task->state == 2)
        {
            printk("TASK_RUNNING");
        }
        else if (task->state == 3)
        {
            printk("TASK_INTERRUPTABLE");
        }
        else if (task->state == 4)
        {
            printk("TASK_UNINTERRUPTABLE");
        }
        else if (task->state == 5)
        {
            printk("TASK_STOPPED");
        }
        else if (task->state == 6)
        {
            printk("TASK ZOMBIE");
        }
        else if (task->state == 7)
        {
            printk("TASK WAITING");
        }
        else if (task->state == 9)
        {
            printk("TASK TERMINAL");
        }
        else if (task->state == 10)
        {
            printk("TASK TERMINAL RUNNING");
        }
    }
    for (i = 0; i < zombie_queue->count; i++)
    {
        struct task_struct * task1 =
                &zombie_queue->queue[zombie_queue->position[i]];
        printk("\n %d      %s        ", task1->processid, task1->name);
        if (task1->state == 1)
        {
            printk("TASK_NEW");
        }
        else if (task1->state == 2)
        {
            printk("TASK_RUNNING");
        }
        else if (task1->state == 3)
        {
            printk("TASK_INTERRUPTABLE");
        }
        else if (task1->state == 4)
        {
            printk("TASK_UNINTERRUPTABLE");
        }
        else if (task1->state == 5)
        {
            printk("TASK_STOPPED");
        }
        else if (task1->state == 6)
        {
            printk("TASK ZOMBIE");
        }
        else if (task1->state == 7)
        {
            printk("TASK WAITING");
        }
        else if (task1->state == 9)
        {
            printk("TASK TERMINAL");
        }
        else if (task1->state == 10)
        {
            printk("TASK TERMINAL RUNNING");
        }
    }
}
void insert_wait_queue(struct task_struct *t)
{
    //printk("\n addinf ...");
    int processid = t->processid;
    // printk("\n process id : %d",processid);
    if (processid == -1)
    {
        printk("\n Wait Queue Over Flow");
    }
    else
    {
        //  printk("\n queee....1");
        wait_queue->queue[processid] = *t;
        // printk("\n queee ....2 ");
        wait_queue->position[wait_queue->count] = processid;
        // printk("\n queee ....3 ");
        wait_queue->count = wait_queue->count + 1;
    }
    // printk("\n done with this ...");
}
struct task_struct * get_current(int processid)
{
    return &wait_queue->queue[processid];
}
struct task_struct * get_current_run(int processid)
{
    return &descriptor->queue[processid];
}
void insert_zombie_queue(struct task_struct *t)
{
    //  printk("\n added into the zombie queue with status id %d",t->status);
    int processid = t->processid;
    if (processid == -1)
    {
        printk("\n Wait Queue Over Flow");
    }
    else
    {
        zombie_queue->queue[processid] = *t;
        zombie_queue->position[zombie_queue->count] = processid;
        zombie_queue->count = zombie_queue->count + 1;
    }
}
int find_zombile_queue(int processid)
{
    int ispresent = 0;
    int i = 0;
    for (; i < zombie_queue->count; i++)
    {
        struct task_struct * task =
                &zombie_queue->queue[zombie_queue->position[i]];
        if (task->processid == processid)
        {
            // printk("\n this child process : %d just died ",task->processid);
            ispresent = 1;
            // do the cleaning up
            break;
        }
    }
    return ispresent;
}
void find_terminal_process(char * pointer)
{
    int i = 0;
    for (; i < wait_queue->count; i++)
    {
        struct task_struct * task = &wait_queue->queue[wait_queue->position[i]];
        //    printk("\n id: %d",task->processid);
        if (task->state == TASK_TERMINAL)
        {
            // printk("\n Found Terminal Task...");
            //      printk("\n task_wait_ p %d",task->wait_p);
            int i = 0;
            // printk("\n Copying ...");
            while (*pointer != '\0')
            {
                task->scan_buf[i] = *pointer;
                //	printk("%c",task->scan_buf[i]);
                pointer++;
                i++;
            }
            task->scan_buf[i] = *pointer;
            // printk("\n hhh :  %d",i);
            //   while(1);
            // printk("\n Putting Back in Queue...");
            insert(task);
            remove_wait_queue(task);
            break;
        }
    }
    //  printk("\n No terminal Task...");
}
void processing_wait_queue()
{
    int i = 0;
    for (; i < wait_queue->count; i++)
    {
        struct task_struct * task = &wait_queue->queue[wait_queue->position[i]];
        //    printk("\n id: %d",task->processid);
        if (task->state == TASK_TERMINAL)
        {
            if (task->timer > 0)
            {
                task->timer = task->timer - 1;
                if (task->timer == 1)
                {
                    task->timer = 0;
                }
            }
            else
            {
                continue;
            }
        }
        if (task->state == TASK_WAITING)
        {
            if (task->wait_p == -1)
            {
                int child_index = 0;
                for (; child_index < task->child_length; child_index++)
                {
                    int is_present = find_zombile_queue(
                            task->child_id[child_index]);
                    if (is_present == 1)
                    {
                        task->state = TASK_RUNNING;
                        insert(task);
                        remove_wait_queue(task);
                        break;
                    }
                }
            }
            else
            {
                int is_present = find_zombile_queue(task->wait_p);
                if (is_present == 1)
                {
                    task->state = TASK_RUNNING;
                    task->wait_p = -1;
                    insert(task);
                    remove_wait_queue(task);
                    break;
                }
            }
        }
        else
        {
            if (task->timer <= 0)
            {
                task->state = TASK_RUNNING;
                insert(task);
                remove_wait_queue(task);
            }
            else
            {
                task->timer = task->timer - 1;
            }
        }
    }
}
void remove_wait_queue(struct task_struct *t)
{
    int processid = t->processid;
    int i = 0;
    for (; i < wait_queue->count; i++)
    {
        if (processid == wait_queue->position[i])
            break;
    }
    //  printk("\n Place where I is %d", i);
    int c;
    for (c = i; c < (wait_queue->count) - 1; c++)
        wait_queue->position[c] = wait_queue->position[c + 1];
    wait_queue->count = wait_queue->count - 1;
}
void remove_zombie_queue(struct task_struct *t)
{
    int processid = t->processid;
    int i = 0;
    for (; i < zombie_queue->count; i++)
    {
        if (processid == zombie_queue->position[i])
            break;
    }
    //  printk("\n Place where I is %d", i);
    int c;
    for (c = i; c < (zombie_queue->count) - 1; c++)
        zombie_queue->position[c] = zombie_queue->position[c + 1];
    zombie_queue->count = zombie_queue->count - 1;
}
void insert(struct task_struct *t)
{
    int processid = t->processid;
    if (processid == -1)
    {
        printk("\n Queue Over Flow");
    }
    else
    {
        descriptor->queue[processid] = *t;
        //  printk("\n descriptor Queue ::");
        descriptor->position[descriptor->count] = processid;
        descriptor->count = descriptor->count + 1;
    }
}
struct task_struct * get_next_process()
{
    if (descriptor->count == 1)
    {
        length = 0;
        //  printk("\n No Task In Hand Scheduling init");
        return &descriptor->queue[descriptor->position[length]];
    }
    length++;
    if (length >= descriptor->count)
        length = 1;
    return &descriptor->queue[descriptor->position[length]];
}
void remove(struct task_struct *t)
{
    if (descriptor->count == 1)
    {
        return;
    }
    int processid = t->processid;
    int i = 0;
    for (; i < descriptor->count; i++)
    {
        if (processid == descriptor->position[i])
            break;
    }
    int c;
    for (c = i; c < (descriptor->count) - 1; c++)
        descriptor->position[c] = descriptor->position[c + 1];
    descriptor->count = descriptor->count - 1;
}

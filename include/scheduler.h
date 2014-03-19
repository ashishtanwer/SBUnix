#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include<defs.h>
#include<process.h>
#define  MAX_PROCESS 100

/*
 struct runqueue{

 struct task_struct *process;
 struct runqueue *next;
 struct runqueue *previous;
 // uint64_t position;


 };
 */

struct queue_descriptor
{

    uint8_t count;
    struct task_struct queue[MAX_PROCESS];
    // uint64_t processid[MAX_PROCESS];
    // uint64_t last;
    uint8_t position[MAX_PROCESS];

};

/*
 struct file_info{


 int file_start;

 int end_of_file;

 };


 struct file_descriptor{


 uint8_t count;
 struct file_info file_buf[MAX_PROCESS];
 // uint64_t processid[MAX_PROCESS];
 // uint64_t last;
 uint8_t position[MAX_PROCESS];



 };
 */

struct task_struct * get_current_run(int);

void print_process();

int find_zombile_queue(int);
void insert_zombie_queue(struct task_struct *);
void remove_zombie_queue(struct task_struct *);

void init_process(struct task_struct *);

void remove(struct task_struct *);

void init_queue();

void insert(struct task_struct *);

int findEmptyPlaces();

void insert_wait_queue(struct task_struct *);

void processing_wait_queue();

void remove_wait_queue(struct task_struct *);

struct task_struct * get_next_process();

#endif

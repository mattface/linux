#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Hart");
MODULE_DESCRIPTION("A handy module to spam junk to kmsg.");
MODULE_VERSION("0.01");

static struct timer_list my_timer;
int spam_interval = 20;
char randomString[1000];

char *randstring(size_t length) {
  static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

  int n = 0;
  for (n = 0; n < length; n++) {
      int i, key;
      get_random_bytes(&i, sizeof(i));
      key = i % (sizeof(charset) -1);
      randomString[n] = charset[key];
  }

  randomString[length] = '\0';
  return randomString;
}

void my_timer_callback( struct timer_list *t )
{
  char *spamstring;
  unsigned int j;
  int spamlength;
  get_random_bytes(&j, sizeof(j));
  spamlength = j % (1000);
  spamstring = randstring(spamlength);

  printk(KERN_EMERG "%s", spamstring);
  mod_timer( &my_timer, jiffies + msecs_to_jiffies(spam_interval));
}

static int __init spam_init(void) {
  int ret;

  printk(KERN_EMERG "Spam kmsg module installing\n");

  // my_timer.function, my_timer.data
  timer_setup( &my_timer, my_timer_callback, 0);

  printk(KERN_EMERG "Starting Spamming every %d ms (%ld)\n", spam_interval, jiffies );
  ret = mod_timer( &my_timer, jiffies + msecs_to_jiffies(spam_interval) );
  if (ret) printk("Error in mod_timer\n");
  return 0;
}

static void __exit spam_exit(void) {
  int ret;

  ret = del_timer( &my_timer );
  if (ret) printk("The timer is still in use...\n");

  printk(KERN_EMERG "Spam kmsg module uninstalling\n");

  return;
}

module_init(spam_init);
module_exit(spam_exit);

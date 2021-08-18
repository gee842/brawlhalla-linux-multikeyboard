#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <stdlib.h>
//https://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=118786
#define msleep(ms) usleep((ms)*1000)

// Translate a keyboard opened in exclusive mode into virtual button presses with uinput
// by Golden Child
//
// using examples from
//
// https://gist.github.com/matthewaveryusa/a721aad80ae89a5c69f7c964fa20fec1
// https://github.com/GrantEdwards/uinput-joystick-demo
// https://www.kernel.org/doc/html/v4.12/input/uinput.html
// https://blog.marekkraus.sk/c/linuxs-uinput-usage-tutorial-virtual-gamepad/

// hard code the keyboard to open in keyboard_to_open
// you can pass the path of the keyboard to open on the command line

 char keyboard_to_open[256] = "/dev/input/event12";


static void setup_abs(int fd, unsigned chan, int min, int max);  

// demo showing how to provide a joystick/gamepad instance from
// userspace.

int main(int argc, char* argv[])
{ sleep(1); // wait for a second
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  
  if (fd < 0)
    {
      perror("open /dev/uinput");
      return 1;
    }

  ioctl(fd, UI_SET_EVBIT, EV_KEY); // enable button/key handling

int translatetable[] = { 

KEY_SPACE, BTN_0, //jump
KEY_G, BTN_1, //drop throw
KEY_H, BTN_2, // light
KEY_J, BTN_3, // heavy
KEY_K, BTN_4, // dodge
KEY_1, BTN_5, // select
KEY_3, BTN_6, // start
KEY_2, BTN_7, // Y
KEY_W, BTN_DPAD_UP,
KEY_A, BTN_DPAD_LEFT,
KEY_S, BTN_DPAD_DOWN,
KEY_D, BTN_DPAD_RIGHT };

int translatetablelen = sizeof(translatetable) / sizeof (int);

int rcode; 


// tell uinput which BTN events we will generate
for (int i=0;i<translatetablelen;i+=2)     ioctl(fd, UI_SET_KEYBIT, translatetable[i+1]);

 char keyboard_name[256] = "qmkbuilder keyboard";
 
 int keyboard_fd;

printf ("\nparameters %d\n",argc);
for (int i=0;i<argc;i++) printf("parameter %d = %s\n",i,argv[i]);
 if (argc > 1)
 	keyboard_fd = open(argv[1], O_RDONLY | O_NONBLOCK);  
else
    keyboard_fd = open(keyboard_to_open, O_RDONLY | O_NONBLOCK);  


  if ( keyboard_fd == -1 ) {
    printf("Failed to open keyboard.\n");
    exit(1);
  }
  rcode = ioctl(keyboard_fd, EVIOCGNAME(sizeof(keyboard_name)), keyboard_name);
  printf("Reading From : %s \n", keyboard_name);

  printf("Getting exclusive access: ");
  rcode = ioctl(keyboard_fd, EVIOCGRAB, 1);
  printf("%s\n", (rcode == 0) ? "SUCCESS" : "FAILURE");


for (int i=0;i<translatetablelen;i+=2)  printf("%d => %d\n",translatetable[i],translatetable[i+1]);

  struct input_event keyboard_event;


  ioctl(fd, UI_SET_EVBIT, EV_ABS); // enable analog absolute position handling
  
  setup_abs(fd, ABS_X,  -512, 512);
  setup_abs(fd, ABS_Y,  -512, 512);
  setup_abs(fd, ABS_Z,  -32767, 32767);
  
  struct uinput_setup setup =
    {
     .name = "Userspace Joystick",
     .id =
     {
      .bustype = BUS_USB,
      .vendor  = 0x3,
      .product = 0x3,
      .version = 2,
     }
    };

  if (ioctl(fd, UI_DEV_SETUP, &setup))
    {
      perror("UI_DEV_SETUP");
      return 1;
    }
  
  if (ioctl(fd, UI_DEV_CREATE))
    {
      perror("UI_DEV_CREATE");
      return 1;
    }


int doquit=0; // must initialize variable or else it's nonzero
struct input_event button_event;
while (1) {      
      int readval;
      
      if ( readval = read(keyboard_fd, &keyboard_event, sizeof(keyboard_event)), readval != -1 ) 
      {

      	if (keyboard_event.type == EV_KEY)
      	for (int i=0;i<translatetablelen;i+=2) if (keyboard_event.code == translatetable[i]) 
      	{
      					
      		if (keyboard_event.value != 2) {  // skip key repeat events
      					
			    button_event.type = EV_KEY;
				button_event.code = translatetable[i+1];
				button_event.value = keyboard_event.value;
						
						  //printf("ev type=%d  key in=%d  => button out=%d value=%d\n",
						  	//button_event.type,
						  	//keyboard_event.code,
							//button_event.code,
							//button_event.value);
		
                write(fd, &button_event, sizeof button_event);

				// send SYNC event
				button_event.type = EV_SYN;
				button_event.code = 0;
				button_event.value = 0;

	
				write(fd, &button_event, sizeof button_event);

	
                // if(write(fd, &button_event, sizeof button_event) < 0)
                // {
                //     perror("write");
                //     printf("e");
                //     return 1;
                // }
                break;
		    }    						
		}								
 	    if (readval <= 0) msleep(0.1); 
    }
 	 // keep from taking a lot of cpu by going to sleep
}


  if(ioctl(fd, UI_DEV_DESTROY))
    {
      printf("UI_DEV_DESTROY");
      return 1;
    }

  close(fd);
  return 0;
}

// enable and configure an absolute "position" analog channel

static void setup_abs(int fd, unsigned chan, int min, int max)
{
  if (ioctl(fd, UI_SET_ABSBIT, chan))
    perror("UI_SET_ABSBIT");
  
  struct uinput_abs_setup s =
    {
     .code = chan,
     .absinfo = { .minimum = min,  .maximum = max },
    };

  if (ioctl(fd, UI_ABS_SETUP, &s))
    perror("UI_ABS_SETUP");
}


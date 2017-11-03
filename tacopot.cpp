#include <unistd.h>
#include <signal.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <X11/Shell.h>
#include <X11/XKBlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

Display* g_display = NULL;

void on_sigint(int /*sig*/)
{
	if(g_display)
	{
		XCloseDisplay(g_display);
		g_display = NULL;
	}
	printf("quitting\n");
	exit(0);
}

void trigger()
{
	printf("Attempted taco detected!\n");

	system("xscreensaver-command --lock");
	system("uvccapture -o/tmp/taco.jpg -x1280 -y720");
}

int main()
{
	signal(SIGINT, on_sigint);

	printf("Tacopot initializing...\n");
	g_display = XOpenDisplay(":0");
	if(!g_display)
	{
		printf("fail to open\n");
		return 1;
	}

	char buf[5] = "    ";

	char keymap_old[32] = {0};
	while(true)
	{
		char keymap[32];
		XQueryKeymap(g_display, keymap);

		for(int nbyte=0; nbyte<32; nbyte ++)
		{
			for(int nbit=0; nbit<8; nbit ++)
			{
				bool old = (keymap_old[nbyte] >> nbit) & 1;
				bool cur = (keymap[nbyte] >> nbit) & 1;

				if(cur && !old)
				{
					//See what key was pressed
					int key = XkbKeycodeToKeysym(g_display, nbyte*8 + nbit, 0, 0);

					//Ignore anything not alphabetical
					if((key < 'a') || (key > 'z') )
						continue;

					for(int j=0; j<3; j++)
						buf[j] = buf[j+1];
					buf[3] = key;

					if(!strcmp(buf, "taco"))
						trigger();
				}
			}
			keymap_old[nbyte] = keymap[nbyte];
		}

		usleep(10 * 1000);
	}

	//Done
	XCloseDisplay(g_display);
	return 0;
}

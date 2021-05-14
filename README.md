## Spy-Null-Driver
## A kernel module for simulating /dev/null and spying on what users write on it.

Are you trying to spy on your server's users commands ? They link their ```.bash_history``` to ```/dev/null``` so that you cant read their entered commands? Here this kernel module code can help you in changing the act of ```/dev/null``` and write every thing written on it to your **TTY**.
It's a kind of kernel-level rootkit that seems to be the kernel module behind ```/dev/null``` file; But a bit diffrent!

It hides itself by removing itself from **sysfs** and **procfs** so that noone can unload the module and interrupt spy. The only way to stop the module is to reboot the system!

The only thing that you have to do is to write your password to ```/dev/null``` and then every written bytes to ```/dev/null``` would be written to your **TTY**. For stopping the spy, you have to just write another instruction to ```/dev/null/```.

Note: You have to set **start spy** and **stop spy** pass strings by editting **start_spy** and **stop_spy** macros at the head of the source code.

Contact me: D4R30@protonmail.com

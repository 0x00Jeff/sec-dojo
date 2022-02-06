# sec-dojo
sec-dojo write ups

## Lab Description
Nix is a Linux infected machine. In this challenge, the first flag was deleted, and the second is encrypted. You should be able to locate the malware binary and analyze it using reverse engineering techniques then recover the two flags. To access the infected machine, use the following SSH credentials: - User: infected - Password: infected Some useful tools : Ghidra, IDA Pro/Free, Radare2, gdb

## first flag
when you log in to the machine via ssh and run `ps aux ` you can see there is a process running called `payload`

![payload](https://user-images.githubusercontent.com/71389295/152689187-f9148ef3-86e7-4a15-b102-7913ce4cf4fe.png)

the next thing I did is running `ltrace` to get to know what the binary tries to do

![ltrace_1](https://user-images.githubusercontent.com/71389295/152689217-a7bdf8f4-b7fc-487c-8d9c-053996c82bb0.png)

it checks the presence of a file in the path `/home/local.txt` using `access(2)`, if not present then sleep and repeat, I `touch`ed that file and tried again

![ltrace_2](https://user-images.githubusercontent.com/71389295/152689266-2c13d4d9-9489-4808-a5ce-512f16afdaa9.png)

and tries to do the same thing with `/root/proof.txt`

after creating both files I got some useful results

![ltrace_flag](https://user-images.githubusercontent.com/71389295/152689318-1015415b-9ad4-4005-be50-a2b2c96f9d2a.png)

it does some networking stuff, then opens `/root/proof.txt` , reads one character from in, writes a character to `/tmp/enc.txt` then delete that file, you can see that the flag is written to `/tmp/enc.txt`

![ltrace_flag2](https://user-images.githubusercontent.com/71389295/152689461-ea5e9aff-93f3-493e-af49-391400b7dfe0.png)


## second flag

getting back to the binary trace, you can see the following

![etc_hosts](https://user-images.githubusercontent.com/71389295/152689622-f02d44d0-d339-4998-8fc9-be34f1717c6e.png)

the binary opens `/etc/hosts`, then writes a string to it, beginning with `192.168.23.23\t1stflag`, let's check the content of that file

![second_flag](https://user-images.githubusercontent.com/71389295/152689696-037fcb21-5474-458f-9214-28632c963b01.png)


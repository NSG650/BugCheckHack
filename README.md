# BugCheckHack
Modify your BSOD!

# Note

This works on real hardware but I would recommend you to use this under a virtual machine

Make sure you have internet access as this tool downloads pdb files for getting offsets of required functions.

Windows builds from 19041.* to current 22000.556 should work

The service created by the program is Automatic and the driver will load at startup

You can change the service setting by using the `sc` tool

# How to use

1) Enable test signing by running
```
bcdedit /set testsigning on
````
2) Copy over `BugCheckHack.sys` to your Windows folder

3) Launch the program as an administrator

![Program](https://cdn.discordapp.com/attachments/855872050132811796/978613277586243605/unknown.png)

4) Fill in your values and click `Save values`

5) Then create the service and then run service

6) If everything went correctly you should get a message saying "Successfully modified the BSOD"

# Aero BSOD

This option `0xc3s` [ret] out `BgpClearScreen` so whatever was present on the screen will be displayed during the BSOD

# Images

![WeDontKnowwhereyourPCis](https://cdn.discordapp.com/attachments/855872050132811796/978650524045963264/unknown.png)
![OmameBSOD](https://cdn.discordapp.com/attachments/413430340388913171/978623156875055134/unknown.png)
[Cat's owner](https://omame.xyz/)

# Resources used

[Libvalinet](https://github.com/valinet/libvalinet/) for downloading and finding offsets in `ntoskrnl.exe` using the pdb.

[Unknowncheats](https://www.unknowncheats.me/forum/general-programming-and-reversing/427419-getkernelbase.html) for getting the kernel base address in the driver.

[Stackoverflow](https://stackoverflow.com/a/41808496) for figuring out how to make the program look nicer and not something from Windows 3.1.
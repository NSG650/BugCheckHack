# BugCheckHack
Modify your BSOD!

# How to use

1) Enable test signing by running
```
bcdedit /set testsigning on
````
2) Using SC create a service and run it
```
sc create BugCheckHack binPath=C:\where\ever\the\driver\is\BugCheckHack.sys type=kernel start=auto
```
3) Create a registry key at HKLM\SOFTWARE and called it BugCheckHack
4) Create 3 string values and name them String1, String2 and Emoticon put your desired text in them.

5) Now run the driver with
```
sc start BugCheckHack
```

6) If it failed with a status code of STATUS_UNSUCCESSFUL then it worked!

# Image

![Funny Image](https://cdn.discordapp.com/attachments/413430340388913171/891746727663640606/unknown.png)

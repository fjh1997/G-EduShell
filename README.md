# G-EduShell
A minimalist reverse shell trojan for educational purposes, co-authored with Google Gemini.
# Platform
Windows
# Build
Edit ip then use MSVC build tool:
```
cl /utf-8  shell.c /link /SUBSYSTEM:WINDOWS ws2_32.lib
```
# Usage
Run  shell.exe and use another server listening to 8877 port:  
```
ncat -lk 8877
```
And wait for reverse shell connection
# Usage Warning

Do not use this code on any server or network without explicit, written permission from the owner.

The author is not responsible for any misuse, damage, or illegal activities caused by this program.

Engaging in unauthorized access to computer systems is illegal and carries severe criminal penalties.

By using this repository, you agree to use this software exclusively for legal, white-hat security research.

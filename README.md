### Cebola - AntiVirus Software  
Currently, I am planning to develop only a macOS-compatible version.  

## What it does when started  
When launched, the software scans all executables on the system for viruses. The process follows these steps:

- Creating a list of all files on the system  
- Filtering only executable files (with `.app` and `mach-o` formats)  
- Scanning these files for known virus signatures  
- If a virus is detected, the user is presented with the following options:
  1. Remove the file (if it is removable)  
  2. Clean the file (if possible)  
  3. Reboot the system in safe mode and then remove the file  

### Cebola - AntiVirus Software
Now i'm planning to do only a linux compatible verison.
## What it does when started
- Scan all executables on System for viruses, done by:
    - making a list of all files
    - filterning only executables (.exe and .elf extensions)
    - scanning these files for known virus's signatures
    - when it detects a virus, it gives the user some options:
        1. remove the file (if the file itself is removable)
        2. clean the file (if possible)
        3. reboot the system in safe mode and then remove the file


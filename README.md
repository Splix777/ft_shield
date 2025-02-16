<h1 align="center">ft_shield</h1>
<h2 align="center">Bonne Année.</h2>

<div align="center">
<img src="docs/readme_images/trojan.png" alt="shield" width="25%">
</div>

<h1 align="center">(🚧Bonus still under construction...🚧)</h1>

# Table of Contents
- [Introduction](#introduction)
- [Objectives](#objectives)
- [General Instructions](#general-instructions)
- [Mandatory Part](#mandatory-part)
- [Instructions](#instructions)
- [Bonus](#bonus)

# Introduction

A trojan horse is a special kind of malicious software, or malware, often mistaken with
virus or parasites. The trojan horse looks like a legit software, but it contains a malicious
parasite. The goal of the trojan horse is to get this parasite inside the computer and
install it unbeknownst to the user.

The contained program is called `"payload"`. It can be any kind of parasite: `virus,
keylogger,spyware...` This parasite is the only element that will execute the actions
inside the victim’s computer. The trojan horse is just the vehicle that "puts the fox in
charge of the henhouse". It’s not malicious itself because it can’t execute any action,
other than installing the real parasite.

In common language, the parasite is often called "trojan horse" by metonymy. This
confusion is often fueled by antivirus publishers.They use "trojan" as a generic name to
describe different kinds of malware that are all but trojan horses.
3


# Objectives

This project follows `Matt_daemon` as well as `Dr_quine`, which aim to make you create a
simple trojan horse!

This trojan will not be complicated in its core, but with a few bonuses and a little
drive, you will quickly understand how you can improve your program.
4


# General Instructions
ft_shield is a binary of your own design that will only launch with the root rights.

This program will be harmless at first (hehe...). When launching ft_shield, it will
only show your login on the standard output. NEVERTHELESS, in the background,
it will have made so much more

# Mandatory Part

### Program Requirements:
This is trojan, for you... But what’s the use, you’ll ask me? Let me tell you:

1. **Executable Name:** `ft_shield`
    - ft_shield will create another program also called ft_shield, that will be located
in the folder containing all the targeted OS binaries and will execute.
    - This newly created program will have to be executed when the targeted machine is
turned on. You will decide of the launching method. Be creative.
    - This program will be launched as a background process just like a daemon.


    - You should be able to launch only one daemon instance.
    - The daemon will have to plug into 4242 port.
    - It has to offer a 3 clients connection simultaneously.
    - When a client connects to the daemon, a password will be required. We ask for a
    secured password (a password in clear in the code is a 0, period).
    - When connection is established with a client, the daemon must offer to launch a
    shell with root rights. You will not be able to access the shell with the shell
    command.


### Constraints:
- Warning! No error message must appear during the creation or the use
of the binary on the targeted machine!

### Example Use Case:

```bash
# ls -al /bin/ft_shield
ls: cannot access /bin/ft_shield: No such file or directory
# service --status-all | grep ft_shield
# ./ft_shield
wandre
# ls -al /bin/ft_shield
-rwxr-xr-x 1 root root 12384 Apr 4 14:02 /bin/ft_shield
# service --status-all | grep ft_shield
[ + ] ft_shield
# service ft_shield status
. ft_shield.service - (null)
Loaded: loaded (/etc/init.d/ft_shield)
Active: active (running) since Mon 2016-04-04 14:08:18 CEST; 1s ago
Process: 10986 ExecStart=/etc/init.d/ft_shield start (code=exited, status=0/SUCCESS)
CGroup: /system.slice/ft_shield.service
-> 10988 /bin/ft_shield
# su wandre
$ nc localhost 4242
Keycode: 42
Keycode: 4242
$>
$> ?
? show help
shell Spawn remote shell on 4242
$> shell
Spawning shell on port 4242
$ nc localhost 4242
id
uid=0(root) gid=0(root) groups=0(root)
exit
$ nc localhost 4242
Keycode: ^C
$ su
# netstat -tulnp | grep 4242
tcp 0 0 0.0.0.0:4242 0.0.0.0:* LISTEN 10988/ft_shield
#
```

## Conclusion
The `ft_shield` project introduces you to the fascinating, albeit ethically challenging, world of trojan horse creation. By designing a program that operates stealthily with root privileges, you've not only honed your skills in system programming and security but also gained insight into how malware can infiltrate and persist in a system. 

This project demonstrates:

- Stealth Installation: The ability to hide in plain sight by placing itself among system binaries and running as a daemon.

- Secure Access: Implementing a secure connection method that requires authentication, showcasing basic principles of network security.
Controlled Execution Environment: Offering a shell with elevated privileges in a controlled manner, ensuring only authorized users can exploit this feature.

Remember, while this project is educational and designed for learning purposes, the principles and techniques involved here are those often used maliciously in the real world. It's crucial to use this knowledge responsibly, contributing to cybersecurity by developing secure systems rather than exploiting vulnerabilities. 

Keep exploring, learning, and innovating in the field of cybersecurity, and consider how you can apply these skills to protect rather than compromise. Happy coding!

## Bonus
TODO
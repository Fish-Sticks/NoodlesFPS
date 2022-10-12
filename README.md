# NoodlesFPS
KernelMode FPS Unlocker, written by fishy

All the code here was written by fishy.

# Build:
Build in x64 Release mode.
If you get a certificate error right click on Project, go to Properties and click on "Driver Signing"
Inside the Driver Signing tab set the Sign Mode to "Test Sign"
Set the Test Certificate to "Create test certificate"
Hit apply to save changes and rebuild.

# Setup:
Open command prompt as admin and type
"bcdedit /set TESTSIGNING ON"
and then restart your computer.

Once restarted, open command prompt as admin and type
sc create NoodlesFPS type=kernel binPath="{PATH_HERE}\NoodlesFPS.sys"


# Usage:
Before opening Roblox run the driver by opening command prompt as admin and typing
"sc start NoodlesFPS"
Once the driver is running start Roblox.

Once Roblox is fully loaded, open Calculator.exe by pressing windows key + r and typing
"calc.exe"
and hitting enter.


**Do not open calculator before opening Roblox or you will BSOD lmao.**
[img]https://gyazo.com/636d347bd68892ed41910de557badb7a[/img]

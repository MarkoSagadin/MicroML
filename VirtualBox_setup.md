## Prerequsites
1. Download Oracle VM VirtualBox and install it
2. Download Ubuntu Server 18.04.3 image file
3. Download Putty

## Virtual Ubuntu setup
1. Open VirtualBox and click New
2. Give it a name (example: LinuxDev), and choose Linux 2.6 / 3.x / 4.x (64-bit)
3. Give it enough ram, more is better, leave something for your host, click next
4. Chose option create a virtual hard disk now
5. Chose VDI option
6. Chose Dynamicaly allocated
7. Chose location for it and size of virtual. You fill 8GB up quite fast so give it more.

## Installing Ubuntu
1. Select created virtual machine and click run
2. Select Ubuntu image file that you downloaded earlier
3. Go through setup, no special options are needed, make sure that you install SSH server
4. Finish installation and close Ubuntu. You can choose option to reboot sooner.

## SSH setup
1. Run Ubuntu in headless mode
2. Open settings->Network->Advanced->Port forwarding
3. Add a rule with Name SSH, Host port is 2222, guest port is 22, click Ok
4. Open Putty and connect to localhost, port 2222, you should save it
5. Now you can connect to your virtual machine with Putty.

## Ubuntu dev tools
1. Run sudo apt update and sudo apt upgrade to get packages
2. Then install other tools:
```
sudo apt install make git gcc g++ unzip bash wget openocd gdb-multiarch gcc-arm-none-eabi cmake minicom
```
## Installing VistualBox Guest Additions and creating a shared folder
1. Make sure that Ubuntu is already running, go to VirtualBox and click Show button
2. Click Devices->Insert Guest Additions CD image
3. Go back to putty session and write: 
```
sudo mkdir /media/cdrom
sudo mount -t auto /dev/cdrom /media/cdrom
cd /media/cdrom
sudo ./VBoxLinuxAdditions.run
```
4. Everything is now prepared on guest side, we can do sudo poweroff 
5. Create a shared folder on desktop called VM_shared
6. Go to VM VirtualBox Manager, select virtual machine and go into Settings->Shared Folders
7. Add a shared folder by clicking a plus button on the right, provide a folder path and check Auto-mount, click ok
8. Run virtual machine in headless mode, connect to it over Putty
9. We need to add us to a certian group so we can access the folder
    sudo adduser <user> vboxsf
10. Close and reconnect with putty
11. You should be able to access to shared folder with `cd /media/sf_VM_shared`

## Setting up st-link to flash programs to microcontrollers
1. Run following commands:
```
sudo apt-get install libusb-1.0-0-dev
git clone https://github.com/texane/stlink stlink
cd stlink
make
```
2. Install binaries:
```
sudo cp build/Release/st-* /usr/local/bin
```
3. Install udev rules
```
sudo cp etc/udev/rules.d/49-stlinkv* /etc/udev/rules.d/
```
4. And restart udev
```
sudo udevadm control --reload
```
5. Connect st-link to PC and open VirtualBox manager.
6. Go under Settings->USB and click add new device (plus icon), select stlink and add it.
7. Unplug and plug in again. If you write lsusb in putty you should see Stlink device.

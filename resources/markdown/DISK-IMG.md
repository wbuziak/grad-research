# Build a disk image for RISC-V ubuntu
This is for a full-system disk image for [gem5](https://www.gem5.org/) simulations.

This documentation is adapted from the gem5 disk image [documentation](https://www.gem5.org/documentation/general_docs/fullsystem/disks), and lots and lots of trial & error.

## Dependencies

 - A statically compiled version of [RISC-V qemu](https://risc-v-getting-started-guide.readthedocs.io/en/latest/linux-qemu.html)
 - [RISC-V Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)

## Setup env

Note: If you are starting from scratch, you can largely follow the setup and then scroll further to your desired benchmark.

 - If you need additional help, there are random notes and commands down at the bottom

# Compiling gem5

First, download all required binaries to a dedicated folder:

```
mkdir fs_files
```

```
wget https://dist.gem5.org/dist/develop/kernels/riscv/static/linux-6.5.5
wget https://dist.gem5.org/dist/develop/kernels/riscv/static/riscv-bootloader-opensbi-1.3.1-20231129
mv riscv-bootloader-opensbi-1.3.1-20231129 riscv-bootloader-opensbi-1.3.1
wget https://dist.gem5.org/dist/develop/images/riscv/riscv-ubuntu-22.04.gz
gunzip riscv-ubuntu-22.04.gz
```

Mount the disk image (spam until it works - seriously):
```
sudo python3 util/gem5img.py mount fs_files/riscv-ubuntu-22.04 mnt
```

Chroot into the disk image, mount devices and begin installing

```
sudo chroot mnt /bin/bash
mount -t proc proc /proc
mount -t sysfs sys /sys
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
```

May be necessary to fix broken symlink for DNS:

```
rm -f /etc/resolv.conf
echo "nameserver 8.8.8.8" > /etc/resolv.conf 
echo "127.0.1.1 $(hostname)" >> /etc/hosts
```

Install necessary dependents and download gem5

```
apt-get update
sudo apt install -y vim m4 build-essential git scons gcc g++ autotools-dev
cd /home/gem5
git clone https://github.com/gem5/gem5.git
cd gem5/util/m5
```

Cross-compile m5 & make visible system-wide:

```
scons riscv.CROSS_COMPILE="" build/riscv/out/m5
sudo cp build/riscv/out/m5 /usr/local/bin/
sudo chmod +x /usr/local/bin/m5
```

# SPEC CPU 2026

This begins with the assumption that you have a SPEC binary / license downloaded on the host machine you wish to set up your disk image and run gem5.

We will need 7zip for unpacking the ISO:

```
sudo apt-get install -y p7zip-full
```

Before we configure, we need to reformat the disk image for more space:

```
qemu-img resize riscv-ubuntu-22.04 +30G
```

Mount the image to a loop device to repartition the disk:

```
sudo losetup -Pf /home/wbuziak/repos/gem5/fs_files/riscv-ubuntu-22.04
```

Check what device it has been attached to:

```
losetup -a
```

Resize the partitions (assuming loop0):

```
sudo parted /dev/loop0 resizepart 1 100%
sudo partprobe /dev/loop0
sudo e2fsck -f /dev/loop0
sudo resize2fs /dev/loop0
```

Cleanup the loop device:

```
sudo losetup -d /dev/loop0
```

Copy the qemu binary into the mounted mnt folder:

```
sudo cp /usr/bin/qemu-riscv64-static mnt/usr/bin/
```

Mount the image and create another mount point for the SPEC ISO

```
sudo python3 util/gem5img.py mount fs_files/riscv-ubuntu-22.04 mnt
```

Copy over the SPEC installation scripts:

```
mkdir -p /tmp/spec_iso
sudo 7z x fs_files/cpu2026-1.0.1.iso -o./mnt/tmp/spec_iso
```

Chroot into the image and run the SPEC installation script:

```
sudo chroot mnt /bin/bash
mount -t proc proc /proc
mount -t sysfs sys /sys
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts

cd /tmp/spec_iso
chmod -R +x .
./install.sh -d /home/gem5/spec
```

Now, we move on to establishing gem5 hooks:

Within the desired benchmark's src directory:

```
cd benchspec/CPU/782.lbm_r/src
vi main.c
```

Add the gem5 header:

```
#include <gem5/m5ops.h>
```

Then, add the m5 exit function calls like so:

```
/* --- AFTER EDITING --- */
/* Mark Start of Region of Interest (ROI) */
m5_work_begin(0, 0);

/* Perform time steps */
for (n = 0; n < nTimeSteps; n++) {
    LBM_performStreamCollide(&grid);
}

/* Mark End of Region of Interest (ROI) */
m5_work_end(0, 0);

/* Terminate gem5 simulation cleanly */
m5_exit(0);
```

Add a config file at ```/home/gem5/spec/config/riscv64.cfg```:

```
cat << 'EOF' > /home/gem5/spec/config/riscv64.cfg
# Global Settings
strict_rundir_verify = 0
check_md5            = 0
ignore_errors        = 1
tune                 = base
label                = riscv64
reportable           = no

# GCC config for RISC-V 64-bit with gem5 m5ops support
default:
CC                  = gcc -std=c99
CXX                 = g++ -std=c++11
FC                  = gfortran

CC_VERSION_OPTION   = --version
CXX_VERSION_OPTION  = --version
FC_VERSION_OPTION   = --version

# Base Optimization
OPTIMIZE            = -O2

# ---------------------------------------------------------
# gem5 m5ops Injection Flags
# ---------------------------------------------------------
# Ensure the compiler can find <gem5/m5ops.h>
EXTRA_CFLAGS        = -I/home/gem5/gem5/include
EXTRA_CXXFLAGS      = -I/home/gem5/gem5/include
EXTRA_FFLAGS        = -I/home/gem5/gem5/include

# Ensure the linker can find and link libm5.a
EXTRA_LDFLAGS       = -L/home/gem5/gem5/util/m5/build/riscv/out
EXTRA_LIBS          = -lm5
EOF
```

Finally, we can build and verify:

```
cd /home/gem5/spec
source shrc
runcpu --config=riscv64.cfg --action=setup --size=ref --noreport -I 782.lbm_r
```

```
cd benchspec/CPU/782.lbm_r/run/run*
nm lbm_r* | grep m5_
```

Ensure that you see the T m5_work_ function symbols on output, if so, you should be ready to run from gem5.

# Parsec

This is not working as of Oct 17 2026:
 - The issue is that the gem5 hooks will not activate when running inside gem5, due to issues with the hooks and parsecmgmt. 

Download the parsec benchmark:

```
cd /home/gem5
wget https://github.com/cirosantilli/parsec-benchmark/releases/download/3.0/parsec-3.0-core.tar.gz
tar -xvf parsec-3.0-core.tar.gz
rm parsec-3.0-core.tar.gz
mv parsec-3.0 parsec-benchmark
```

We now modify the hooks to recognize gem5 exit conditions

Open ```pks/libs/hooks/src/hooks.c```

```
vi parsec-benchmark/pkgs/libs/hooks/src/hooks.c
```

Add the ```#include <gem5/m5ops.h>``` header at the top:

```
#include "include/hooks.h"
#include "config.h"

#include <stdio.h>
#include <assert.h>
#include <gem5/m5ops.h>
```

Then, ensure that the roi_begin() & roi_end() have the appropriate m5_work instruction:

```
void __parsec_roi_begin() {
    // Existing hook code...

    /* gem5 ROI Begin Hook */
    m5_work_begin(0, 0);
}

void __parsec_roi_end() {
    /* gem5 ROI End Hook */
    m5_work_end(0, 0);

    // Existing hook code...
}
```

Modify the hooks build configuration with the following flags:

```
vi parsec-benchmark/pkgs/libs/hooks/parsec/gcc-hooks.bldconf
```

```
#!/bin/bash

# gcc-hooks.bldconf - configuration file for PARSEC

source ${PARSECDIR}/pkgs/libs/hooks/parsec/gcc.bldconf

# Path to gem5 root inside the guest image
GEM5_DIR="/home/gem5/gem5"

# Append include and library paths
INCPATH="${INCPATH} -I${GEM5_DIR}/include"
LIBPATH="${LIBPATH} -L${GEM5_DIR}/util/m5/build/riscv/out"
LIBS="${LIBS} -lm5"
```

Update the config files to avoid future errors:

```
find /home/gem5/parsec-benchmark -type f -name "config.guess" -exec cp /usr/share/misc/config.guess {} \;
find /home/gem5/parsec-benchmark -type f -name "config.sub" -exec cp /usr/share/misc/config.sub {} \;
```

Replace hardcoded x86 flags with riscv flags:

```
sed -i 's|-DM5OP_ADDR=0xFFFF0000 -DM5OP_PIC -Iinclude|-I/home/gem5/gem5/include|g' /home/gem5/parsec-benchmark/pkgs/libs/hooks/src/Makefile.shared
sed -i 's/OBJS = hooks.lo$/OBJS = hooks.lo m5op_x86.lo/g' /home/gem5/parsec-benchmark/pkgs/libs/hooks/src/Makefile.shared
```

Then, update the build configurations:

```
echo 'LDFLAGS="${LDFLAGS} -L/home/gem5/gem5/util/m5/build/riscv/out"' >> /home/gem5/parsec-benchmark/config/gcc-hooks.bldconf
echo 'LIBS="${LIBS} -lm5"' >> /home/gem5/parsec-benchmark/config/gcc-hooks.bldconf
```

Finally, we compile the benchmark:

```
echo 'export PARSECDIR="$(pwd)"' > /home/gem5/parsec-benchmark/env.sh
echo 'export PATH="${PARSECDIR}/bin:${PATH}"' >> /home/gem5/parsec-benchmark/env.sh
cd parsec-benchmark
source env.sh
parsecmgmt -a build -p blackscholes -c gcc-hooks
```

Alternatively, you can compile all benchmarks:

```
parsecmgmt -a build -p all -c gcc-hooks
```

# Microbenchmark

To compile a micro-benchmark from within the disk image that links the m5 binary and is capable of enacting the gem5 exit conditions:

You must first add the gem5 m5 binary as well as the include folder from the host machine.

Mount the disk image and create the following file structure:

microbenchmark/

  |--- bin/

  |--- lib/

  ||---- libm5.a

  |--- src/

  ||---- micro.c

  |--- include/

  ||---- m5_mmap.h 

  ||---- gem5/ 

  |||---- m5ops.h  

Ensure that you have the following include paths within the header of the microbenchmark:

```
#include <gem5/m5ops.h>
#include "m5_mmap.h"
```

Then, compile as:
```
gcc -no-pie -DGEM5 -I include/ src/micro.c -L lib/ -lm5 -o bin/micro
```

If you fail due to the absence of a /dev/mem device, you can create it with the following:

```
mknod /dev/mem c 1 1
chmod 660 /dev/mem
```

## Extra Notes

# Binaries

In order to successfully simulate a full-system architecture, you must have compatible OS, bootloader and kernel binaries. Many can be found at the [gem5 resources]() repository.

X86:

  - Parsec:
 ```
 wget https://gem5dist.blob.core.windows.net/dist/develop/images/x86/ubuntu-18-04/parsec.img.gz
 ```
 - linux kernel:
 ```
 wget https://gem5dist.blob.core.windows.net/dist/develop/kernels/x86/static/vmlinux-5.2.3
 ```

- Disk Image: (standard ubuntu disk image)
 ```
 wget https://dist.gem5.org/dist/develop/images/x86/x86-ubuntu-22-04.gz
 ```


RISC-V:

  - Bootloader:
  ```
  wget https://dist.gem5.org/dist/develop/kernels/riscv/static/riscv-bootloader-opensbi-1.3.1-20231129
  ```
  - Kernel:
  ```
  wget https://dist.gem5.org/dist/develop/kernels/riscv/static/linux-6.5.5
  ```
  - Disk Image:
  ```
  wget https://dist.gem5.org/dist/develop/images/riscv/riscv-ubuntu-22.04.gz
  ```


```
mkdir mnt
```
## Load RISC-V disk image contents with gem5 binaries, etc.
This can also be done from within a file, see gem5 resources

```
wget https://gem5resources.blob.core.windows.net/dist-gem5-org/dist/develop/images/riscv/riscv-ubuntu-22.04.gz
gzip -d riscv-ubuntu-22.04.gz
```

## Mount the image, if error, keep retrying
```
sudo python3 util/gem5img.py mount resources/binaries/riscv-ubuntu-22.04 mnt
```

To check loop devices:

```
losetup -a
```

Clear any un-wanted loop devices:
```
sudo umount -R mnt 2>/dev/null
sudo losetup -D
```

If you experience the following error message:

```
mount: /PATH-TO-MOUNT/mnt: wrong fs type, bad option, bad superblock on /dev/loop0p1, missing codepage or helper program, or other error.
  dmesg(1) may have more information after failed mount system call.
```

The problem is most likely an issue with the mounting partition type, you can use the following to find the correct partition manually.

Attach with partition scanning:
```
sudo losetup -fP fs_files/x86-ubuntu-22-04
```
Find the correct partition with ```lsblk```

We are looking for an ```EXT4``` partition type:

```
sudo dmesg | tail -n 20
```

Check for the ```TYPE="ext4"``` partition (likely /deb/loop0p2):

```
sudo blkid /dev/loop0p2
```

Once you have found the device partition with the appropriate type, mount it manually:

```
sudo mount /dev/loop0p2 mnt
```

## If you need to unmount
You should unmount after you are done
```
sudo python3 util/gem5img.py umount mnt
```

## Copy appropriate system files onto image
This is important for a few files that the image will need to borrow from a real system 

This gives the disk image the ability to update and install packages from apt
```
sudo cp /etc/resolv.conf mnt/etc/ --remove-destination 
sudo /bin/mount -o bind /dev/null mnt/dev/null
```

## Chroot into the image to open a shell within
Try:
```
sudo chroot mnt qemu-riscv64-static /bin/bash
```

If that does not work, you can chroot using:

```
sudo chroot mnt
```

If you have further issues with chroot, it is likely because the disk image is compiled for a different ISA than the host machine, you will need to install qemu-static:

```
sudo apt install qemu-user-static binfmt-support
```

Then, copy the binary to the mounted folder:

```
sudo cp /usr/bin/qemu-x86_64-static /mnt/chroot/usr/bin/
```

## change permissions 
```
chmod 777 mnt
chown -R man: /var/cache/man/
```

```exit```  optional for full shell

## Update and clone repositories 

```
apt update
apt install git
```

Now, clone any repo you like

Connect with a terminal:

```
./util/term/m5term localhost 3456
```

## For gem5
For riscv, to cross compile the m5 binary, this can be done from the ~/gem5/util/m5 directory in a similar manner to how you build the gem5 binary.
```
scons riscv.CROSS_COMPILE=/home/wbuziak/../../opt/riscv/bin/riscv64-unknown-linux-gnu- build/riscv/out/m5
```

Install the RISC-V GNU compiler:

```
sudo apt install g++-riscv64-linux-gnu gcc-riscv64-linux-gnu
```

You will then want to copy this binary onto your disk image sbin/ folder, as per the [disk-image](https://www.gem5.org/documentation/general_docs/fullsystem/disks) documentation.

For x86

Install the x86 compiler (for a host machine with another ISA):

```
sudo apt install gcc-x86-64-linux-gnu g++-x86-64-linux-gnu
```

You can compile the m5 binary for x86 with:

```
scons build/x86/out/m5 x86.CROSS_COMPILE=x86_64-linux-gnu-
```

-----

You may also want to compile [the util/term/m5term binary](https://www.gem5.org/documentation/general_docs/fullsystem/m5term) that allows you to connect to a FS serial terminal with the following command:

```
./util/term/m5term localhost 3456
```

-----

Finally, ensure that you have a proper init script that loads your readfile contents that contain the shell commands for your benchmark. It will look something like sbin/gem5_init.sh:

```
#!/bin/sh
m5 readfile > script.sh
if [ -s script.sh ]; then
    # if the file is not empty then execute it
    chmod +x script.sh
    ./script.sh
    m5 exit
# otherwise, drop to terminal/exit simulation
else
    # Directly log in as the gem5 user
    printf "Dropping to shell as gem5 user...\n"
    exec su - gem5
fi

m5 exit
```

Note: if you experience issues, check that sbin/gem5_init.sh is fully executable.



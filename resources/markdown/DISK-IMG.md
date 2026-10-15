# Build a disk image for RISC-V ubuntu
This is for a full-system disk image for [gem5](https://www.gem5.org/) simulations.

This documentation is adapted from the gem5 disk image [documentation](https://www.gem5.org/documentation/general_docs/fullsystem/disks).

## Dependencies

 - A statically compiled version of [RISC-V qemu](https://risc-v-getting-started-guide.readthedocs.io/en/latest/linux-qemu.html)
 - [RISC-V Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)

## Setup env

Note: If you are starting from scratch, you can largely follow this in-order.

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

# Parsec

For compiling parsec onto a disk image in RISC-V:

If you haven't already, download the disk image:

```
wget https://dist.gem5.org/dist/develop/images/riscv/riscv-ubuntu-22.04.gz
```

Mount devices & update

In order to update, we first have to mount filesystems:

```
mount -t proc proc /proc
mount -t sysfs sys /sys
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
```

It may be necessary to fix the broken symlink for the DNS:

```
rm -f /etc/resolv.conf
echo "nameserver 8.8.8.8" > /etc/resolv.conf 
echo "127.0.1.1 $(hostname)" >> /etc/hosts
```

Now, we should be ready to update & install dependencies:

```
apt-get update
sudo apt install -y vim m4 build-essential git scons gcc g++ autotools-dev
```

Clone gem5 in the disk image:

```
cd /home/gem5
git clone https://github.com/gem5/gem5.git
cd gem5/util/m5
```

Compile the m5 binary within the disk image:

```
scons riscv.CROSS_COMPILE="" build/riscv/out/m5
```

Copy the m5 binary and enable it system wide:

```
sudo cp build/riscv/out/m5 /usr/local/bin/
sudo chmod +x /usr/local/bin/m5
```

Now, we download the parsec benchmark:

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
source env.sh
parsecmgmt -a build -p blackscholes -c gcc-hooks
```

Alternatively, you can compile all benchmarks:

```
parsecmgmt -a build -p all -c gcc-hooks
```

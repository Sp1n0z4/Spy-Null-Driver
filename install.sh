#!/bin/bash

# Installer for fake null driver. Written by Darcy (D4R30@protonmail.com). GITHUB: github.com/D4R30


def_majornumber=300
sayhelp () {
  echo -e "
    Fake Null Driver installer.

    Syntax:
      ./install.sh MODE OS-BASE
    MODE:

      InstallAll: Install required package(Linux kernel headers), then compile and install.
      CompileAndInstall: Compile and install the kernel module.
      install: Just install the kernel module.
      compile: Just compile the kernel module.
      recover: Remove fake null driver file in /dev and setup the normal driver.(Removing the module is not possible)

      Default is \"InstallAll\"

    OS-BASE (Required just for \"CompileAndInstall\" mode.):
      Enter your operating system base. Debian, arch and RedHat are supported.

      Default is \"Arch\"!

    Example:
      ./install.sh compileandinstall debian
      ./install.sh install
  "
}

# Install Linux kernel headers(Required for compilation).
installheaders () {
  echo Installing Linux kernel headers...
  if [ $1 == 0 ]
  then
    apt install linux-headers-$(uname -r)
  elif [ $1 == 1 ]
  then
    pacman -S linux-headers-$(uname -r)
  elif [ $1 == 2 ]
  then
    yum install kernel-devel
  else
    echo "Using arch as default os..."
    pacman -S linux-headers-$(uname -r)
  fi
}

# Insert module to the kernel
installmodule () {
  echo Inserting the module...
  insmod $(pwd)/code.ko debugmod=0 major_num=$def_majornumber
  rm /dev/null
  mknod /dev/null c $def_majornumber 0
}

# Compile the source
compile () {
  echo Compiling the source...
  if [ ! -r "MakeFile" ]
  then
	  echo '"MakeFile" does not exist or read permission is not granted. Please download and locate the file in the current directory.'
	  exit
  fi

  make
}

stringmode=${1,,}
os=${2,,}

# Checking installation mode.
if [ $stringmode == "installall" ]
then

# Checking entered os name.
  if [ $os == "arch" ]
  then
    osnum=0
  elif [ $os == "debian" ]
  then
    osnum=1
  elif [ $os == "redhat" ]
  then
    osnum=2
  else
    echo "Not supported or not understood OS name: $2."
    osnum=0
  fi

  installheaders $osnum
  compile
  installmodule

elif [ $stringmode == "compileandinstall" ]
then
  compile
  installmodule
elif [ $stringmode == "install" ]
then
  installmodule
elif [ $stringmode == "compile" ]
then
  compile
elif [ $stringmode == "help" ]
then
  sayhelp
elif [ $stringmode == "recover" ]
then
  echo Removing /dev/null...
  rm /dev/null
  echo Making the special file for the read null driver...
  mknod /dev/null c 1 3
else
  echo -e "Can not understand entred mode. Using \"InstallAll\" as default."
  installheaders $osnum
  compile
  installmodule
fi

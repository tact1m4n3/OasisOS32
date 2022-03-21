FROM ubuntu:16.04

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y make
RUN apt-get install -y nasm
RUN apt-get install -y gcc binutils libc6-dev-i386
RUN apt-get install -y python3
RUN apt-get install -y xorriso
RUN apt-get install -y grub-common grub-pc-bin

VOLUME /env
WORKDIR /env
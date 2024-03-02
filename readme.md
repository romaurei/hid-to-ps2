# HID TO PS/2

This project aims to create a full hardware and software solution for an adaptor that can translate the input from any modern Mouse and Keyboard to a PS/2 output compatible with old systems.

## General requirements

* The hardware must have a USB host with two type A ports.
* The hardware must have two PS/2 outputs compatible with old systems for mouse and keyboard.
* The hardware must be powered by the PS/2 port/s from the host PC.
* The USB Ports should support standard HID mouse and keyboard.
* The device should support regular wired mouse and keyboard, as well as USB dongles capable of receiving multiple devices in one port (Logitech style).

## Hardware Platform

Preliminairly, this project is intended to run in a Microchip SAMD21 microcontroller as USB host.

A custom PCB should be designed for the final implementation.

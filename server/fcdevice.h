/*
 * Fadecandy device interface
 * 
 * Copyright (c) 2013 Micah Elizabeth Scott
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include "usbdevice.h"
#include <set>


class FCDevice : public USBDevice
{
public:
    FCDevice(libusb_device *device, bool verbose);
    virtual ~FCDevice();

    static bool probe(libusb_device *device);

    virtual int open();
    virtual bool matchConfiguration(const Value &config);
    virtual void writeMessage(const OPCSink::Message &msg);
    virtual void writeColorCorrection(const Value &color);
    virtual std::string getName();

    static const unsigned NUM_PIXELS = 512;

    const char *getSerial() { return mSerial; }

    // Send current buffer contents
    void writeFramebuffer();

    // Framebuffer accessor
    uint8_t *fbPixel(unsigned num) {
        return &mFramebuffer[num / PIXELS_PER_PACKET].data[3 * (num % PIXELS_PER_PACKET)];
    }
 
private:
    static const unsigned PIXELS_PER_PACKET = 21;
    static const unsigned LUT_ENTRIES_PER_PACKET = 31;
    static const unsigned FRAMEBUFFER_PACKETS = 25;
    static const unsigned LUT_PACKETS = 25;
    static const unsigned LUT_ENTRIES = 257;
    static const unsigned OUT_ENDPOINT = 1;

    static const uint8_t TYPE_FRAMEBUFFER = 0x00;
    static const uint8_t TYPE_LUT = 0x40;
    static const uint8_t TYPE_CONFIG = 0x80;
    static const uint8_t FINAL = 0x20;

    static const uint8_t CFLAG_NO_DITHERING     = (1 << 0);
    static const uint8_t CFLAG_NO_INTERPOLATION = (1 << 1);
    static const uint8_t CFLAG_NO_ACTIVITY_LED  = (1 << 2);
    static const uint8_t CFLAG_LED_CONTROL      = (1 << 3);

    struct Packet {
        uint8_t control;
        uint8_t data[63];
    };

    struct Transfer {
        Transfer(FCDevice *device, void *buffer, int length);
        ~Transfer();
        libusb_transfer *transfer;
        FCDevice *device;
    };

    const Value *mConfigMap;
    std::set<Transfer*> mPending;

    char mSerial[256];
    libusb_device_descriptor mDD;
    Packet mFramebuffer[FRAMEBUFFER_PACKETS];
    Packet mColorLUT[LUT_PACKETS];
    Packet mFirmwareConfig;

    void submitTransfer(Transfer *fct);
    void configureDevice(const Value &config);
    void writeFirmwareConfiguration();
    static void completeTransfer(struct libusb_transfer *transfer);

    void opcSetPixelColors(const OPCSink::Message &msg);
    void opcSysEx(const OPCSink::Message &msg);
    void opcSetGlobalColorCorrection(const OPCSink::Message &msg);
    void opcSetFirmwareConfiguration(const OPCSink::Message &msg);
    void opcMapPixelColors(const OPCSink::Message &msg, const Value &inst);
};

# camera-linux-qt

### dependencies 
```
sudo apt-get install qtbase5-dev qtmultimedia5-dev qttools5-dev 
sudo apt-get install libasound2-dev libopus-dev libvpx-dev libjpeg-dev
sudo apt-get install v4l2loopback-dkms
```
```
git clone https://chromium.googlesource.com/libyuv/libyuv
cd libyuv/build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -Wno-dev ..
sudo make all install
```

### build
```
git clone https://github.com/pshvedko/camera-linux-qt.git
cd camera-linux-qt
mkdir build
cd build
cmake ..
make
```

### pre-run
```
sudo modprobe v4l2loopback exclusive_caps=1
sudo modprobe snd-aloop
```

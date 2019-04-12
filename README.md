Fitipower 18e3:5031 USB 双目摄像头 Opencv 测试代码
===================================================

功能：打开摄像头，设置成双目模式。按 ESC 键退出，按 S 键保存图像到当前目录（按键需要在选中图像显示窗口的情况下才有效）。

依赖于 libwebcam https://sourceforge.net/projects/libwebcam/

此摄像头默认只输出单目图像，参考 https://www.jianshu.com/p/2ce302d588a0

使用：

安装 libwebcam，并且根据实际情况修改 CMakeLists.txt 中 include_directories 跟 LINK_DIRECTORIES
根据实际情况修改 CAM_ID，CAM_DEVICE 宏定义

``
mkdir build
cd build
cmake ..
make
./VideoCapture
``


